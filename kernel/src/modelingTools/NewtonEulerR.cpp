/* Siconos-Kernel, Copyright INRIA 2005-2012.
* Siconos is a program dedicated to modeling, simulation and control
* of non smooth dynamical systems.
* Siconos is a free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
* Siconos is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Siconos; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*
* Contact: Vincent ACARY, siconos-team@lists.gforge.inria.fr
*/

// \todo : create a work vector for all tmp vectors used in computeg, computeh ...

#include <cstdio>

#include "NewtonEulerR.hpp"
#include "Interaction.hpp"
#include "NewtonEulerDS.hpp"

#include "BlockVector.hpp"
#include "SimulationTypeDef.hpp"


//#define NER_DEBUG
//#define DEBUG_STDOUT
//#define DEBUG_MESSAGES
#include <debug.h>


#include <iostream>


void NewtonEulerR::initComponents(Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM)
{

  DEBUG_PRINT("NewtonEulerR::initComponents(Interaction& inter) starts\n");

  unsigned int ySize = inter.getSizeOfY();
  unsigned int xSize = inter.getSizeOfDS();
  unsigned int qSize = 7 * (xSize / 6);

  if (! _jachq)
    _jachq.reset(new SimpleMatrix(ySize, qSize));
  else
  {
    if (_jachq->size(0) == 0)
    {
      // if the matrix dim are null
      _jachq->resize(ySize, qSize);
    }
    else
    {
      assert((_jachq->size(1) == qSize && _jachq->size(0) == ySize) ||
             (printf("NewtonEuler::initComponents _jachq->size(1) = %d ,_qsize = %d , _jachq->size(0) = %d ,_ysize =%d \n", _jachq->size(1), qSize, _jachq->size(0), ySize) && false) ||
             ("NewtonEuler::initComponents inconsistent sizes between _jachq matrix and the interaction." && false));
    }
  }

  DEBUG_EXPR(_jachq->display());

  if (! _jachqT)
    _jachqT.reset(new SimpleMatrix(ySize, xSize));



  //_jachqT.reset(new SimpleMatrix(ySize, xSize));


  DEBUG_EXPR(_jachqT->display());


  DEBUG_PRINT("NewtonEulerR::initComponents(Interaction& inter) ends\n");
}

void NewtonEulerR::setJachq(SP::SimpleMatrix newJachq)
{
  _jachq = newJachq;
}

void NewtonEulerR::setJachqPtr(SP::SimpleMatrix newPtr)
{
  _jachq = newPtr ;
}


void NewtonEulerR::initialize(Interaction& inter, VectorOfBlockVectors& DSlink, VectorOfVectors& workV, VectorOfSMatrices& workM)
{
  // Memory allocation for G[i], if required (depends on the chosen constructor).
  initComponents(inter, DSlink, workV, workM);

  _contactForce.reset(new SiconosVector(DSlink[NewtonEulerR::p1]->size()));
  _contactForce->zero();
}


void NewtonEulerR::computeh(double time, BlockVector& q0, SiconosVector& y)
{
  prod(*_jachq, q0, y, true);
  if (_e)
    y += *_e;
}

void NewtonEulerR::computeDotJachq(double time, SiconosVector& workQ, SiconosVector& workZ, SiconosVector& workQdot)
{
  if (_plugindotjacqh)
  {
    if (_plugindotjacqh->fPtr)
    {
      ((FPtr2)(_plugindotjacqh->fPtr))(workQ.size(), &(workQ)(0), workQdot.size(), &(workQdot)(0), &(*_dotjachq)(0, 0), workZ.size(), &(workZ)(0));
      // Copy data that might have been changed in the plug-in call.
    }
  }
}

void  NewtonEulerR::computeSecondOrderTimeDerivativeTerms(double time, Interaction& inter, VectorOfBlockVectors& DSlink, SP::DynamicalSystem ds1, SP::DynamicalSystem ds2)
{
  DEBUG_PRINT("NewtonEulerR::computeSecondOrderTimeDerivativeTerms starts\n");

  // Compute the time derivative of the Jacobian
    if (!_dotjachq) // lazy initialization
  {
    unsigned int sizeY = inter.getSizeOfY();
    unsigned int xSize = inter.getSizeOfDS();
    unsigned int qSize = 7 * (xSize / 6);

    _dotjachq.reset(new SimpleMatrix(sizeY, qSize));
  }
  // Compute the product of the time derivative of the Jacobian with dotq
  SiconosVector workQdot = *DSlink[NewtonEulerR::dotq]; // we assume that dotq is up to date !
  SiconosVector workQ = *DSlink[NewtonEulerR::q0]; // we assume that dotq is up to date !
  SiconosVector workZ = *DSlink[NewtonEulerR::z]; // we assume that dotq is up to date !
  DEBUG_EXPR(workQdot.display(););
  computeDotJachq(time, workQ, workZ, workQdot);
  _secondOrderTimeDerivativeTerms.reset(new SiconosVector(_dotjachq->size(0)));

  DEBUG_EXPR(_dotjachq->display(););

  prod(1.0,*_dotjachq, workQdot, *_secondOrderTimeDerivativeTerms, true);


  DEBUG_EXPR(_secondOrderTimeDerivativeTerms->display());

  // Compute the product of jachq and Tdot --> jachqTdot

  unsigned int k = 0;
  DSIterator itDS;
  unsigned int ySize = inter.getSizeOfY();
  unsigned int xSize = inter.getSizeOfDS();
  SP::SimpleMatrix auxBloc(new SimpleMatrix(ySize, 7));
  SP::SimpleMatrix auxBloc2(new SimpleMatrix(ySize, 6));
  Index dimIndex(2);
  Index startIndex(4);
  
  SP::SimpleMatrix jachqTdot(new SimpleMatrix(ySize, xSize));
  bool endl = false;
  for (SP::DynamicalSystem ds = ds1; !endl; ds = ds2)
  {
    endl = (ds == ds2);
    
    startIndex[0] = 0;
    startIndex[1] = 7 * k / 6;
    startIndex[2] = 0;
    startIndex[3] = 0;
    dimIndex[0] = ySize;
    dimIndex[1] = 7;
    setBlock(_jachq, auxBloc, dimIndex, startIndex);

    NewtonEulerDS& d = *std11::static_pointer_cast<NewtonEulerDS> (ds);
    d.computeTdot();
    SiconosMatrix& Tdot = *d.Tdot();

    DEBUG_EXPR(d.display());
    DEBUG_EXPR((d.Tdot())->display());

    prod(*auxBloc, Tdot, *auxBloc2);

    startIndex[0] = 0;
    startIndex[1] = 0;
    startIndex[2] = 0;
    startIndex[3] = k;
    dimIndex[0] = ySize;
    dimIndex[1] = 6;

    setBlock(auxBloc2, jachqTdot, dimIndex, startIndex);
    DEBUG_EXPR(jachqTdot->display());

    k += ds->getDim();
  }

  // compute the product of jachqTdot and v
  SiconosVector workVelocity = *DSlink[NewtonEulerR::velocity];
  DEBUG_EXPR(workVelocity.display(););
  prod(1.0, *jachqTdot, workVelocity, *_secondOrderTimeDerivativeTerms, false);
  DEBUG_EXPR(_secondOrderTimeDerivativeTerms->display());
  DEBUG_PRINT("NewtonEulerR::computeSecondOrderTimeDerivativeTerms ends\n");

  *DSlink[NewtonEulerR::z] = workZ;
}


void NewtonEulerR::computeOutput(double time, Interaction& inter, InteractionProperties& interProp, unsigned int derivativeNumber)
{

  /*\warning : implemented for the bouncing ball !!!!*/

  VectorOfBlockVectors& DSlink = *interProp.DSlink;
  DEBUG_PRINT("NewtonEulerR::computeOutput ");
  DEBUG_PRINTF("with time = %f and derivativeNumber = %i starts\n", time, derivativeNumber);

  SiconosVector& y = *inter.y(derivativeNumber);
  if (derivativeNumber == 0)
  {
    computeh(time, *DSlink[NewtonEulerR::q0], y);
  }
  else
  {
    /* \warning V.A. 30/05/2013
     *  This part is not very clear :
     *  computeJachq should compute Jachq
     *  but we use instead _jachqT which is not updated in this method !!
     */
    computeJachq(time, inter, DSlink);
    DEBUG_EXPR(_jachq->display(););

    if (derivativeNumber == 1)
    {
      assert(_jachqT);
      assert(DSlink[NewtonEulerR::velocity]);
      DEBUG_EXPR(_jachqT->display(););
      prod(*_jachqT, *DSlink[NewtonEulerR::velocity], y);
    }
    else if (derivativeNumber == 2)
    {

      RuntimeException::selfThrow("Warning: we attempt to call NewtonEulerR::computeOutput(double time, Interaction& inter, InteractionProperties& interProp, unsigned int derivativeNumber) for derivativeNumber=2");
    }
    else
      RuntimeException::selfThrow("NewtonEulerR::computeOutput(double time, Interaction& inter, InteractionProperties& interProp, unsigned int derivativeNumber) derivativeNumber out of range or not yet implemented.");
  }
  DEBUG_PRINT("NewtonEulerR::computeOutput(double time, Interaction& inter, InteractionProperties& interProp, unsigned int derivativeNumber)  ends\n")

}

/** to compute p
*  \param double : current time
*  \Param unsigned int: "derivative" order of lambda used to compute input
*/
void NewtonEulerR::computeInput(double time, Interaction& inter, InteractionProperties& interProp, unsigned int level)
{
  /*\warning : implemented for the bouncing ball !!!!*/
  VectorOfBlockVectors& DSlink = *interProp.DSlink;

  DEBUG_PRINT("NewtonEulerR::computeInput(double time, Interaction& inter, InteractionProperties& interProp, unsigned int level) starts \n")
  DEBUG_PRINTF("with time = %f and level = %i starts\n", time, level);
  DEBUG_EXPR(printf("interaction %p\n",&inter););
  DEBUG_EXPR(inter.display(););

  // get lambda of the concerned interaction
  SiconosVector& lambda = *inter.lambda(level);

  DEBUG_EXPR(lambda.display(););
  DEBUG_EXPR(DSlink[NewtonEulerR::p0 + level]->display(););

  if (level == 1) /* \warning : we assume that ContactForce is given by lambda[level] */
  {
    prod(lambda, *_jachqT, *_contactForce, true);
#ifdef NER_DEBUG
    {
      printf("NewtonEulerR::computeInput contact force :");
      _contactForce->display();
    }
#endif
    /*data is a pointer of memory associated to a dynamical system*/
    /** false because it consists in doing a sum*/
    prod(lambda, *_jachqT, *DSlink[NewtonEulerR::p0 + level], false);
#ifdef NER_DEBUG
    {
      std::cout << "_jachqT" << std::endl;
      _jachqT->display();
      std::cout << "data[p0+level]" << DSlink[NewtonEulerR::p0 + level] <<  std::endl;
      std::cout << "data[p0+level]->vector(0)" << DSlink[NewtonEulerR::p0 + level]->vector(0) <<  std::endl;
      if (DSlink[NewtonEulerR::p0 + level]->getNumberOfBlocks() > 1)
        std::cout << "data[p0+level]->vector(1)" << DSlink[NewtonEulerR::p0 + level]->vector(1) <<  std::endl;
      DSlink[NewtonEulerR::p0 + level]->display();


      SP::SiconosVector buffer(new SiconosVector(DSlink[NewtonEulerR::p0 + level]->size()));
      prod(lambda, *_jachqT, *buffer, true);
      std::cout << "added part to p" << buffer <<  std::endl;
      buffer->display();

      printf("NewtonEulerR::computeInput end for level %i\n", level);
    }
#endif
  }

  else if (level == 2) /* \warning : we assume that ContactForce is given by lambda[level] */
  {
    prod(lambda, *_jachqT, *_contactForce, true);
    DEBUG_EXPR(_contactForce->display(););

    /*data is a pointer of memory associated to a dynamical system*/
    /** false because it consists in doing a sum*/
    assert(DSlink[NewtonEulerR::p0 + level]);
    prod(lambda, *_jachqT, *DSlink[NewtonEulerR::p0 + level], false);

#ifdef NER_DEBUG
    {
      DEBUG_EXPR(_jachqT->display(););
      //  std::cout << "data[p0+level]" << DSlink[NewtonEulerR::p0 + level] <<  std::endl;
      //  std::cout << "data[p0+level]->vector(0)" << DSlink[NewtonEulerR::p0 + level]->vector(0) <<  std::endl;
      // if (DSlink[NewtonEulerR::p0 + level]->getNumberOfBlocks() > 1)
      //    std::cout << "data[p0+level]->vector(1)" << DSlink[NewtonEulerR::p0 + level]->vector(1) <<  std::endl;
      DEBUG_EXPR(DSlink[NewtonEulerR::p0 + level]->display(););

      SP::SiconosVector buffer(new SiconosVector(DSlink[NewtonEulerR::p0 + level]->size()));
      prod(lambda, *_jachqT, *buffer, true);
      std::cout << "added part to p   " << buffer <<  std::endl;
      buffer->display();

      printf("NewtonEulerR::computeInput end for level %i \n", level);
    }
#endif
  }
  else if (level == 0)
  {
    prod(lambda, *_jachq, *DSlink[NewtonEulerR::p0 + level], false);
#ifdef NER_DEBUG
    std::cout << "_jachq" << std::endl;
    _jachq->display();
    std::cout << "data[p0+level]" << DSlink[NewtonEulerR::p0 + level] <<  std::endl;
    std::cout << "data[p0+level]->vector(0)" << DSlink[NewtonEulerR::p0 + level]->vector(0) <<  std::endl;
    if (DSlink[NewtonEulerR::p0 + level]->getNumberOfBlocks() > 1)
      std::cout << "data[p0+level]->vector(1)" << DSlink[NewtonEulerR::p0 + level]->vector(1) <<  std::endl;
    DSlink[NewtonEulerR::p0 + level]->display();


    SP::SiconosVector buffer(new SiconosVector(DSlink[NewtonEulerR::p0 + level]->size()));
    prod(lambda, *_jachq, *buffer, true);
    std::cout << "added part to p" << buffer <<  std::endl;
    buffer->display();

    printf("NewtonEulerR::computeInput end for level %i:", level);
    printf("\n");
#endif

  }
  else
    RuntimeException::selfThrow("NewtonEulerR::computeInput(double time, Interaction& inter, InteractionProperties& interProp, unsigned int level)  not yet implemented for level > 1");
  DEBUG_PRINT("NewtonEulerR::computeInput(double time, Interaction& inter, InteractionProperties& interProp, unsigned int level) ends\n");
}

/*It computes _jachqT=_jachq*T. Uploaded in the case of an unilateral constraint (NewtonEulerFrom3DLocalFrameR and NewtonEulerFrom1DLocalFrameR)*/

void NewtonEulerR::computeJachqT(Interaction& inter, SP::DynamicalSystem ds1, SP::DynamicalSystem ds2)
{
  DEBUG_PRINT("NewtonEulerR::computeJachqT(Interaction& inter) starts \n");
  DEBUG_PRINTF("with inter =  %p\n",&inter);
  DEBUG_EXPR(inter.display());

  unsigned int k = 0;
  unsigned int ySize = inter.getSizeOfY();
  SP::SimpleMatrix auxBloc(new SimpleMatrix(ySize, 7));
  SP::SimpleMatrix auxBloc2(new SimpleMatrix(ySize, 6));
  Index dimIndex(2);
  Index startIndex(4);
  bool endl = false;
  for (SP::DynamicalSystem ds = ds1; !endl; ds = ds2)
  {
    endl = (ds == ds2);
    startIndex[0] = 0;
    startIndex[1] = 7 * k / 6;
    startIndex[2] = 0;
    startIndex[3] = 0;
    dimIndex[0] = ySize;
    dimIndex[1] = 7;
    setBlock(_jachq, auxBloc, dimIndex, startIndex);

    NewtonEulerDS& d = *std11::static_pointer_cast<NewtonEulerDS> (ds);
    SiconosMatrix& T = *d.T();

    DEBUG_EXPR(d.display());
    DEBUG_EXPR((d.T())->display());

    prod(*auxBloc, T, *auxBloc2);

    startIndex[0] = 0;
    startIndex[1] = 0;
    startIndex[2] = 0;
    startIndex[3] = k;
    dimIndex[0] = ySize;
    dimIndex[1] = 6;

    setBlock(auxBloc2, _jachqT, dimIndex, startIndex);
    DEBUG_EXPR(_jachqT->display());

    k += ds->getDim();
  }

  DEBUG_PRINT("NewtonEulerR::computeJachqT(Interaction& inter) ends \n")
}

void NewtonEulerR::computeJach(double time, Interaction& inter, InteractionProperties& interProp)
{
  VectorOfBlockVectors& DSlink = *interProp.DSlink;
  computeJachq(time, inter, DSlink);
  //computeJachqDot(time, inter); // This is not needed here
  //computeDotJachq(time, inter);
  computeJachlambda(time, inter);
}