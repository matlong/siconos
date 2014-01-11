#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "NonSmoothDrivers.h"
#include "frictionContact_test_function.h"
#include "projectionOnCone.h"

#include "SiconosBlas.h"

typedef struct {
  VariationalInequality * vi;
  FrictionContactProblem * fc3d;
} Problems;


void Ftest(void * self, double *x, double *F)
{
  VariationalInequality * vi = (VariationalInequality *) self;
  Problems* pb = (Problems *)vi->env;             
  FrictionContactProblem * fc3d = pb->fc3d;
  //frictionContact_display(fc3d);
 
  int nc = fc3d->numberOfContacts;
  int nLocal =  fc3d->dimension;
  int n = nc * nLocal; 

  cblas_dcopy(n , fc3d->q , 1 , F, 1);
  prodNumericsMatrix(n, n, 1.0, fc3d->M, x, 1.0, F);
  int contact =0;
 
  for (contact = 0 ; contact < nc ; ++contact)
  {
    int pos = contact * nLocal;
    double  normUT = sqrt(F[pos + 1] * F[pos + 1] + F[pos + 2] * F[pos + 2]);
    F[pos] +=  (fc3d->mu[contact] * normUT);
  }
}


void PXtest(void *viIn, double *x, double *PX)
{
  VariationalInequality * vi = (VariationalInequality *) viIn;
  Problems* pb = (Problems *)vi->env;             
  FrictionContactProblem * fc3d = pb->fc3d;
  //frictionContact_display(fc3d);
  
  int contact =0; 
  int nc = fc3d->numberOfContacts;
  int nLocal =  fc3d->dimension;
  int n = nc * nLocal; 

  cblas_dcopy(n , x , 1 , PX, 1);
  
  for (contact = 0 ; contact < nc ; ++contact)
  {
    int pos = contact * nLocal;
    projectionOnCone(&PX[pos], fc3d->mu[contact]);
  }
}



int main(void)
{
  VariationalInequality vi;
  //vi.self = &vi;
  vi.F = &Ftest;
  vi.ProjectionOnX = &PXtest;

  NumericsOptions global_options;
  setDefaultNumericsOptions(&global_options);
  global_options.verboseMode = 1; // turn verbose mode to off by default

  SolverOptions * options = (SolverOptions *) malloc(sizeof(SolverOptions));
  int info = variationalInequality_setDefaultSolverOptions(options, SICONOS_VI_EG);
  options->dparam[0]=1e-8;
  
  FILE * finput  =  fopen("./data/Example1_Fc3D_SBM.dat", "r");
  FrictionContactProblem* problem = (FrictionContactProblem *)malloc(sizeof(FrictionContactProblem));
  info = frictionContact_newFromFile(problem, finput);
  // frictionContact_display(problem);

  Problems *pb= (Problems *)malloc(sizeof(Problems));
  vi.env = pb;


  pb->vi = &vi;
  pb->fc3d = problem;
  frictionContact_display(pb->fc3d);

  int n = problem->numberOfContacts * problem->dimension;
  vi.size=n;

  double *x = (double*)malloc(n* sizeof(double));
  double *w = (double*)malloc(n* sizeof(double));

  PXtest(&vi, x,w);

 
  info = variationalInequality_driver(&vi, 
                                      x, 
                                      w, 
                                      options, 
                                      &global_options);
  int i =0;
  for (i =0; i< n ; i++)
  {
    printf("x[%i]=%f\t",i,x[i]);    printf("w[%i]=F[%i]=%f\n",i,i,w[i]);
  }
 
  deleteSolverOptions(options);
  free(options);
  free(problem);
  free(x);
  free(w);
}