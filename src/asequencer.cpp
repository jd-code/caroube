/*
 *  $Id: asequencer.cpp,v 1.1 2003/09/24 22:53:44 jd Exp $
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


// #include "asequencer.h"
#include "caroube.h"

namespace caroube
{
    SonGeSeq::SonGeSeq (void)
    {
	moretoplay = false;
	setbpm (80);
	nbv = 0;
	lo.push (this);
    }

    SonGeSeq::~SonGeSeq (void)  // JDJDJDJD probably a lot of stuff to do here !!!
    {
    }

    size_t SonGeSeq::push (SonGe * ps)
    {
	vo.push_back (ps);
	nbv ++;
	ttt = 0;

	return nbv;
    }

    size_t SonGeSeq::feedraw (vector <int> & row)
    {
	vector<int> v;
	vector<int>::iterator i;
	size_t n;
	
	for (i=row.begin(), n=0; (i!=row.end()) && (n<nbv); i++, n++) {
	    v.push_back (*i);
	}
	if (n != nbv) {
	    if (n > nbv)
		cerr << "SonGeSeq::feedraw was fed a too long vector !" << endl ;
	    else {
		cerr << "SonGeSeq::feedraw was fed a too short vector !" << endl ;
		while (n < nbv) {
		    v.push_back (0);
		    n++;
		}
	    }
	}
	sched.push_back (v);
	moretoplay = true;
	return vo.size ();
    }

    bool SonGeSeq::setbpm (int bpm)
    {
	if ((bpm > 20) && (bpm <= 4*500)) {
	    SonGeSeq::bpm = bpm;
	    nttt = 60*44100 / (BUCKETNBSAMPLE * bpm);
	    return true;
	}
	return false ;
    }

    Uint16 SonGeSeq::bucketComp (void)
    {
	if (ttt > 0) {
	    ttt --;
	    return 0;
	}
	ttt = nttt;
	if (sched.size () > 0) {
	    vector<int> &v = *sched.begin();
	    vector<int>::iterator iv;
	    size_t i;

	    for (i=0,iv=v.begin() ; (i<nbv) && (iv!=v.end()) ; i++, iv++) {
		if (*iv != 0) {	// we have to play this voice
		    // cerr << "playing voice " << i << " at level " << *iv << "(bpm=" << (60*44100/(4*BUCKETNBSAMPLE*nttt)) << ")" << endl ;
		    vo[i]->rewind ();
		    lo.push (vo[i]);
		    vo[i]->vr = ((*iv) << 3); // 8 * 8 = 64
		    vo[i]->vl = ((*iv) << 3); // 8 * 8 = 64
		}
	    }
	    sched.pop_front();
	}
	else
	    moretoplay = false;
	
	return 0;
    }

    void SonGeSeq::flush (void)
    {
	sched.erase (sched.begin(), sched.end());	// we rely on bucketComp for setting moretoplay to false afterward...
    }

    static const string errname ("error");

    const string & SonGeSeq::getname (int iv)	//!< returns the vernacular name of the i-th channel
    {
	if ((iv >= 0)  &&  (iv < (int)nbv))
	    return vo[iv]->getname ();
	else
	    return errname;
    }
};

/*
 * $Log: asequencer.cpp,v $
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
