
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

