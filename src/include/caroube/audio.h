/*
 *  $Id$
 *
 *  jd - 2002/12/03 from oscil.cpp,v 1.5 2002/12/02 23:40:57 cristelle Exp
 *
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

#ifndef   CAROUBE_H_INC
#warning audio.h should not be included as is. caroube.h should be included instead
#endif // CAROUBE_H_INC

#ifndef BB_AUDIO_H_INC
#define BB_AUDIO_H_INC


//! all the sound facilities: generations, load, save, sequences
/*! The caroube fruit makes sound when shaked.
 *
 *  This namespace carries all utilities for generating sounds (SonGe and inherited),
 *  to sequence them (SonGeSeq),
 *  to load and save them (Sample)...
 */
namespace caroube
{
    using namespace std ;

    //! returns the release-name of this module
    const char * get_caroube_version (void);

    // ---------------------------------------------------------------------------------

#ifndef SINT16_MAX
#define SINT16_MAX 32767
#endif // SINT16_MAX

#ifndef SINT16_MIN
#define SINT16_MIN (-32767-1)
#endif // SINT16_MIN

#ifndef M_PI
# define M_E		2.7182818284590452354	/* e */
# define M_LOG2E	1.4426950408889634074	/* log_2 e */
# define M_LOG10E	0.43429448190325182765	/* log_10 e */
# define M_LN2		0.69314718055994530942	/* log_e 2 */
# define M_LN10		2.30258509299404568402	/* log_e 10 */
# define M_PI		3.14159265358979323846	/* pi */
# define M_PI_2		1.57079632679489661923	/* pi/2 */
# define M_PI_4		0.78539816339744830962	/* pi/4 */
# define M_1_PI		0.31830988618379067154	/* 1/pi */
# define M_2_PI		0.63661977236758134308	/* 2/pi */
# define M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
# define M_SQRT2	1.41421356237309504880	/* sqrt(2) */
#endif //M_PI

    // ---------------------------------------------------------------------------------


#define BUCKETNBSAMPLE 8	/*!< \brief   the number of sample in a bucket (for triggering bucketComp procedure)	*/
#define BUCKETMASK     7	/*!< \brief   the mask for checking buck fillness					*/

#define LSREF 4096		/*!< \brief   the length of the reference sinusoid					*/

#define NBDPHI 256		/*!< \brief   the number of phase incrementators					*/
#define DPHIMULT 1024		/*!< \brief   the fixed decimal multiplicator for dphis					*/
#define DPHISHIFT 10		/*!< \brief   the fixed decimal shifter for dphis (1024 = 2^10)				*/
#define LSREFMASQ 0x3fffff	/*!< \brief   the mask matching for binary modulo LSREF*1024				*/


    CAROUBE_H_SCOPE Sint16 sr [LSREF];   /*!< \brief   the reference sinusoid itself					*/
    CAROUBE_H_SCOPE Sint32 dphi [NBDPHI];/*!< \brief   the phase incrementators themselves				*/

    // ---------------------------------------------------------------------------------


    class SonGe;

    typedef list<SonGe *> ListPSonGe;	    // JDJDJD probably a vector would be better

    typedef map<SonGe *, int> MapPSonGe;

    class PlayedPSonGe : public MapPSonGe
    {
	public:
	    void push (SonGe *p);

    };

    //! the list of currently played SonGe
    CAROUBE_H_SCOPE PlayedPSonGe lo;
    //! the currently computed SonGe (not for public use !)
    CAROUBE_H_SCOPE PlayedPSonGe::iterator isonge;
    

    //! SOuNd GEnerator - the canvas for any sound object
    /*! 
     *  any object that finally produce sounds inherit from SonGe
     *  so that it is called-back every time a new sample is to be produced
     *  and from time to time in order to perform less time-stressed operation
     */
    class SonGe
    {   private:
	    string name;

	protected:
	    //! sets thename of the SonGe
	    void setname (const string & name) { SonGe::name = name; } 
	    void setname (const char * name) { SonGe::name = name; } 

	public:
	    //! the left-channel playing volume min=0, max=256
	    int vl;
	    //! the right-channel playing volume min=0, max=256
	    int vr;

	    inline SonGe (void) { vl = 0, vr = 0; name = "no-name"; }

	    //! the every-sample called-back method
	    /*! this virtual member function is called back every time a sample has to
	     *  be delivered before mixing.
	     *  be careful : this function is called 44 thousand times per seconds,
	     *  it's design should be as light as possible.
	     */
	    virtual inline Sint16 getSample (void) { return 0; }

	    //! the every-bucket-sample called-back method
	    /*! this virtual member function is called back every time BUCKETNBSAMPLE
	     *  were computed.
	     *  It is designed for calculations that are needed only from time to time
	     *  in order to release the stress of getSample
	     */
	    virtual inline Uint16 bucketComp (void) { lo.erase (isonge); return 0; }

	    //! puts the sound generator in the states just before it begins emmiting any sound
	    /*! If such a concept exists for the said object, of course... */
	    virtual inline void rewind (void) { }

	    //! method for retrieving the SonGe's name.
	    const string & getname (void) { return name; }

	    /*! the destructor doesn't remove from the list of played SonGe.
	     *  also doesn't remove from other refernce such as sequencer...
	     */
	    virtual ~SonGe (void) { }
    };


    // ---------------------------------------------------------------------------------

    class Oscillator : public SonGe
    {   public:
	    Oscillator (int freq);
	    inline Sint16 getSample (void) {
		phase += dphi;
		phase &= LSREFMASQ;
		return sr [phase >> DPHISHIFT];
	    }

	private:
	    Sint32 phase;
	    Sint32 dphi;
    };


    // ---------------------------------------------------------------------------------


    typedef enum { ENVEL_LIN, ENVEL_LOG } EnvElType;	// vnature of an enveloppe segment

    typedef struct {
	Uint16 tamp;	    // target amplitude
	Sint32 tta;		    // time length left to reach the target amplitude (in samples-length)
	EnvElType type;  // nature of the segment
    } EnvEl;

    class Envelope : public list<EnvEl>
    {
	public:
	    void addEnvel (Uint16 tamp, Sint32 tta) {
		EnvEl envel;
		envel.type = ENVEL_LIN;
		envel.tamp = tamp;
		envel.tta = tta;
		push_back (envel);
	    }
	    void addLogEnvel (Uint16 ammo) {
		EnvEl envel;
		envel.type = ENVEL_LOG;
		envel.tamp = ammo;
		envel.tta = 1;
		push_back (envel);
	    }
    };

    ostream& operator<< (ostream & out, Envelope & e);


    // ---------------------------------------------------------------------------------


    class EnvGen : public Envelope
    {
	public:
	    bool isfinished;
	    
	    inline EnvGen() : Envelope () {
		ienv = Envelope::begin();
		amp = 0;	// CBCB the initial amplitude could be set as a property of the envelope
			    // or as a parameter of the constructor
		tta = 0;
		tamp = 0;
		isfinished = false;
	    }
	    inline EnvGen(Envelope & env) : Envelope (env) {
		ienv = Envelope::begin();
		amp = 0;	// CBCB the initial amplitude could be set as a property of the envelope
			    // or as a parameter of the constructor
		tta = 0;
		tamp = 0;
		isfinished = false;
	    }
	    inline EnvGen (const EnvGen & eg) {
		// variante : (Envelope &) (*this) = (Envelope)eg;
		*(Envelope *) this = (Envelope)eg;
		
		ienv = Envelope::begin();
		amp = 0;	// CBCB the initial amplitude could be set as a property of the envelope
			    // or as a parameter of the constructor
		tta = 0;
		tamp = 0;
		isfinished = false;
	    }
	    inline EnvGen & operator= (const EnvGen & eg) {
		cerr << "EnvGen & EnvGen::operator=(const EnvGen &) on est appelé !!!" << endl ;
		// JDJDJDJD faire attentention a l'auto-affectation (stroustrup p.272)
		// variante : (Envelope &) (*this) = (Envelope)eg;
		*(Envelope *) this = (Envelope)eg;
		
		ienv = Envelope::begin();
		amp = 0;	// CBCB the initial amplitude could be set as a property of the envelope
			    // or as a parameter of the constructor
		tta = 0;
		tamp = 0;
		isfinished = false;
		return *this;
	    }
	    inline Uint16 bucketComp (void) {
		if (tta <= 0) {
		    if (ienv == Envelope::end())
		    {   
			isfinished = true;

			// this below was before we had the isfinished concept
			// but there might have some good ideas to fetch here JDJDJDJD
			//
			//	// JDJDJDJD here we arbitrarily return 0.
			//	// but we should better keep returning the last target amp requested
			//	// (that can eventually be 0)
			type = ENVEL_LIN;
			damp = 0;
			amp = tamp;	// JDJDJD here there may be a "pop!" (we jump to arbitrary value)
			tta = 100000; // not very good indeed JDJDJDJD
		    } else {
			switch (ienv->type) {
			    case ENVEL_LIN:
				type = ENVEL_LIN;
				tamp = ienv->tamp << 15;
				tta = ienv->tta;
    if (tta != 0)
				damp = (tamp - amp) / tta;
				ienv++;
				break;
			    case ENVEL_LOG:
				type = ENVEL_LOG;
				amp >>= 15;
				tamp = ienv->tamp;
				tta = ienv->tta;
				ienv++;
				break;
			}
		    }
		}
		switch (type) {
		    default:
		    case ENVEL_LIN:
			tta--;
			amp += damp;
			return (Uint16)(amp >> 15) ;    // JDJDJDJD missing here : a clipping test to validate
							// that the value belongs to [0:32768]
		    case ENVEL_LOG:
			amp *= tamp;    // in that case both amp and damp belongs to [0:32768]
			amp >>= 15;
			if (amp == 0)   // the log-segment is finished when 0 is reached
			    tta = 0;
			return amp;
		}
	    }
	    void rewind (void) {
		ienv = Envelope::begin();
    //	    amp = 0;	// CBCB the initial amplitude could be set as a property of the envelope
    //			// or as a parameter of the constructor
		tta = 0;
		tamp = 0;
		isfinished = false;
	    }

	private:
	    EnvElType type;		    // current type of enveloppe element
	    Sint32 damp;		    // delta amplitude
	    Sint32 amp;		    // current (shifted) amplitude
	    Sint32 tamp;		    // target (shifted) amplitude
	    Sint32 tta;	    	    // time length left to reach the target amplitude (expressed in
					// number of BUCKETNBSAMPLE and NOT in samples)
	    Envelope::iterator ienv;    // the next envelope description (envelope) iterator (envelope envelope)
    };

    // ---------------------------------------------------------------------------------

    class EnvOsc : public EnvGen, public Oscillator
    {
	public:
	    EnvOsc (int freq) : Oscillator (freq) { }
	    EnvOsc (Envelope & envelope, Oscillator & o) : EnvGen (envelope), Oscillator (o) { }
	    inline Uint16 bucketCompNoErase (void) {
		return amp = EnvGen::bucketComp ();
	    }
	    inline Uint16 bucketComp (void) {
		bucketCompNoErase ();
		if (isfinished) lo.erase (isonge);
		return amp;
	    }
	    inline Sint16 getSample (void) {
		return (Sint16) (((Sint32)Oscillator::getSample() * amp) >> 15);
	    }

	    inline void rewind (void) { EnvGen::rewind (); }
	private:
	    Uint16 amp;
    };

    // ---------------------------------------------------------------------------------

    bool initaudio(size_t nbsamples = 1024);

};
#endif // BB_AUDIO_H_INC

