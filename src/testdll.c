/*
 *  $Id$
 */

/* this function exists only in libraries that are dll, it is used to check for dll-ness at configure */
#if defined WIN32 && defined DLL_EXPORT
#warning yep we have this symbol defined

#ifdef __cplusplus
extern "C"
#endif

char libcaroube_is_dll (void)
{
  return 1;
}

#endif /* WIN32 && DLL_EXPORT */

