/* statistics/gsl_spearman.h
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

#ifndef __GSL_SPEARMAN_H__
#define __GSL_SPEARMAN_H__

#include <gsl/gsl_vector.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_sort_vector.h>
#include "LEAF_Export.h"
extern "C" {
struct LEAF_Export LF_gsl_stats_spearman_workspace
{
  size_t n;           /* number of observations in each input vector */
  
  gsl_vector *ranks1; /* ranks of first vector */
  gsl_vector *ranks2; /* ranks of first vector */
  gsl_vector *d;      /* sort vector */
  gsl_permutation *p; /* sort vector index */
};

LEAF_Export LF_gsl_stats_spearman_workspace* LF_gsl_stats_spearman_alloc (const size_t n);
LEAF_Export void LF_gsl_stats_spearman_free (LF_gsl_stats_spearman_workspace *w);
LEAF_Export double LF_gsl_stats_spearman (const gsl_vector *data1, const gsl_vector *data2, LF_gsl_stats_spearman_workspace *w);
}


#endif /* __GSL_SPEARMAN_H__ */
