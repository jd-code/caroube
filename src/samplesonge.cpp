/* 
 * $Id$
 * GenRythm Copyright (C) 2002,2003 Cristelle Barillon & Jean-Daniel Pauget
 * making genetics and groovy sound/noise/rythms
 *
 * genrythm@disjunkt.com  -  http://genrythm.disjunkt.com/
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

#include "fft.h"

#include "caroube.h"

namespace caroube
{
//     void SampleSonGeFFT::lomehi (void)
//     {
// 	lo = 0.0, me = 0.0, hi = 0.0;
// 
// 	if (Sample::data == NULL) {
// cerr << "lomehi for " << Sample::name << "    [undefined]" << endl ;
// 	    return ;
// 	}
// 
// 	double re[2048], im[2048];
// 	Sint16 *cur, *end;
// 	int inc;
// 	int i;
// 	
// 	cur = Sample::data;
// 	end = Sample::data + Sample::size ;
// 	inc = Sample::nbch ;
// 	
// 	while (cur < end) {
// 	    for (i=0 ; i<2048 ; i++) {
// 		im[i] = 0.0;
// 		if (cur < end) {
// 		    re[i] = *cur / 32768.0;
// 		    cur += inc;			// JDJDJDJD only the first (left ?) channel is studied ?
// 		} else
// 		    re[i] = 0.0;
// 	    }
// 	    fft (re, im, 11, 0);
// 	    for (i=0 ; i<1024 ; i++)
// 		re[i] = sqrt (re[i] * re[i] + im[i] * im[i]);
// 
// 	    for (i=0 ; i<8 ; i++)   // because 7 ~= 150Hz * (2048 / 44100Hz)
// 		lo += re[i];
// 
// 	    for (i=8 ; i<93 ; i++)  // because 92 ~= 2000Hz * (2048 / 44100Hz)
// 		me += re[i];
// 
// 	    for (i=93 ; i<512 ; i++)
// 		hi += re[i];
// 	}
// cerr << "lomehi for " << Sample::name << "    [" << lo << " , " << me << " , " << hi << "]" << endl ;
//     }

};

