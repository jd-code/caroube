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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CAROUBE_H_GLOBINST
#include "caroube.h" 



namespace caroube {
using namespace std ;

void draw_oscill (Sint16 *p, int nb);

// ---------------------------------------------------------------------------------

void initSr (void) {
    int i;
    for ( i=0 ; i<LSREF ; i++ ) 
	sr [i] = (Sint16)((SINT16_MAX-1) * sin ( i * 2 * (M_PI / LSREF))); 
// JDJDJD for fun : alternative to the regular reference sinusoid :
// square signal...
//	sr [i] = (i<(LSREF>>1)) ? (INT16_MAX-1) : - (INT16_MAX-1);
// ...and rich sinusoid
//	sr [i] = (Sint16)((INT16_MAX-1) * sin ( i * 2 * (M_PI / LSREF))  * sin ( i * 2 * (M_PI / LSREF)) * sin ( i * 2 * (M_PI / LSREF))); 
}

// ---------------------------------------------------------------------------------

void initDphi (void) {
    double qt = pow ( 2, 1.0/24);
    int i;
    for ( i=0 ; i<NBDPHI ; i++)
	dphi [i] = (Sint32)((pow (qt, i-119) * 440.0 * LSREF * DPHIMULT) / 44100.0);
}

// ---------------------------------------------------------------------------------

Oscillator::Oscillator (int freq = 0)  : SonGe () {
    dphi = (Sint32)(((double)freq * LSREF * DPHIMULT) / 44100.0);
}

// ---------------------------------------------------------------------------------


ostream& operator<< (ostream & out, Envelope & e) {
    Envelope::iterator i;

    for (i=e.begin() ; i!=e.end() ; i++) {
	out << "t_amp : " << i->tamp << " tta : " << i->tta ;
	switch (i->type) {
	    case ENVEL_LIN:
		out << " linear" << endl;
		break;
	    case ENVEL_LOG:
		out << " logarithmic" << endl;
		break;
	    default:
		out << " UNKNOWN TYPE ?" << endl;
		break;
	}
    }
    return out;
}
    

// ---------------------------------------------------------------------------------


void fill_audio( void *udata, Uint8 *stream, int len) {
    int i;
    int nb = len >> 2; // =/4 

    Sint16 *p = (Sint16 *) stream;

    PlayedPSonGe::iterator ilo;
    Sint16 sample;

    for (i=0 ; i<nb ; i++) {
	if ((i & BUCKETMASK) == 0) {
	    for (ilo=lo.begin() ; ilo!=lo.end() ; ) {
		isonge = ilo;
		ilo++;				// we increment before calling bucketComp, because...
		isonge->first->bucketComp() ;   // ...bucketComp may supress the SonGe currently pointed by isonge 
	    }
	}
      
        Sint32 Cl = 0,
               Cr = 0;
	for (ilo=lo.begin() ; ilo!=lo.end() ; ilo++) {
	    sample = ilo->first->getSample();
	    Cl += (sample * ilo->first->vl) >> 8;
	    Cr += (sample * ilo->first->vr) >> 8;
	}
	// ---- clipping tests
	if (Cl > SINT16_MAX) Cl = SINT16_MAX; else if (Cl < SINT16_MIN) Cl = SINT16_MIN;
	if (Cr > SINT16_MAX) Cr = SINT16_MAX; else if (Cr < SINT16_MIN) Cr = SINT16_MIN;

	*p++ = Cl;
	*p++ = Cr;
    } 
}



// ---------------------------------------------------------------------------------


void PlayedPSonGe::push (SonGe *p) {
    iterator i = find (p);
    if (i != end()) {
	i->first->rewind ();
    }
    else {
	MapPSonGe::operator[] (p);
    }
}


// ---------------------------------------------------------------------------------


bool initaudio(size_t nbsamples) {
    SDL_AudioSpec wanted, obtained;

    /* Set the audio format */
    wanted.freq = 44100;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 2;    /* 1 = mono, 2 = stereo */
    // wanted.samples = 1024;  /* Good low-latency value for callback */
    wanted.samples = nbsamples;
    wanted.callback = fill_audio;
    wanted.userdata = NULL;

    /* Opens the audio device, forcing the desired format */
    if ( SDL_OpenAudio (&wanted, &obtained) < 0 ) {	
	//JDJDJDJD is this a trick with SDL-1.2.7 ?
	//cerr << "could not open audio: " << SDL_GetError() << endl; 
	cerr << "could not open audio: " << endl ;
	return false;
    }
    
    if (wanted.freq != obtained.freq) 
	cerr << "obtained freq : "<< obtained.freq << "instead of : " << wanted.freq << endl;
    if (wanted.format != obtained.format) 
	cerr << "obtained format : "<< obtained.format << "instead of : " << wanted.format << endl;
    if (wanted.channels != obtained.channels) 
	cerr << "obtained channels : "<< obtained.channels << "instead of : " << wanted.channels << endl;
    if (wanted.samples != obtained.samples) 
	cerr << "obtained samples : "<< obtained.samples << "instead of : " << wanted.samples << endl;
    
    // this initialisation should probably move somewhere-else JDJDJDJD
    initSr ();
    initDphi ();

    return true;
}

};

