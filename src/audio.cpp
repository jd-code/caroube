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

#include "config.h"
#ifdef CAROUBCANJACK
#include <jack/jack.h> 
#include <jack/midiport.h>
#include <errno.h>  // ENODATA
#endif

#define CAROUBE_H_GLOBINST
#include "caroube.h" 
#include "midi.h"

namespace caroube {
using namespace std ;

void draw_oscill (Sint16 *p, int nb);

const char * get_caroube_version (void)
{
#ifdef BZOUTOTALOCAL
    return ("Total-Local® " PACKAGE "-" VERSION " $Id$");
#else
    return (PACKAGE "-" VERSION " $Id$");
#endif
}

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

Oscillator::Oscillator (int freq /* = 0 */)  : SonGe () {
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
    

// ---------- jack stuff -----------------------------------------------------------
#ifdef CAROUBCANJACK

bool usejack = false;	    // are we currently using jack ?
bool leftjack = false;	    // have we been laid off from some jack server ?

jack_port_t *output_portl = NULL,  // jack outlets
	    *output_portr = NULL,
            *input_portl = NULL,   // jack inlets
            *input_portr = NULL,
	    *midi_input = NULL;	    // jack midi inlet

jack_client_t *jclient;	    // jack-client handle towrad the jack-server


void jack_shutdown (void *arg) {
    jclient = NULL;
    leftjack = true;
}
#endif

// ---------------------------------------------------------------------------------

    // this code is duplicated within the jack call-back routine and the SDL call-back routine


#ifdef CAROUBCANJACK
int jackcallback (jack_nframes_t nb, void *jack_arg) {
    unsigned int i;

    jack_default_audio_sample_t *jackoutr =
        (jack_default_audio_sample_t *) jack_port_get_buffer (output_portr, nb);
    jack_default_audio_sample_t *jackoutl =
        (jack_default_audio_sample_t *) jack_port_get_buffer (output_portl, nb);

    jack_default_audio_sample_t *jackinr = (input_portr == NULL) ? NULL :
        (jack_default_audio_sample_t *) jack_port_get_buffer (input_portr, nb);
    jack_default_audio_sample_t *jackinl = (input_portl == NULL) ? NULL :
        (jack_default_audio_sample_t *) jack_port_get_buffer (input_portl, nb);

    if (midi_input != NULL) {
	jack_midi_event_t event;
	void* midiin_buf = jack_port_get_buffer(midi_input, nb);
	int i;
	int n = jack_midi_get_event_count(midiin_buf);
	for (i=0 ; i<n ; i++) {
	    if (jack_midi_event_get (&event, midiin_buf, i) != ENODATA) {
		if (event.size > 0) {
		    int channel = ((unsigned char *)event.buffer)[0] & 0x0f;
		    ActivMidiListener::iterator mi;
		    for (mi=midil[channel].begin() ; mi!=midil[channel].end() ; mi++) {
			mi->first->signalback (event);
		    }
		}
	    }
	}
    }

    PlayedPSonGe::iterator ilo;
    CapturingSonCa::iterator ilc;
    Sint16 sample;

//	wondering if the nb param would gitter ...
//	static int dumpit = 0;
//	dumpit ++;
//	if (dumpit == 1) {
//	    cerr << "jackcallback nb = " << nb << endl;
//	}
//	if (dumpit == 27)
//	    dumpit = 0;


    for (i=0 ; i<nb ; i++) {
	if ((i & BUCKETMASK) == 0) {	    // JDJDJDJD il faudrait avoir un compteur general sinon ca peut se decaler ...
	    for (ilc=lc.begin() ; ilc!=lc.end() ; ) {
		isonca = ilc;
		ilc++;				// we increment before calling bucketComp, because...
		isonca->first->bucketComp() ;   // ...bucketComp may supress the SonGe currently pointed by isonge 
	    }
	    
	    for (ilo=lo.begin() ; ilo!=lo.end() ; ) {
		isonge = ilo;
		ilo++;				// we increment before calling bucketComp, because...
		isonge->first->bucketComp() ;   // ...bucketComp may supress the SonGe currently pointed by isonge 
	    }
	}
      
        Sint32 Cl = 0,
               Cr = 0;

	double  dl, dr;

	if (jackinl == NULL) {
	    dl = 0.0;
	} else {
	    dl = (double)(*jackinl++) * 32768.0;
	    dl = (dl >= 32767.0) ? 32767.0 : dl;
	    dl = (dl <= -32767.0) ? -32767.0 : dl;
	}
	if (jackinr == NULL) {
	    dr = 0.0;
	} else {
	    dr = (double)(*jackinr++) * 32768.0;
	    dr = (dr >= 32767.0) ? 32767.0 : dr;
	    dr = (dr <= -32767.0) ? -32767.0 : dr;
	}
	
	for (ilc=lc.begin() ; ilc!=lc.end() ; ilc++) {
	    ilc->first->getSample((Sint16)dl, (Sint16)dr);
	}
	for (ilo=lo.begin() ; ilo!=lo.end() ; ilo++) {
	    sample = ilo->first->getSample();
	    Cl += (sample * ilo->first->vl) >> 8;
	    Cr += (sample * ilo->first->vr) >> 8;
	}
	// ---- clipping tests
	if (Cl > SINT16_MAX) Cl = SINT16_MAX; else if (Cl < SINT16_MIN) Cl = SINT16_MIN;
	if (Cr > SINT16_MAX) Cr = SINT16_MAX; else if (Cr < SINT16_MIN) Cr = SINT16_MIN;

	*jackoutl ++ = ((jack_default_audio_sample_t) Cl)/32768.0;
	*jackoutr ++ = ((jack_default_audio_sample_t) Cr)/32768.0;
    }

    return 0;
}
#endif

void fill_audio( void *udata, Uint8 *stream, int len) {
    int i;
    int nb = len >> 2; // =/4 

    Sint16 *p = (Sint16 *) stream;

    PlayedPSonGe::iterator ilo;
    Sint16 sample;

    for (i=0 ; i<nb ; i++) {
	if ((i & BUCKETMASK) == 0) {	    // JDJDJDJD il faudrait avoir un compteur general sinon ca peut se decaler ...
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


void CapturingSonCa::push (SonCa *p) {
    iterator i = find (p);
    if (i != end()) {
	i->first->rewind ();	// JDJDJDJD c'est carrement discutable comme concept
    }
    else {
	MapCSonCa::operator[] (p);
    }
}


// ---------------------------------------------------------------------------------

void pauseaudio (int pause_on) {
#ifdef CAROUBCANJACK
    if (usejack) {
	if (pause_on == 0) {
	    if (jack_activate (jclient)) {
		cerr << "cannot activate jack as client" << endl;
		leftjack = true;    // we must leave cleanly later since jackd's not here ...
	    }
	} else {
	    if (jack_deactivate (jclient)) {
		cerr << "cannot deactivate jack as client" << endl;
		leftjack = true;    // we must leave cleanly later since jackd's not here ...
	    }
	}
    } else {
#endif
    SDL_PauseAudio(pause_on);
#ifdef CAROUBCANJACK
    }
#endif
}

bool initinputaudio (void) {
#ifdef CAROUBCANJACK
    if (!usejack) {
	cerr << "audio input is only handled throught jack. set SDL_AUDIODRIVER to jack." << endl;
	return false;
    }
    
    input_portl = jack_port_register (jclient, "inputl", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    input_portr = jack_port_register (jclient, "inputr", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    if ((input_portl == NULL) || (input_portr == NULL)) {
	cerr << "some jack input ports could notr be created ??" << endl;
	return false;
    }
    return true;
#else
    return false;
#endif
}

bool initaudio(string & appname, size_t nbsamples) {
#ifdef CAROUBCANJACK
    char *env_sdl_audiodriver = getenv ("SDL_AUDIODRIVER");
if (env_sdl_audiodriver != NULL)
    if ((env_sdl_audiodriver != NULL) && (strncmp (env_sdl_audiodriver, "jack", 4)) == 0) {
	usejack = true;
    }

    if (usejack) {
	jack_status_t status;
	jclient = jack_client_open (appname.c_str(), JackNoStartServer, &status);
	if (jclient == NULL) {
	    if (status & JackServerFailed) {
		cerr << "could not connect to jackd server, not running ?" << endl;
	    } else {
		cerr << "could not connect to jackd server, status :" << status << endl;
	    }
	    return false;
	}

	if (status & JackNameNotUnique) {
	    appname = jack_get_client_name (jclient);
	}

	jack_set_process_callback (jclient, jackcallback, 0);
	jack_on_shutdown (jclient, jack_shutdown, 0);
	cerr << "engine sample rate: " << jack_get_sample_rate (jclient) << endl;

	output_portl = jack_port_register (jclient, "outputl", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	output_portr = jack_port_register (jclient, "outputr", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

	midi_input = jack_port_register (jclient, "input", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    } else {
#endif
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
    
#ifdef CAROUBCANJACK
    } // if (usejack) ... else ...
#endif
    // this initialisation should probably move somewhere-else JDJDJDJD
    initSr ();
    initDphi ();

    return true;
}

bool initaudio(size_t nbsamples) {
    string appname("caroube-cli");
    return initaudio (appname, nbsamples);
}

bool leaveaudio (void) {
    pauseaudio (0);
#ifdef CAROUBCANJACK
    jack_client_close (jclient);
#endif
    return true;
}

};

