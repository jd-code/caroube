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

#ifndef   CAROUBE_H_INC
#warning samplesonge.h should not be included as is. caroube.h should be included instead
#endif // CAROUBE_H_INC

#ifndef SAMPLESONGE_H_INC
#define SAMPLESONGE_H_INC

namespace caroube
{
    class SampleSonGe : public SonGe, public Sample
    {
	    Sint16 *cur;		//!< the current pointer to the next sample to be played
	    Sint16 *end;		//!< a pointer to the first non-valid sample (not to be referenced !)
	    int inc;			//!< how much should we increment the current pointer
	public:
	    SampleSonGe (const char * nomfile) : SonGe () {
		Sample::readsample (nomfile);
		if (Sample::data != NULL) {
		    end = Sample::data + Sample::size ;
		    inc = Sample::nbch ;
		    setname (Sample::name);
		} else {
		    setname ("empty");
		    end = NULL;
		    inc = 0;
		}
		cur = Sample::data;
	    }

	    SampleSonGe (void) : SonGe () {
		end = NULL;
		cur = NULL;
		inc = 0;
	    }

	    bool actualize_sample (void)
	    {
		if (Sample::data != NULL) {
		    end = Sample::data + Sample::size ;
		    inc = Sample::nbch ;
		    cur = Sample::data;
		    setname (Sample::name);
		    return true;
		} else {
		    end = NULL;
		    inc = 0;
		    cur = NULL;
		    setname ("empty");
		    return false;
		}
	    }

	    SampleSonGe (Sample & smpl) : SonGe () {
		end = NULL;
		cur = NULL;
		inc = 0;
		sharesample (smpl);
		actualize_sample ();
	    }

	    SampleSonGe (SampleSonGe & smplsg) : SonGe () {
		end = NULL;
		cur = NULL;
		inc = 0;
		sharesample ((Sample &)smplsg);
		// sharesample (*((Sample *)&smplsg));
		actualize_sample ();
	    }

	    bool readsample (const char *nomfile) {
		Sample::readsample (nomfile);
		return actualize_sample ();
	    }

	    virtual inline Sint16 getSample (void) {
		if (cur < end) {
		    Sint16 temp = *cur;
		    cur += inc;
		    return temp;
		} else
		    return 0;
	    }

	    virtual inline Uint16 bucketComp (void) {
		if (cur >= end) {
		    caroube::lo.erase (isonge);
		    cur = Sample::data;
		}
		return 0;
	    }

	    virtual inline void rewind (void) {
		cur = Sample::data;
	    }
    };

//    class SampleSonGeFFT : public SampleSonGe
//    {
//	public :
//	    double lo, me, hi;
//
//	    void lomehi (void); //!< computes the lo, me, hi magical values with fft
//
//	    inline int whichlomehi (void)	//!< returns which of lo, me or hi is the greatest
//		{   if (lo >= me) {
//			if (lo >= hi)
//			    return 1;	// LO
//			else
//			    return 3;   // HI
//		    } else {
//			if (me >= hi)
//			    return 2;	// ME
//			else
//			    return 3;	// HI
//		    }
//		}
//    };
};

#endif // SAMPLESONGE_H_INC

/*
 * $Log: samplesonge.h,v $
 * Revision 1.3  2003/10/03 21:25:38  jd
 * moved fft properties frem samplesongefft to sample itself
 *
 * Revision 1.2  2003/09/29 22:24:21  jd
 * added fft.c/h
 * added sharing samples in samplesonge
 *
 * Revision 1.1.1.1  2003/09/24 22:53:44  jd
 * librarized it
 *
 * Revision 1.1.1.1  2003/09/11 23:52:11  jd
 * first importation
 *
 * Revision 1.2  2003/08/14 14:10:21  jd
 * cleaned comments for doxygen
 * added fft
 * added sample sorting with lomehi
 *
 * Revision 1.1.1.1  2003/06/04 13:56:51  jd
 * caroube librarised
 *
 * Revision 1.4  2003/02/27 23:38:25  jd
 * nested all audio into namespace "caroube"
 *
 * Revision 1.3  2003/02/25 16:15:00  jd
 * extended name-use in samplesonge inheritants
 *
 * Revision 1.2  2003/02/14 16:09:05  cristelle
 * *** empty log message ***
 *
 * Revision 1.1  2003/01/18 12:17:55  jd
 * merged genetics + SonGe files
 * first release with sounds
 * cleanup for cvs
 *
 *
 */
