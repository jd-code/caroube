/*
 *  $Id: asequencer.h,v 1.1 2003/09/24 22:53:44 jd Exp $
 *
 *  jd - 2003/01/16 a Songe sequencer for genrythm
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
#warning asequencer.h should not be included as is. caroube.h should be included instead
#endif // CAROUBE_H_INC

#ifndef GR_ASQNCR_H_INC
#define GR_ASQNCR_H_INC

#include <vector>

namespace caroube
{

    class SonGeSeq : private SonGe
    {
	private:
	    vector<SonGe *> vo;		//!< the list of SonGe matching the channels
	    size_t nbv;			//!< the number of channels (/voices)
	    int bpm;			//!< the current bpm
	    list <vector<int> > sched;
	    int ttt;			//!< decounter : time to (next) 'tick'...
	    int nttt;			//!< next reload for the decounter
	
	    Uint16 bucketComp (void);
	    SonGeSeq (SonGeSeq &);	//!< no copy-constructor for SonGeSeq...
					// if needed though, SonGeSeq reference can be used...
	    SonGeSeq& operator=(SonGeSeq&);  //!< private ! not allowed !
	
	public:
	    bool moretoplay;
	    SonGeSeq (void);			//!< constructor
	    size_t push (SonGe * ps);		//!< adding a voice
	    bool setbpm (int bpm);			//!< setting the bpm
	    size_t feedraw (vector <int> & row);	//!< feeding a raw
	    void flush (void);			//!< empty every schedulled events
	    const string & getname (int iv);	//!< the vernacular name of the i-th channel
	    ~SonGeSeq ();
    };
};

#endif // GR_ASQNCR_H_INC

/*
 * $Log: asequencer.h,v $
 * Revision 1.1  2003/09/24 22:53:44  jd
 * Initial revision
 *
 * Revision 1.1.1.1  2003/09/11 23:52:11  jd
 * first importation
 *
 * Revision 1.2  2003/08/14 14:10:20  jd
 * cleaned comments for doxygen
 * added fft
 * added sample sorting with lomehi
 *
 * Revision 1.1.1.1  2003/06/04 13:56:51  jd
 * caroube librarised
 *
 * Revision 1.5  2003/03/20 17:41:05  jd
 * conceptual correction for SonGeSeq's copy constructor
 *
 * Revision 1.4  2003/02/27 23:38:25  jd
 * nested all audio into namespace "caroube"
 *
 * Revision 1.3  2003/02/25 16:15:00  jd
 * extended name-use in samplesonge inheritants
 *
 * Revision 1.2  2003/02/08 00:18:42  jd
 * added flush
 *
 * Revision 1.1  2003/01/18 12:17:55  jd
 * merged genetics + SonGe files
 * first release with sounds
 * cleanup for cvs
 *
 *
 */
