## 
## $Id: Makefile.am,v 1.6 2004/05/03 23:19:16 jd Exp $
## GenRythm Copyright (C) 2002,2003 Cristelle Barillon & Jean-Daniel Pauget
## making genetics and groovy sound/noise/rythms
##
## loopsoids@disjunkt.com  -  http://loopsoids.disjunkt.com/
## 
## GenRythm Copyright (C) 2002 Cristelle Barillon, Jean-Daniel PAUGET
## A whole set of bouncing balls
## loopsoids@disjunkt.com  -  http://loopsoids.disjunkt.com/
## 
## This file is part of GenRythm.
## 
## GenRythm is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
## 
## GenRythm is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with GenRythm; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
## 
## you can also try the web at http://www.gnu.org/
## 

lib_LTLIBRARIES =libcaroube.la

libcaroube_la_LDFLAGS = -version-info @lt_major@:@lt_revision@:@lt_age@ @GL_LIBS@ @WIN32LDFLAF@
##			-no-undefined \
##			-Xlinker --export-all-symbols \

libcaroube_la_SOURCES = samplesonge.cpp audio.cpp conv_ieee_ext.c \
			readsample.cpp asequencer.cpp \
			fft.c \
			testdll.c



## loopsoids_LDADD = @SDL_LIBS@ @GL_LIBS@ @GRAPEFRUIT_LIBS@
## loopsoids_LDADD = @GL_LIBS@

pkginclude_HEADERS =	include/caroube/asequencer.h \
			include/caroube/audio.h \
			include/caroube/caroube.h \
			include/caroube/conv_ieee_ext.h \
			include/caroube/readsample.h \
			include/caroube/samplesonge.h \
			include/caroube/voncendian.h \
			include/caroube/fft.h

## 
##  Revision 1.1.1.1  2003/06/04 13:56:51  jd
##  caroube librarised
##
