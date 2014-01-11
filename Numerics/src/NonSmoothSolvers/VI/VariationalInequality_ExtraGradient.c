/* Siconos-Numerics, Copyright INRIA 2005-2012.
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
//#include "projectionOnCone.h"
#include "VariationalInequality_Solvers.h"
#include "VariationalInequality_computeError.h"
#include "SiconosBlas.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void variationalInequality_ExtraGradient(VariationalInequality* problem, double *x, double *w, int* info, SolverOptions* options)
{
  /* /\* int and double parameters *\/ */
  int* iparam = options->iparam;
  double* dparam = options->dparam;
  /* Number of contacts */
  int n = problem->size;
  /* Maximum number of iterations */
  int itermax = iparam[0];
  /* Tolerance */
  double tolerance = dparam[0];


  /*****  Fixed point iterations *****/
  int iter = 0; /* Current iteration number */
  double error = 1.; /* Current error */
  int hasNotConverged = 1;
  dparam[0] = dparam[2]; // set the tolerance for the local solver


  double * xtmp = (double *)malloc(n * sizeof(double));
  double * wtmp = (double *)malloc(n * sizeof(double));

  double rho = 0.0, rho_k =0.0;
  int isVariable = 0;

  if (dparam[3] > 0.0)
  {
    rho = dparam[3];
    if (verbose > 0)
    {
      printf("----------------------------------- VI - Extra Gradient (EG) - Fixed stepsize with  rho = %14.7e \n", rho);
    }
  }
  else
  {
    /* Variable step in iterations*/
    isVariable = 1;
    rho = -dparam[3];
    if (verbose > 0)
    {
      printf("----------------------------------- VI - Extra Gradient (EG) - Variable stepsize with starting rho = %14.7e \n", rho);
    }
     
  }

  /* Variable for Line_search */
  int success =0;
  double error_k;
  int ls_iter = 0;
  int ls_itermax = 10;
  double tau=0.6, L= 0.9, Lmin =0.3, taumin=0.7;
  double a1=0.0, a2=0.0;
  double * x_k =0;
  double * w_k =0;

  if (isVariable)
  {
    x_k = (double *)malloc(n * sizeof(double));
    w_k = (double *)malloc(n * sizeof(double));
  }

  //isVariable=0;
  if (!isVariable)
  {
    /*   double minusrho  = -1.0*rho; */
    while ((iter < itermax) && (hasNotConverged > 0))
    {
      ++iter;

      /* xtmp <- x  */
      cblas_dcopy(n , x , 1 , xtmp, 1);

      /* wtmp <- F(xtmp) */
      problem->F(problem,xtmp,wtmp);
      
      /* xtmp <- xtmp - F(xtmp) */
      cblas_daxpy(n, -1.0, wtmp , 1, xtmp , 1) ;
      
      /* wtmp <-  ProjectionOnX(xtmp) */
      problem->ProjectionOnX(problem,xtmp,wtmp);
      
      /* x <- x - wtmp */
      cblas_daxpy(n, -1.0, wtmp , 1, x , 1) ;
      
      /* x <-  ProjectionOnX(x) */
      cblas_dcopy(n , xtmp , 1 , x, 1);

      problem->ProjectionOnX(problem,xtmp,x);


      /* problem->F(problem,x,w); */
      /* cblas_daxpy(n, -1.0, w , 1, x , 1) ; */
      /* cblas_dcopy(n , x , 1 , xtmp, 1); */
      /* problem->ProjectionOnX(problem,xtmp,x); */

      /* **** Criterium convergence **** */
      variationalInequality_computeError(problem, x , w, tolerance, options, &error);
      

      
      if (options->callback)
      {
        options->callback->endIteration(options->callback->env, n,
                                        x, w,
                                        error);
      }

      if (verbose > 0)
      {
        printf("----------------------------------- VI - Extra Gradient (EG) - Iteration %i rho = %14.7e \tError = %14.7e\n", iter, rho, error);
      }
      if (error < tolerance) hasNotConverged = 0;
      *info = hasNotConverged;
    }
  }

  if (isVariable)
  {
    while ((iter < itermax) && (hasNotConverged > 0))
    {
      ++iter;


      /* Store the error */
      error_k = error;

      /* x_k <-- x store the x at the beginning of the iteration */
      cblas_dcopy(n , x , 1 , x_k, 1);

      problem->F(problem,x,w_k);

      ls_iter = 0 ;
      success =0;

      while (!success && (ls_iter < ls_itermax))
      {

        rho_k = rho * pow(tau,ls_iter);

        /* x <- x - rho_k*  w_k */
        cblas_daxpy(n, -rho_k, w_k , 1, x , 1) ;
        
        /* wtmp <-  ProjectionOnX(xtmp) */
        cblas_dcopy(n , x , 1 , xtmp, 1);
        problem->ProjectionOnX(problem,xtmp,x);
        
        problem->F(problem,x,w);

        /* velocitytmp <- velocity */
        cblas_dcopy(n, w, 1, wtmp , 1) ;


        /* velocitytmp <- velocity - velocity_k   */
        cblas_daxpy(n, -1.0, w_k , 1, wtmp , 1) ;

        a1 = cblas_dnrm2(n, wtmp, 1);

        /* reactiontmp <- reaction */
        cblas_dcopy(n, x, 1,xtmp , 1) ;

        /* reactiontmp <- reaction - reaction_k   */
        cblas_daxpy(n, -1.0, x_k , 1, xtmp , 1) ;

        a2 = cblas_dnrm2(n, xtmp, 1) ;

        success = (rho_k*a1 < L * a2)?1:0;

        /* printf("rho_k = %12.8e\t", rho_k); */
        /* printf("a1 = %12.8e\t", a1); */
        /* printf("a2 = %12.8e\t", a2); */
        /* printf("norm reaction = %12.8e\t",cblas_dnrm2(n, x, 1) ); */
        /* printf("success = %i\n", success); */

        ls_iter++;
      }
      /* velocitytmp <- q  */
      /* cblas_dcopy(n , q , 1 , velocitytmp, 1); */

      /* prodNumericsMatrix(n, n, alpha, M, reaction, beta, velocitytmp); */

      problem->F(problem,x,wtmp);

      /* x <- x - rho_k*  wtmp */
      cblas_daxpy(n, -rho_k, wtmp , 1, x , 1) ;
      
      /* wtmp <-  ProjectionOnX(xtmp) */
      cblas_dcopy(n , x , 1 , xtmp, 1);
      problem->ProjectionOnX(problem,xtmp,x);

     

      /* **** Criterium convergence **** */
      variationalInequality_computeError(problem, x , w, tolerance, options, &error);

      /*Update rho*/
      if ((rho_k*a1 < Lmin * a2) && (error < error_k))
      {
        rho =rho_k/taumin;
      }
      else
        rho =rho_k;


      if (verbose > 0)
      {
        printf("----------------------------------- VI - Extra Gradient (EG) - Iteration %i rho = %14.7e \tError = %14.7e\n", iter, rho, error);
      }
      if (error < tolerance) hasNotConverged = 0;
      *info = hasNotConverged;
    }
  }


  if (verbose > 0)
  {
    printf("----------------------------------- VI - Extra Gradient (EG) - #Iteration %i Final Error = %14.7e\n", iter, error);
  }
  dparam[0] = tolerance;
  dparam[1] = error;
  iparam[7] = iter;
  free(xtmp);
  free(wtmp);

}


int variationalInequality_ExtraGradient_setDefaultSolverOptions(SolverOptions* options)
{
  int i;
  if (verbose > 0)
  {
    printf("Set the Default SolverOptions for the ExtraGradient Solver\n");
  }

  options->solverId = SICONOS_VI_EG;
  options->numberOfInternalSolvers = 0;
  options->isSet = 1;
  options->filterOn = 1;
  options->iSize = 8;
  options->dSize = 8;
  options->iparam = (int *)malloc(options->iSize * sizeof(int));
  options->dparam = (double *)malloc(options->dSize * sizeof(double));
  options->dWork = NULL;
  options->iWork = NULL;   options->callback = NULL; options->numericsOptions = NULL;
  for (i = 0; i < 8; i++)
  {
    options->iparam[i] = 0;
    options->dparam[i] = 0.0;
  }
  options->iparam[0] = 20000;
  options->dparam[0] = 1e-3;
  options->dparam[3] = 1e-3;
  options->dparam[3] = -1.0; // rho is variable by default
  
  options->internalSolvers = NULL;

  return 0;
}