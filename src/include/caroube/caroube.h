/* 
 * $Id$
 * Caroube Copyright (C) 2002,2003 Cristelle Barillon & Jean-Daniel Pauget
 * A whole set of sound facilities: generations, load, save, sequences
 *
 * caroube@disjunkt.com  -  http://caroube.disjunkt.com/
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * you can also try the web at http://www.gnu.org/
 *
 */

#ifndef CAROUBE_H_INC
#define CAROUBE_H_INC

#include <iostream>
#include <list>
#include <map>
#include <string>

#include "SDL.h"

#ifdef _WIN32
#   ifdef DLL_EXPORT
#	ifdef CAROUBE_H_GLOBINST
#	    warning dll_export
#	    define CAROUBE_H_SCOPE __declspec(dllexport)
#	else
#	    define CAROUBE_H_SCOPE extern
#	endif
#   else
#	ifdef LIBCAROUBE_DLL_IMPORT
#	    warning dll_import
#	    define CAROUBE_H_SCOPE extern __declspec(dllimport)
#	else
// JDJDJDJD still some study needed here :
//     objects seem to need being defined once only via CAROUBE_H_GLOBINST selection rather than behaving
//     exclusively via CAROUBE_H_SCOPE ???
#	    ifdef CAROUBE_H_GLOBINST
#		define CAROUBE_H_SCOPE
#	    else
#		define CAROUBE_H_SCOPE extern
#	    endif
#	endif
#   endif
#endif
#ifndef CAROUBE_H_SCOPE
#   ifdef CAROUBE_H_GLOBINST
#	define CAROUBE_H_SCOPE
#   else
#	define CAROUBE_H_SCOPE extern
#   endif
#endif


#include "audio.h"
#include "asequencer.h"
#include "readsample.h"
#include "samplesonge.h"

#ifndef bzouzerr
#define bzouzerr cerr << __FILE__  << ':'  << __LINE__ << ':' << __PRETTY_FUNCTION__ << ':'
#endif

#endif // CAROUBE_H_INC

/*
 * $Log: caroube.h,v $
 * Revision 1.3  2004/04/30 14:22:02  jd
 * *** empty log message ***
 *
 * Revision 1.2  2003/10/29 23:37:34  jd
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2003/09/24 22:53:44  jd
 * librarized it
 *
 *
 */
 
