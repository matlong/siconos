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

/*!\file lcp_avi_caoferris.c
 \brief Solve an LCP by reformulating it as an AVI and the solver by Cao and
Ferris solves the subsequent AVI.
 \author Olivier Huber
*/

#include "LinearComplementarityProblem.h"
#include "avi_caoferris.h"
#include "AVI_Solvers.h"
#include "LCP_Solvers.h"
#include <assert.h>

void lcp_avi_caoferris(LinearComplementarityProblem* problem, double *z, double *w, int *info, SolverOptions* options)
{
  unsigned int n = problem->size;
  assert(n > 0);

  double* d_vec = (double *)malloc(n*sizeof(double));
  for (unsigned i = 0; i < n; ++i) d_vec[i] = -1.0;

  /* Set of active constraint is trivial */
  unsigned* A = (unsigned*)malloc(n*sizeof(unsigned));
  for (unsigned i = 0; i < n; ++i) A[i] = i + 1;

  /* Call directly the 3rd stage */
  *info = avi_caoferris_stage3(problem, w, z, d_vec, n, A, options);

  /* free allocated stuff */
  free(A);
  free(d_vec);
}