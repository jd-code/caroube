/*
 * $Id: voncendian.h,v 1.1 2003/09/24 22:53:44 jd Exp $
 * GenRythm Copyright (C) 2002,2003 Cristelle Barillon & Jean-Daniel Pauget
 * making genetics and groovy sound/noise/rythms
 *
 * genrythm@disjunkt.com  -  http://genrythm.disjunkt.com/
 * 
 * Groovit Copyright (C) 1998 Jean-Daniel PAUGET
 * making accurate and groovy sound/noise
 * jd@disjunkt.fr  -  http://groovit.disjunkt.com/
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
 *
 */

#ifndef VONCENDIAN_H_INC
#define VONCENDIAN_H_INC

#include "SDL.h"

// JDJD endian.h is replaced by autoconf Macro AC_C_BIGENDIAN 
// #include <endian.h>
#ifdef WORDS_BIGENDIAN
#define VONCBYTE_ORDER BIG_ENDIAN
#else
#define VONCBYTE_ORDER LITTLE_ENDIAN
#endif // WORDS_BIGENDIAN
// JDJD end of part related to Autoconf's Macro AC_C_BIGENDIAN



#if (VONCBYTE_ORDER != LITTLE_ENDIAN)
#if (VONCBYTE_ORDER != BIG_ENDIAN)
#error your endianess seems not to be known from me
#endif
#endif

#ifndef VONCBYTE_ORDER
#error endianness unknown
#endif
// JDJDJDJD at removing inclusion of endian.h : I don't know or remember what
// that check was for in groovit code ???
//
// #ifndef LITTLE_ENDIAN
// #error endianness unknown
// #endif


#if (VONCBYTE_ORDER == LITTLE_ENDIAN)
static inline Sint32 chunkconv (Sint32 chunk)		/* read chunk mit endianness */
{	return chunk;
}
#else
static inline Sint32 chunkconv (Sint32 chunk)
{	return (
		((l & 0xFF000000) >> 24)+
		((l & 0x00FF0000) >> 8)+
		((l & 0x0000FF00) << 8)+
		((l & 0x000000FF) << 24)
	        );
}
#endif

#if (VONCBYTE_ORDER == LITTLE_ENDIAN)
static inline Sint32 VONClong (Sint32 l)
{	return l;
}
#else
static inline Sint32 VONClong (Sint32 l)			/* converts 4BYTES read in LITTLEendian */
{	return (
		((l & 0xFF000000) >> 24)+
		((l & 0x00FF0000) >> 8)+
		((l & 0x0000FF00) << 8)+
		((l & 0x000000FF) << 24)
	        );
}
#endif

#if (VONCBYTE_ORDER == LITTLE_ENDIAN)
static inline int VONCint (int i)
{	return i;
}
#else
static inline int VONCint (int i)			/* converts 2BYTES read in LITTLEendian */
{
	return (
		((i & 0xFF00) >> 8)+
		((i & 0x00FF) << 8)
		);
}
#endif

// JDJDJDJD ----------- added after removal of endianess code in readsample

#if (BYTE_ORDER == LITTLE_ENDIAN)
inline Sint32 CONVlong (Sint32 l)  /* converts 4BYTES read in BIGendian */
{
    return (
	       ((l & 0xFF000000) >> 24) +
	       ((l & 0x00FF0000) >> 8) +
	       ((l & 0x0000FF00) << 8) +
	       ((l & 0x000000FF) << 24)
	);
}
#else
inline Sint32 CONVlong (Sint32 l)
{
    return l;
}
#endif

#if (BYTE_ORDER == LITTLE_ENDIAN)
inline int CONVint (int i)     /* converts 2BYTES read in BIGendian */
{
    return (
	       ((i & 0xFF00) >> 8) +
	       ((i & 0x00FF) << 8)
	);
}
#else
inline int CONVint (int i)
{
    return i;
}
#endif


#endif // VONCENDIAN_H_INC

/*
 * $Log: voncendian.h,v $
 * Revision 1.1  2003/09/24 22:53:44  jd
 * Initial revision
 *
 * Revision 1.1.1.1  2003/09/11 23:52:11  jd
 * first importation
 *
 * Revision 1.1.1.1  2003/06/04 13:56:51  jd
 * caroube librarised
 *
 * Revision 1.3  2003/05/11 21:09:07  jd
 * merge with grapefruit lib
 *
 * Revision 1.2  2003/02/16 00:23:04  jd
 * corrected endianess regarding mingw/win32
 *
 * Revision 1.1  2003/01/18 12:17:55  jd
 * merged genetics + SonGe files
 * first release with sounds
 * cleanup for cvs
 *
 * Revision 1.1  2002/12/13 13:58:51  jd
 * added readsample derivated from groovit's code
 * imported new module audo.cpp/h from bouncingball's code
 *
 * Revision 1.2.0.24.0.2  1999/10/19 19:29:45  jd
 * *** empty log message ***
 *
 * Revision 1.2.0.24.0.1  1999/10/11 18:36:29  jd
 * *** empty log message ***
 *
 * Revision 1.2.0.24  1999/10/11 15:30:51  jd
 * second pre-tel-aviv revision
 *
 * Revision 1.2.0.23.0.1  1999/09/15 10:35:48  jd
 * *** empty log message ***
 *
 * Revision 1.2.0.23  1999/09/15 10:20:39  jd
 * second pre tel-aviv public revision, for testing
 *
 * Revision 1.2.0.21.1.2  1998/12/11 13:47:15  jd
 * Initial revision
 *
 */
