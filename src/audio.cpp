/* 
 * $Id: audio.cpp,v 1.3 2004/04/30 14:22:02 jd Exp $
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

/*
 * $Log: audio.cpp,v $
 * Revision 1.3  2004/04/30 14:22:02  jd
 * *** empty log message ***
 *
 * Revision 1.2  2003/12/09 22:57:50  jd
 * added tuned buffer-size in initaudio
 *
 * Revision 1.1.1.1  2003/09/24 22:53:44  jd
 * librarized it
 *
 * Revision 1.1.1.1  2003/09/11 23:52:11  jd
 * first importation
 *
 * Revision 1.1.1.1  2003/06/04 13:56:51  jd
 * caroube librarised
 *
 * Revision 1.5  2003/05/20 12:40:03  jd
 * springtime cleanup
 *
 * Revision 1.4  2003/02/27 23:38:25  jd
 * nested all audio into namespace "caroube"
 *
 * Revision 1.3  2003/02/25 16:15:00  jd
 * extended name-use in samplesonge inheritants
 *
 * Revision 1.2  2003/02/21 00:09:39  jd
 * corrected stream-in binary bug on win32
 * added --enable-build-for-win32 switch to configure
 *
 * Revision 1.1  2003/01/18 12:17:55  jd
 * merged genetics + SonGe files
 * first release with sounds
 * cleanup for cvs
 *
 * Revision 1.2  2002/12/17 00:46:49  jd
 * removed GL dependency
 *
 * Revision 1.1  2002/12/13 13:58:51  jd
 * added readsample derivated from groovit's code
 * imported new module audo.cpp/h from bouncingball's code
 *
 * Revision 1.3  2002/12/05 00:47:39  jd
 * added Vector3 and its florilege of operators
 *
 * Revision 1.2  2002/12/04 00:30:06  jd
 * first rougth spatialisation added (not related with observation angle or distance)
 * strength of ball-ball collisions reported with sound volume.
 *
 * Revision 1.1  2002/12/03 21:29:13  jd
 * added sound support (no spatialisation)
 *
 * Revision 1.5  2002/12/02 23:40:57  cristelle
 * added logarithmic decrease type of enveloppe segments
 *
 * Revision 1.4  2002/11/29 09:56:58  jd
 * small keys additions
 *
 * Revision 1.1.1.1  2002/11/28 12:39:44  cristelle
 * first draft of an oscilloscope in SDL
 *
 * Revision 1.6  2002/11/25 23:07:21  cristelle
 * added ability to delete a Songe from the list
 *
 * Revision 1.5  2002/11/23 23:43:32  cristelle
 * played with multiple inheritance
 *
 */
