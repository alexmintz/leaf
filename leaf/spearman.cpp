/* spearman/spearman.c
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

//#include <config.h>
#include <gsl/gsl_errno.h>
#include "gsl_stats_spearman.h"

/*
 * This module contains routines to calculate the Spearman
 * rank correlation coefficient.
 *
 * The internal routines contain code adapted from
 *
 * [1] "The Apache Commons Mathematics Library"
 *     http://commons.apache.org/math/
 *
 */

#include "spearman.h"

/*
  LF_gsl_stats_spearman_alloc()
  Allocate space for a spearman workspace. The size of the
workspace is O(4n + n).

Inputs: n - number of observations in each input vector

Return: pointer to workspace
*/
LF_gsl_stats_spearman_workspace *
LF_gsl_stats_spearman_alloc (const size_t n)
{
  if (n == 0)
  {
    GSL_ERROR_NULL ("n must be at least 1", GSL_EINVAL);
  }
  else
  {
    LF_gsl_stats_spearman_workspace *w;
    
    w = (LF_gsl_stats_spearman_workspace *) malloc
      (sizeof (LF_gsl_stats_spearman_workspace));
    
    if (w == 0)
    {
      GSL_ERROR_NULL ("failed to allocate space for workspace",
		      GSL_ENOMEM);
    }
    
    w->n = n;
    
    w->ranks1 = gsl_vector_alloc (n);
    if (w->ranks1 == 0)
    {
      free (w);
      GSL_ERROR_NULL ("failed to allocate space for ranks1 vector",
		      GSL_ENOMEM);
    }
      
    w->ranks2 = gsl_vector_alloc (n);
    if (w->ranks2 == 0)
    {
      gsl_vector_free (w->ranks1);
      free (w);
      GSL_ERROR_NULL ("failed to allocate space for ranks2 vector",
		      GSL_ENOMEM);
    }
    
    w->d = gsl_vector_alloc (n);
    if (w->d == 0)
    {
      gsl_vector_free (w->ranks2);
      gsl_vector_free (w->ranks1);
      free (w);
      GSL_ERROR_NULL ("failed to allocate space for d vector",
		      GSL_ENOMEM);
    }
      
    w->p = gsl_permutation_alloc (n);
    if (w->p == 0)
    {
      gsl_vector_free (w->d);
      gsl_vector_free (w->ranks2);
      gsl_vector_free (w->ranks1);
      free (w);
      GSL_ERROR_NULL ("failed to allocate space for p permutation",
		      GSL_ENOMEM);
    }
    
    return w;
  }
} /* LF_gsl_stats_spearman_alloc() */

/*
LF_gsl_stats_spearman_free()
  Free a gsl_spearman_workspace.

Inputs: w - workspace to free

Return: none
 */
void
LF_gsl_stats_spearman_free (LF_gsl_stats_spearman_workspace *w)
{
//  RETURN_IF_NULL (w);
  gsl_permutation_free (w->p);
  gsl_vector_free (w->d);
  gsl_vector_free (w->ranks2);
  gsl_vector_free (w->ranks1);
  free (w);
} /* LF_gsl_stats_spearman_free() */

/*
LF_gsl_stats_spearman()
  Compute the Spearman rank correlation coefficient.

Inputs: data1 - first vector of observations
        data2 - second vector of observations

Return: Spearman coefficient
*/
double
LF_gsl_stats_spearman (const gsl_vector *data1,
                    const gsl_vector *data2,
		    LF_gsl_stats_spearman_workspace *w)
{
  if (data1->size != w->n)
  {
    GSL_ERROR ("data1 vector has wrong size", GSL_EBADLEN);
  }
  else if (data2->size != w->n)
  {
    GSL_ERROR ("data2 vector has wrong size", GSL_EBADLEN);
  }
  else
  {
    double rs = 0.0;
    
    LF_spearman_rank (data1, w->ranks1, w->d, w->p);
    LF_spearman_rank (data2, w->ranks2, w->d, w->p);
    
    rs = gsl_stats_correlation (w->ranks1->data, w->ranks1->stride,
				w->ranks2->data, w->ranks2->stride,
				w->n);
    
    return rs;
  }
} /* LF_gsl_stats_spearman() */

/****************************************
 *          INTERNAL ROUTINES           *
 ****************************************/

/*
LF_spearman_resolve_ties()
  Resolve a sequence of ties.

The input ranks array is expected to take the same value for all indices in
tiesTrace. The common value is recoded with the average of the indices. For
example, if ranks = <5,8,2,6,2,7,1,2> and tiesTrace = <2,4,7>, the result
will be <5,8,3,6,3,7,1,3>.

Input: nties - number of ties
       ranks - vector of ranks
       ties_trace - vector with the indices of the ties

Return: none

Note: this routine is adapted from The Apache Commons Mathematics Library
*/
void
LF_spearman_resolve_ties (const size_t nties, gsl_vector *ranks, size_t *ties_trace)
{
  size_t i;
  
  // constant value of ranks over ties_trace
  double c = gsl_vector_get (ranks, ties_trace[0]);
  
  // new rank (ie. the average of the current indices)
  double avg = (2*c + nties - 1) / 2;
  
  for(i=0; i<nties; ++i)
    gsl_vector_set (ranks, ties_trace[i], avg);
} /* LF_spearman_resolve_ties() */

/*
LF_spearman_rank()
 Rank data using the natural ordering on doubles, ties being resolved by 
taking their average.

Input: data - vector of observations
       ranks - vector of ranks
       d - vector of sorted observations
       p - vector of indices of observations as if they were sorted

Return: none

Note: this routine is adapted from The Apache Commons Mathematics Library
*/
int
LF_spearman_rank (const gsl_vector *data,
	       gsl_vector *ranks,
	       gsl_vector *d,
	       gsl_permutation *p)
{
  size_t i;
  
  // copy the input data and sort them
  gsl_vector_memcpy (d, data);
  gsl_sort_vector (d);
  
  // get the index of the input data as if they were sorted
  gsl_sort_vector_index (p, data);
  
  // walk the sorted array, filling output array using sorted positions,
  // resolving ties as we go
  double pos = 1;
  gsl_vector_set (ranks, gsl_permutation_get (p, 0), pos);
  size_t nties = 1;
  
  size_t * ties_trace = (size_t*) calloc (1, sizeof(size_t));
  if (ties_trace == 0)
  {
    GSL_ERROR_NULL ("failed to allocate space for ties_trace vector",
		    GSL_ENOMEM);
  }
  ties_trace[0] = gsl_permutation_get (p, 0);
  
  for (i=1; i<data->size; ++i)
  {
    if (gsl_vector_get (d, i) - gsl_vector_get (d, i-1) > 0)
    {
		pos = (double)(i + 1);
      if (nties > 1)
        LF_spearman_resolve_ties (nties, ranks, ties_trace);
      ties_trace = (size_t*) realloc (ties_trace, sizeof(size_t));
      if (ties_trace == 0)
      {
	GSL_ERROR_NULL ("failed to allocate space for ties_trace vector",
			GSL_ENOMEM);
      }
      nties = 1;
      ties_trace[0] = gsl_permutation_get (p, i);
    }
    else
    {
      ++nties;
      ties_trace = (size_t*) realloc (ties_trace, nties * sizeof(size_t));
      if (ties_trace == 0)
      {
	GSL_ERROR_NULL ("failed to allocate space for ties_trace vector",
			GSL_ENOMEM);
      }
      ties_trace[nties-1] = gsl_permutation_get (p, i);
    }
    gsl_vector_set (ranks, gsl_permutation_get (p, i), pos);
  }
  if(nties > 1)
    LF_spearman_resolve_ties (nties, ranks, ties_trace);
  
  free (ties_trace);
  
  return GSL_SUCCESS;
} /* LF_spearman_rank() */
