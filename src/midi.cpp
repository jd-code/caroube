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

#include "config.h"
#ifdef CAROUBCANJACK
#include <jack/jack.h> 
#include <jack/midiport.h>
#include <errno.h>  // ENODATA
#endif

#include "midi.h"

namespace caroube {

using namespace std ;

    void ActivMidiListener::push (MidiCanalListener *p) {
	iterator i = find (p);
	if (i != end()) {
	}
	else {
	    map<MidiCanalListener *, int>::operator[] (p);
	}
    }

}
