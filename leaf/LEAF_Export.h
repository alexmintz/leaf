/*
*
*	© Copyright 2011 by TAF Co. All rights reserved.
*
*/

#ifndef _LEAF_EXPORT_H_
#define _LEAF_EXPORT_H_

#include "ace/config-lite.h"

#if defined (LEAF_AS_STATIC_LIBS)
#  if !defined (LEAF_HAS_DLL)
#    define LEAF_HAS_DLL 0
#  endif /* ! LEAF_HAS_DLL */
#else
#  if !defined (LEAF_HAS_DLL)
#    define LEAF_HAS_DLL 1
#  endif /* ! LEAF_HAS_DLL */
#endif

#if defined (LEAF_HAS_DLL) && (LEAF_HAS_DLL == 1)
#  if defined (LEAF_BUILD_DLL)
#    define LEAF_Export ACE_Proper_Export_Flag
#    define LEAF_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define LEAF_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* LEAF_BUILD_DLL */
#    define LEAF_Export ACE_Proper_Import_Flag
#    define LEAF_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define LEAF_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* LEAF_BUILD_DLL */
#else /* LEAF_HAS_DLL == 1 */
#  define LEAF_Export
#  define LEAF_SINGLETON_DECLARATION(T)
#  define LEAF_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* LEAF_HAS_DLL == 1 */

#endif /*_LEAF_EXPORT_H_*/

