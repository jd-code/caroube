/* 
 * $Id$
 * GenRythm Copyright (C) 2002,2003 Cristelle Barillon & Jean-Daniel Pauget
 * making genetics and groovy sound/noise/rythms
 *
 * genrythm@disjunkt.com  -  http://genrythm.disjunkt.com/
 * 
 * Groovit Copyright (C) 1998,1999 Jean-Daniel PAUGET
 * making accurate and groovy sound/noise
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

#include <stdio.h>
#include <stdlib.h>
/* #include <fcntl.h> */
#include <math.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include <fstream>

#include "conv_ieee_ext.h"
#include "voncendian.h"
// #include "readsample.h"
#include "caroube.h"
#include "fft.h"

namespace caroube
{

/* format AIFF ............................................................ */
#define AIFFBUFSIZE 1024	/* taille du buffer intermediaire pour lecture AIFF */
#define CHUNKFORM 0x4d524f46
#define CHUNKAIFF 0x46464941
#define CHUNKNAME 0x454d414e	/* NAME */
#define CHUNKAUTH 0x48545541	/* AUTHor */
#define CHUNKRIGH 0x20296328	/* "(c) " */
#define CHUNKANNO 0x4f4e4e41	/* ANNOtation */
#define CHUNKCOMT 0x544d4f43	/* COMmenT */
#define CHUNKAPPL 0x4c505041	/* APPLication specifik chunk */
#define CHUNKAESD 0x44534541	/* AESD Audio rEcording SounD chunk */
#define CHUNKMIDI 0x4944494d	/* MIDI chunk */
#define CHUNKINST 0x5453494e	/* INSTrument chunk */
#define CHUNKMARK 0x4b52414d	/* INSTrument chunk */
#define CHUNKCOMM 0x4d4d4f43	/* COMMon chunk */
#define CHUNKSSND 0x444e5353	/* Sampled SouND chunk */

/* format WAV ............................................................. */
#define CHUNKRIFF    0x46464952	/* RIFF container */
#define CHUNKWAVE    0x45564157
#define CHUNKwavFMT  0x20746d66	/* ForMaT definitions */
#define CHUNKwavdata 0x61746164	/* DATA chunk */
#define CHUNKwavLIST 0x5453494c	/* wave-LIST chunk -not handled */
#define CHUNKwavslnt 0x746e6c73	/* silent chunk -not handled */
#define CHUNKwavfact 0x74636166	/* FACT chunk -not handled */
#define CHUNKwavcue  0x20657563	/* CUE-points postions chunk -not handled */
#define CHUNKwavplst 0x74736c70	/* Playing LiST chunk -not handled */

#ifdef SHOWRCS
static char *rcsid = "$Id$";

#endif

/* USELESS because we need to read a 80bit (!) ieee 754 float */
char   *CONVdouble (char *p)
{
    char    s, *q = p, *r = p + 7;
    int     i;

    for (i = 0; i < 4; i++)
    {
	s = *p;
	*q++ = *r;
	*r-- = s;
    }
    return p;
}

Sample::Sample (void) {
    Pnbref = NULL;
    data = NULL;
    size = 0;
    nbch = 0;
}

void Sample::free (void) {
    size = 0;
    if (Pnbref != NULL) {
	(*Pnbref) --;
	if (*Pnbref == 0) {
	    ::free (Pnbref);
	    Pnbref = NULL;
	}
    }
    data = NULL;
    name = "";
    fname = "";
    nbch = 0;
}

bool Sample::allocate (size_t s) {

    if (Pnbref != NULL)
	free ();

    Pnbref = (int *) malloc (s + sizeof (int));
    if (Pnbref != NULL) {
	data = (Sint16 *)(Pnbref + 1);
	*Pnbref = 1;
	return true;
    } else {
	data = NULL;
	return false;
    }
}

Sample::~Sample (void) {
    free ();
}

Sint16 * Sample::sharesample (Sample & s) {
    if (data != NULL) {
	cerr << "Sample::sharesample: I dont overwrite samples, it must be freed before !" << endl ;
	return data ;
    }
    free ();

    if (s.Pnbref == NULL)
	return NULL;

    data = s.data;
    Pnbref = s.Pnbref;
    (*Pnbref) ++;
    size = s.size;
    nbch = s.nbch;
    name = s.name;
    fname = s.fname;

    return data;
}

Sint16 * Sample::readsample_wav (const char *nomfile) {
    if (data != NULL) {
	cerr << "Sample::readsample_wav: I dont overwrite samples, it must be freed before !" << endl ;
	return data ;
    }
    free ();
    
    ifstream ifile (nomfile, ios::binary);
    if (!ifile) {
	int e = errno;
	cerr << "Sample::readsample_wav: could not open " << nomfile << " : " << strerror (e) << endl ;
    }

    struct stat Sfstat;

    stat (nomfile, &Sfstat);

    Sint32 buf[3];
    ifile.read ((char*)buf, 12);   /* lecture RIFF container chunck */
    if (!ifile) {
	int e = errno ;
	cerr << "Sample::readsample_wav : failed reading the first open chunk : " << strerror (e) << endl ;
	return NULL;
    }

    if ((chunkconv (buf[0]) != CHUNKRIFF) || (chunkconv (buf[2]) != CHUNKWAVE)) {
	cerr << nomfile << "isn't wave compliant: first chunk not RIFFxxxxWAVE" << endl ;
	return NULL;
    }
    if (VONClong (buf[1]) + 8 != Sfstat.st_size)    // we warn but proceed with login anyway...
	cerr << nomfile << "hasn't expected size: "
	     << Sfstat.st_size << "instead of "
	     << VONClong (buf[1] + 8)
	     << endl ;

    fname = nomfile;
    // the name comes from the filename...
    size_t p = fname.find_last_of ("/\\");
    if (p == string::npos)
	name = fname;
    else 
	name = fname.substr(p + 1);
    p = name.find_last_of (".");
    if (p != string::npos)
	name = name.substr(0, p);

    return readsample_wav (ifile, false) ;
}

    
Sint16 * Sample::readsample_wav (istream & cin, bool notpreread) {
    int     skipped = 0;
    Sint32    taille = 0;
    Sint16  *ps = NULL;
    Sint32    nbsample = 0, n;
    int     realsamplesize = 0, samplesize = 0, nbchannel = 0, framesize = 0;
    Sint32    rate = 0;
    int	    nbreadch = 0;
    int	    ch;

    if (notpreread) {
	if (data != NULL) {
	    cerr << "Sample::readsample_wav: I dont overwrite samples, it must be freed before !" << endl ;
	    return data ;
	}
	free ();

	Sint32 buf[3];
	cin.read ((char *)buf, 12);	/* lecture RIFF container chunck */
	if (!cin) {
	    int e = errno ;
	    cerr << "Sample::readsample_wav : failed reading the first open chunk : "
		 << strerror (e)
		 << endl ;
	    return NULL;
	}

	if ((chunkconv (buf[0]) != CHUNKRIFF) || (chunkconv (buf[2]) != CHUNKWAVE))
	{
	    cerr << fname << "isn't wave compliant: first chunk not RIFFxxxxWAVE" << endl ;
	    return NULL;
	}
	// there is no test with global length when using on-the-fly streams
    }

    unsigned char buf [AIFFBUFSIZE],
	    *bbuf;
    
    while (cin)
    {
	if (!cin.read ((char *)buf, 8))
	    break;
	skipped = 0;
	taille = VONClong ((*(Sint32 *) (buf + 4)));
	switch (chunkconv (*(Sint32 *) buf)) {

	  case CHUNKwavFMT:
	      if (taille > AIFFBUFSIZE) {
		  cerr << "Sample::readsample_wav : " << fname << " : FMT chunk too big, skipping "
		       << taille << "bytes" << endl ;
		  skipped = 1;
		  break;
	      }
	      cin.read ((char *)buf, taille) ;
	      if (VONCint (*(Sint16 *) buf) != 1)
		  cerr << "Sample::readsample_wav : " << fname
		       << " : data type others than PCM not handled" << endl ;
	      nbchannel = VONCint ((*(Sint16 *) (buf + 2)));
	      if ((nbchannel < 1) || (nbchannel > 2))
		  cerr << "Sample::readsample_wav : " << fname
		       << " : " << nbchannel << " data-channels, only the two firsts will be used"
		       << endl ;
	      nbreadch = nbchannel > 2 ? 2 : nbchannel;
	      nbch = nbreadch;
	      rate = VONClong ((*(Sint32 *) (buf + 4)));
	      realsamplesize = VONCint ((*(Sint16 *) (buf + 14)));
	      framesize = VONCint ((*(Sint16 *) (buf + 12)));

	      samplesize = realsamplesize;
	      if ((samplesize <= 32) && (samplesize > 0)) {
		  if (samplesize < 9)
		      samplesize = 1;
		  else if (samplesize < 17)
		      samplesize = 2;
		  else if (samplesize < 25)
		      samplesize = 3;
		  else
		      samplesize = 4;
	      } else {
		  cerr << "Sample::readsample_wav : " << fname
		       << " cannot handle samplesize, considering wrong FMT_ chunk" << endl ;
		  samplesize = 0;
	      }
	      if (framesize != samplesize * nbchannel)
		  cerr << "Sample::readsample_wav : " << fname
		       << ": framesize " << framesize << " is different from samplesize "
		       << samplesize << " * " << nbchannel << " ???" << endl ;

	      break;

	  case CHUNKwavdata:
	      if (samplesize == 0) {
		  cerr << "Sample::readsample_wav : " << fname
		       << " : entering data chunk without FMT_ chunk, skipped "
		       << taille << "bytes" << endl ;
		  skipped = 1;
		  break;
	      }
	      if (data != NULL) {
		  cerr << "Sample::readsample_wav : " << fname
		       << " : a second sample sound data chunk was found, "
		       << taille << " bytes skipped" << endl ;
		  skipped = 1;
		  break;
	      }
	      nbsample = taille / framesize;
#ifdef DEB_READSAMPLE
	       cerr << name << nbchannel << " channels of " <<
		    << nbsample << " x " << realsamplesize << " bits at "
		    << rate << " Hz (framesize " << framesize << ")" << endl ;
#endif
	    /* allocation du sample qui sera converti */
	      // if ((data = (Sint16 *) malloc (nbsample * nbchannel * sizeof (Sint16))) == NULL)
	      if (!allocate (nbsample * nbchannel * sizeof (Sint16)))
	      {
		  cerr << "Sample::readsample_wav : " << fname
		       << " : could not allocate " << nbsample * nbchannel * sizeof (Sint16)
		       << " bytes ??" << endl ;
		  skipped = 1;
		  break;
	      }
	      ps = data;
	      switch (samplesize) {
		case 1:
		    for (n = 0; n < nbsample; n++) {
			// if (fread (buf, 1, framesize, f) != framesize)
			if (!cin.read ((char *)buf, framesize))
			    break;
			for (ch=0 ; ch<nbreadch ; ch++) {
			    bbuf = &buf[ch * samplesize];
			    bbuf[0] ^= 0x80;
#ifndef DOWNSAMPLE
			    *ps++ = bbuf[0] << 8;
#else
			    if ((n & 1) == 0)
				*ps++ = bbuf[0] << 8;
#endif
			}
		    }
		    break;
		case 2:
		case 3:
		case 4:
		    for (n = 0; n < nbsample; n++) {
			// if (fread (buf, 1, framesize, f) != framesize)
			if (!cin.read ((char *)buf, framesize))
			    break;
			for (ch=0 ; ch<nbreadch ; ch++) {
			    bbuf = &buf[ch * samplesize];
#if (BYTE_ORDER == LITTLE_ENDIAN)
#ifndef DOWNSAMPLE
			*ps++ = *(Sint16 *) bbuf;
#else
			if ((n & 1) == 0)
			    *ps++ = *(Sint16 *) bbuf;
#endif
#else
#ifndef DOWNSAMPLE
			*ps++ = (bbuf[0] << 8) | bbuf[1];
#else
			if ((n & 1) == 0)
			    *ps++ = (bbuf[0] << 8) | bbuf[1];
#endif
#endif
			}
		    }
		    break;
		default:
		    cerr << "Sample::readsample_wav: big internal problem #0001" << endl ;
		    exit (1); // JDJDJDJD this is not very good
	      }
#ifndef DOWNSAMPLE
	      size = n;
#else
	      size = n >> 1;
#endif
	      break;

	  case CHUNKwavLIST:
	  case CHUNKwavslnt:
	  case CHUNKwavfact:
	  case CHUNKwavcue:
	  case CHUNKwavplst:
	      skipped = 1;
	      break;

	  default:
	      // fprintf (stderr, "%s: %c%c%c%c (0x%08lx) invalid chunk mark, skipping %ld bytes\n\r",
	      cerr << "Sample::readsample_wav: " << fname << " : "
	           << (buf[0] >= 32 ? buf[0] : '?')
	           << (buf[1] >= 32 ? buf[1] : '?')
	           << (buf[2] >= 32 ? buf[2] : '?')
	           << (buf[3] >= 32 ? buf[3] : '?')
		   << "(" << *(Sint32 *) buf << ") invalid chunk mark, skipping "
		   << taille << " bytes " << endl ;
	      skipped = 1;
	  }
	if (skipped)
	    cin.seekg (taille, ios::cur);
	    // fseek (f, taille, SEEK_CUR);
    }
    return data;
}

Sint16 * Sample::readsample_aiff (const char *nomfile) {
    if (data != NULL) {
	cerr << "Sample::readsample_aiff: I dont overwrite samples, it must be freed before !" << endl ;
	return data ;
    }
    free ();
    
    ifstream ifile (nomfile, ios::binary);
    if (!ifile) {
	int e = errno;
	cerr << "Sample::readsample_aiff: could not open " << nomfile << " : " << strerror (e) << endl ;
    }

    struct stat Sfstat;

    stat (nomfile, &Sfstat);

    Sint32 buf[3];
    
    ifile.read ((char *)buf, 12);	/* lecture chunk FORM */
    if (!ifile) {
	int e = errno ;
	cerr << "Sample::readsample_aiff : " << nomfile
	     << " : failed reading the first open chunk : " << strerror (e) << endl ;
	return NULL;
    }

    if ((chunkconv (buf[0]) != CHUNKFORM) || (chunkconv (buf[2]) != CHUNKAIFF))
    {
	cerr << "Sample::readsample_aiff : " << nomfile
	     << " isnt aiff compliant: first chunk not COMMxxxxAIFF" << endl ;
	return NULL;
    }
    if (CONVlong (buf[1]) + 8 != Sfstat.st_size)
	cerr << "Sample::readsample_aiff : " << nomfile
	     << " : hasnt expected size: " << Sfstat.st_size
	     << " instead of " << CONVlong (buf[1]) + 8 << endl ;

    fname = nomfile;
    // the name comes from the filename...
    size_t p = fname.find_last_of ("/\\");
    if (p == string::npos)
	name = fname;
    else 
	name = fname.substr(p + 1);
    return readsample_wav (ifile, false) ;
}
 
Sint16 * Sample::readsample_aiff (istream & cin, bool notpreread) {
    int     skipped = 0;
    Sint32    taille = 0;
    Sint16  *ps = NULL;
    Sint32    nbsample = 0, n;
    int     samplesize = 0, nbchannel = 0, framesize = 0;
    Sint32    rate = 0;
    int	    nbreadch = 0;
    int	    ch;

    if (notpreread) {
	if (data != NULL) {
	    cerr << "Sample::readsample_wav: I dont overwrite samples, it must be freed before !" << endl ;
	    return data ;
	}
	free ();

	Sint32 buf[3];
	cin.read ((char *)buf, 12);	/* lecture chunk FORM */
	if (!cin) {
	    int e = errno ;
	    cerr << "Sample::readsample_aiff : " << fname
		 << " : failed reading the first open chunk : " << strerror (e) << endl ;
	    return NULL;
	}

	if ((chunkconv (buf[0]) != CHUNKFORM) || (chunkconv (buf[2]) != CHUNKAIFF))
	{
	    cerr << "Sample::readsample_aiff : " << fname
		 << " isnt aiff compliant: first chunk not COMMxxxxAIFF" << endl ;
	    return NULL;
	}
	// there is no test with global length when using on-the-fly streams
    }

    unsigned char buf[AIFFBUFSIZE],
	    * bbuf;

    while (cin) {
	if (!cin.read ((char *)buf, 8))
	    break;
	skipped = 0;
	taille = CONVlong (*(Sint32 *) (buf + 4));
	switch (chunkconv (*(Sint32 *) buf)) {
	  case CHUNKNAME:
	      if (taille > AIFFBUFSIZE - 1) {
		  cerr << "Sample::readsample_aiff : " << fname
		       << " : name chunk too big, skipped" << endl ;
		  skipped = 1;
	      } else {
		  cin.read ((char *)buf, taille);
		  buf[taille] = 0;
		  cerr << "NAME: " << buf << endl ;
		  name = (char *) (&buf[0]) ;
	      }
	      break;
	  case CHUNKAUTH:
	      if (taille > AIFFBUFSIZE - 1) {
		  cerr << "Sample::readsample_aiff : " << fname
		       << " : author chunk too big, skipped" << endl ;
		  skipped = 1;
	      } else {
		  cin.read ((char *)buf, taille);
		  buf[taille] = 0;
		  cerr << fname << ": author: " << buf << endl ;
	      }
	      break;
	  case CHUNKRIGH:
	      if (taille > AIFFBUFSIZE - 1) {
		  cerr << "Sample::readsample_aiff : " << fname
		       << " : copyright chunk too big, skipped" << endl ;
		  skipped = 1;
	      }
	      else {
		  cin.read ((char *)buf, taille);
		  buf[taille] = 0;
		  cerr << fname << ": (c) " << buf << endl ;
	      }
	      break;
	  case CHUNKANNO:
	      if (taille > AIFFBUFSIZE - 1) {
		  cerr << "Sample::readsample_aiff : " << fname
		       << " : annotation chunk too big, skipped" << endl ;
		  skipped = 1;
	      }
	      else {
		  cin.read ((char *)buf, taille);
		  buf[taille] = 0;
		  cerr << fname << ": Annotation : " << buf << endl ;
	      }
	      break;
	  case CHUNKCOMT:
	    /* JDJD BUG: the comment chunk isnt handled */
	      skipped = 1;
	      break;
	  case CHUNKAPPL:
	  case CHUNKAESD:
	  case CHUNKMIDI:
	  case CHUNKINST:
	  case CHUNKMARK:
	      skipped = 1;
	      break;
	  case CHUNKCOMM:
	      if (taille > AIFFBUFSIZE) {
		  cerr << "Sample::readsample_aiff : " << fname
		       << " : COMM chunk too big, skipping " << taille << " bytes" << endl ;
		  skipped = 1;
		  break;
	      }
	      cin.read ((char *)buf, taille);
	      nbchannel = CONVint (*(Sint16 *) buf);
	      if ((nbchannel < 1) || (nbchannel > 2))
		  cerr << "Sample::readsample_aiff : " << fname
		       << nbchannel << " data-channels, only the two firsts will be used" << endl ;
	      nbreadch = nbchannel > 2 ? 2 : nbchannel;
	      nbch = nbreadch;
	      nbsample = CONVlong (*(Sint32 *) (buf + 2));
	      samplesize = CONVint (*(Sint32 *) (buf + 6));
	      rate = (Sint32) ConvertFromIeeeExtended (buf + 8);
/* JDJDJDJD should go to info ? */
#ifdef DEB_READSAMPLE
	      cerr << fname << " : " << nbchannel << " channels of " << nbsample << " x "
		   << samplesize << " bits at " << rate " Hz" << endl ;
#endif
	      if ((samplesize <= 32) && (samplesize > 0)) {
		  if (samplesize < 9)
		      samplesize = 1;
		  else if (samplesize < 17)
		      samplesize = 2;
		  else if (samplesize < 25)
		      samplesize = 3;
		  else
		      samplesize = 4;
	      } else {
		  cerr << "Sample::readsample_aiff : " << fname
		       << " : cannot handle samplesize, considering wrong COMM chunk" << endl ;
		  samplesize = 0;
	      }
	      framesize = samplesize * nbchannel;
/* JDJDJDJD should go to info ? */
/* fprintf (stderr, "samplesize %d bytes - framesize %d bytes\n\r", samplesize, framesize); */

	      break;
	  case CHUNKSSND:
	      if (samplesize == 0) {
		  cerr << "Sample::readsample_aiff : " << fname
		       << " : entering sound chunk without COMM chunk, skipped "
		       << taille << "bytes" << endl ;
		  skipped = 1;
		  break;
	      }
	      if (data != NULL) {
		  cerr << "Sample::readsample_aiff : " << fname
		       << " : a second sample sound chunk was found, skipped "
		       << taille << "bytes" << endl ;
		  skipped = 1;
		  break;
	      }
	    /* allocation du sample qui sera converti */
	      // if ((data = (Sint16 *) malloc (nbsample * nbreadch * sizeof (Sint16))) == NULL) {
	      if (!allocate (nbsample * nbreadch * sizeof (Sint16))) {
		  cerr << "Sample::readsample_aiff : " << fname
		       << " : could not allocate " << nbsample * nbreadch * sizeof (Sint16)
		       << " bytes ??" << endl ;
		  skipped = 1;
		  break;
	      }
	      cin.read ((char *)buf, 8);
	      if ((*(Sint32 *) buf != 0) || (*(Sint32 *) (buf + 4) != 0))
		  cerr << "Sample::readsample_aiff : " << fname
		       << " : non-null offset or blocksize, ignored" << endl ;
	      ps = data;
	      switch (samplesize)
		{
		case 1:
		    for (n = 0; n < nbsample; n++)
		    {
			if (!cin.read ((char *)buf, framesize))
			    break;
			for (ch=0 ; ch<nbreadch ; ch++)
			{   bbuf = &buf[ch * samplesize];
#ifndef DOWNSAMPLE
			    *ps++ = bbuf[0] << 8;
#else
			    if ((n & 1) == 0)
				*ps++ = bbuf[0] << 8;
#endif
			}
		    }
		    break;
		case 2:
		case 3:
		case 4:
		    for (n = 0; n < nbsample; n++)
		    {
			if (!cin.read ((char *)buf, framesize))
			    break;
			for (ch=0 ; ch<nbreadch ; ch++)
			{   bbuf = &buf[ch * samplesize];
#if (BYTE_ORDER == LITTLE_ENDIAN)
#ifndef DOWNSAMPLE
			    *ps++ = (bbuf[0] << 8) | bbuf[1];
#else
			    if ((n & 1) == 0)
				*ps++ = (bbuf[0] << 8) | bbuf[1];
#endif
#else
#ifndef DOWNSAMPLE
			    *ps++ = *(Sint16 *) bbuf;
#else
			    if ((n & 1) == 0)
				*ps++ = *(Sint16 *) bbuf;
#endif
#endif
			}
		    }
		    break;
		default:
		    cerr << "Sample::readsample_aiff : big internal problem #0002" << endl ;
		    exit (1);
		}
#ifndef DOWNSAMPLE
	      size = n;
#else
	      size = n >> 1;
#endif
	      break;
	  default:
           // fprintf (stderr, "%s: %c%c%c%c (0x%08lx) invalid chunk mark, skipping %ld bytes\n\r",
	      cerr << "Sample::readsample_wav: " << fname << " : "
	           << (buf[0] >= 32 ? buf[0] : '?')
	           << (buf[1] >= 32 ? buf[1] : '?')
	           << (buf[2] >= 32 ? buf[2] : '?')
	           << (buf[3] >= 32 ? buf[3] : '?')
		   << "(" << *(Sint32 *) buf << ") invalid chunk mark, skipping "
		   << taille << " bytes " << endl ;
	      skipped = 1;
	  }
	if (skipped)
	    cin.seekg (taille, ios::cur);
	    // fseek (f, taille, SEEK_CUR);
    }
    return data;
}

Sint16 * Sample::readsample (const char *nomfile) {
    char   *p = NULL;
    Sint16  *tempo;

#ifdef SHOWRCS
static bool firstuse = true;
    if (firstuse) {
	cerr << rcsid << endl ;
	firstuse = false;
    }
#endif

    if (nomfile == NULL)
	return NULL;

    p = strrchr (nomfile, '.');
    if (p != NULL)
	switch (chunkconv (*(Sint32 *) p)) {
	  case 0x7661772e:	/* .wav */
	  case 0x5641572e:	/* .WAV */
	  case 0x7661572e:	/* .Wav */
	      return readsample_wav (nomfile);
	  case 0x6669612e:	/* .aif */
	  case 0x4649412e:	/* .AIF */
	  case 0x6669412e:	/* .Aif */
	      return readsample_aiff (nomfile);
	  default:
	      break;
	}

    cerr << "could not find out nature of " << nomfile << ", trying wav..." << endl ;
    if ((tempo = readsample_wav (nomfile)) != NULL)
	return tempo;
    cerr << "not good, trying aiff..." << endl ;
    if ((tempo = readsample_aiff (nomfile)) != NULL)
	return tempo;
    cerr << "not good, aborting..." << endl ;
    return NULL;
}

void Sample::lomehi (void)
{
    lo = 0.0, me = 0.0, hi = 0.0;

    if (Sample::data == NULL) {
cerr << "lomehi for " << Sample::name << "    [undefined]" << endl ;
	return ;
    }

    double re[2048], im[2048];
    Sint16 *cur, *end;
    int inc;
    int i;
    
    cur = Sample::data;
    end = Sample::data + Sample::size ;
    inc = Sample::nbch ;
    
    while (cur < end) {
	for (i=0 ; i<2048 ; i++) {
	    im[i] = 0.0;
	    if (cur < end) {
		re[i] = *cur / 32768.0;
		cur += inc;			// JDJDJDJD only the first (left ?) channel is studied ?
	    } else
		re[i] = 0.0;
	}
	fft (re, im, 11, 0);
	for (i=0 ; i<1024 ; i++)
	    re[i] = sqrt (re[i] * re[i] + im[i] * im[i]);

	for (i=0 ; i<8 ; i++)   // because 7 ~= 150Hz * (2048 / 44100Hz)
	    lo += re[i];

	for (i=8 ; i<93 ; i++)  // because 92 ~= 2000Hz * (2048 / 44100Hz)
	    me += re[i];

	for (i=93 ; i<512 ; i++)
	    hi += re[i];
    }
cerr << "lomehi for " << Sample::name << "    [" << lo << " , " << me << " , " << hi << "]" << endl ;
}


};

