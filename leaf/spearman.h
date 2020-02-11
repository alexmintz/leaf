/* spearman/spearman.h
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
extern "C" {
LEAF_Export void
LF_spearman_resolve_ties (const size_t nties,
		       gsl_vector *ranks,
		       size_t *ties_trace);

LEAF_Export int
LF_spearman_rank (const gsl_vector *data,
	       gsl_vector *ranks,
	       gsl_vector *d,
	       gsl_permutation *p);
}