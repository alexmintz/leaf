/* spearman/test.c
 *
 * Copyright (C) 2012 Timothee Flutre
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_test.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_ieee_utils.h>
#include <gsl/gsl_nan.h>
#include <gsl/gsl_statistics.h>
#include "leaf/gsl_stats_spearman.h"

extern "C" {
void test_gsl_stats_spearman ()
{
  size_t i;
  const size_t na = 14, nb = 14;
  size_t stridea = 1, strideb = 1;
  const double rawa[] =
    {.0421, .0941, .1064, .0242, .1331,
     .0773, .0243, .0815, .1186, .0356,
     .0728, .0999, .0614, .0479};
  const double rawb[] =
    {.1081, .0986, .1566, .1961, .1125,
     .1942, .1079, .1021, .1583, .1673,
     .1675, .1856, .1688, .1512};
  double * groupa = (double *) malloc (stridea * na * sizeof(double));
  double * groupb = (double *) malloc (strideb * nb * sizeof(double));
  for (i = 0 ; i < na ; i++)
    groupa[i * stridea] = (double) rawa[i] ;
  for (i = 0 ; i < nb ; i++)
    groupb[i * strideb] = (double) rawb[i] ;

  // check with current test for Pearson coef
  double r = gsl_stats_correlation(groupa, stridea, groupb, strideb, nb);
  double expected = -0.112322712666074171;
  if(r != expected)
    gsl_test_rel (r, expected, 0.000001, //GSL_DBL_EPSILON,
		  "error in Pearson coef: %.18f (exp) != %.18f (obs)\n",
		  expected, r);

  // get answer from R:
  // groupa <- c(.0421, .0941, .1064, .0242, .1331, .0773, .0243, .0815, .1186, .0356, .0728, .0999, .0614, .0479)
  // groupb <- c(.1081, .0986, .1566, .1961, .1125, .1942, .1079, .1021, .1583, .1673, .1675, .1856, .1688, .1512)
  // rs <- cor(groupa, groupb, method="spearman")
  // format(rs, digits=18)  #-0.160439560439560425
  gsl_vector_view vgroupa = gsl_vector_view_array_with_stride (groupa,
							       stridea,
							       na);
  gsl_vector_view vgroupb = gsl_vector_view_array_with_stride (groupb,
							       strideb,
							       nb);
  LF_gsl_stats_spearman_workspace * w;
  w = LF_gsl_stats_spearman_alloc (nb);
  double rs = LF_gsl_stats_spearman (&vgroupa.vector, &vgroupb.vector, w);
  LF_gsl_stats_spearman_free (w);
  double expected_rs = -0.160439560439560425;
  if(rs != expected_rs)
    gsl_test_rel (rs, expected_rs, 0.000001, //GSL_DBL_EPSILON,
		  "error in Spearman coef without ties: %.18f (exp) != %.18f (obs)\n",
		  expected_rs, rs);

  // 2nd test, with ties this time
  // groupb2 <- groupb
  // groupb2[8] <- groupb2[4]; groupb2[11] <- groupb2[4]
  double * groupb2 = (double *) malloc (strideb * nb * sizeof(double));
  for (i = 0 ; i < nb ; i++)
    groupb2[i * strideb] = (double) rawb[i] ;
  groupb2[7] = groupb2[3];
  groupb2[10] = groupb2[3];
  gsl_vector_view vgroupb2 = gsl_vector_view_array_with_stride (groupb2,
								strideb,
								nb);
  w = LF_gsl_stats_spearman_alloc (nb);
  double rs2 = LF_gsl_stats_spearman (&vgroupa.vector, &vgroupb2.vector, w);
  LF_gsl_stats_spearman_free (w);
  double expected_rs2 = -0.0905086083851043971;
  if(rs2 != expected_rs2)
    gsl_test_rel (rs2, expected_rs2, 0.000001, //GSL_DBL_EPSILON,
		  "error in Spearman coef with ties: %.18f (exp) != %.18f (obs)\n",
		  expected_rs2, rs2);

  free (groupa);
  free (groupb);
  free (groupb2);
}

int
main (int argc, char ** argv)
{
  gsl_ieee_env_setup();

  argc = 0;                     /* prevent warnings about unused parameters */
  argv = 0;

  test_gsl_stats_spearman ();

  exit(gsl_test_summary());
}
}