/*
 *  $Id$
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

