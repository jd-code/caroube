/* 
 *  les fonctions de lecture d'echantillons...
 *
 * GenRythm Copyright (C) 2002,2003 Cristelle Barillon & Jean-Daniel Pauget
 * making genetics and groovy sound/noise/rythms
 *
 * genrythm@disjunkt.com  -  http://genrythm.disjunkt.com/
 *
 * $Id$
 * Groovit Copyright (C) 1998,1999 Jean-Daniel PAUGET
 * making accurate and groovy sound/noise
 *
 * jd@disjunkt.fr  -  http://groovit.disjunkt.com/
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
#endif  // CAROUBE_H_INC

#ifndef READSAMPLE_H_INC
#define READSAMPLE_H_INC

namespace caroube
{
    using namespace std ;

    //! reading and storing some sampled sounds
    /*!	can read wav and aiff files, up to stereo
     *  (more channels is to be tested though its already written). JDJD
     */
    class Sample
    {
	private:
	    Sample (const Sample & s);			    //! Not Allowed !
	    Sample & operator= (const Sample & s);	    //! Not Allowed !

	protected:
	    Sint16  *data;  /*!< \brief	    pointer to the sample's datas in signed-16bits enterlaced (nbchan) */
	    int *   Pnbref; /*!< \brief	    a pointer to where to get how many times *data is referenced */

	    Sint32  size;   /*!< \brief	    size of the sample (0 means invalid or not yet valid */
	    Sint16  nbch;   /*!< \brief	    number of channels */
	    string  name,   /*!< \brief	    the displayed name of the sample */
		    fname;  /*!< \brief	    the filename of the sample */
	public:
	    ~Sample (void);
	    Sample (void);

	    //! frees the matching sample buffer and counts references to it
	    void free (void);

	    //! alocates the matching sample buffer and counts references to it
	    bool allocate (size_t s);

	    //! creates a new sample entry without duplicating the samples data
	    Sint16 * sharesample (Sample & s);

	    Sint16 * readsample_wav  (const char *nomfile);
	    Sint16 * readsample_wav  (istream & cin, bool notpreread=true);
	    Sint16 * readsample_aiff (const char *nomfile);
	    Sint16 * readsample_aiff (istream & pcin, bool notpreread=true);

	    
	    //! generic reading of a sample
	    /*! tries to load the file according to its extension.
	     *  Even if the extension doesn't match anything known, it will try
	     *  every common file formats one after the other.
	     *
	     *  Drawbacks : this function isn't schedulled and may block the main process*
	     *  until the sample is read.
	     */
	    Sint16 * readsample	 (const char *nomfile);

	    double lo, me, hi;

	    void lomehi (void); //!< computes the lo, me, hi magical values with fft

	    inline int whichlomehi (void)	//!< returns which of lo, me or hi is the greatest
		{   if (lo >= me) {
			if (lo >= hi)
			    return 1;	// LO
			else
			    return 3;   // HI
		    } else {
			if (me >= hi)
			    return 2;	// ME
			else
			    return 3;	// HI
		    }
		}
    };
};

#endif // READSAMPLE_H_INC

