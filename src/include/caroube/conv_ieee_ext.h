
#ifndef CONVIEEEEXT_H_INC
#define CONVIEEEEXT_H_INC


#ifdef __cplusplus
extern "C" {
#endif

/* Interface Functions */
void    ConvertToIeeeExtended (double num, char *bytes);
double  ConvertFromIeeeExtended (unsigned char *bytes);

#ifdef __cplusplus
}   /* closing the 'extern "C" {' above... */
#endif

#endif // CONVIEEEEXT_H_INC

/*
 * $Log: conv_ieee_ext.h,v $
 * Revision 1.1  2003/09/24 22:53:44  jd
 * Initial revision
 *
 * Revision 1.1.1.1  2003/09/11 23:52:11  jd
 * first importation
 *
 * Revision 1.2  2003/06/06 14:03:19  jd
 * fine cleanup for aesthetic
 *
 *
 */
