#ifndef VERSION_H
#define VERSION_H

#define VER_COMPANY   "Twainet"
#define VER_COPYRIGHT "Copyright © Twainet. 2010-2015. All rights reserved."
#define VER_PRODUCT   "Twainet"

#define VER_NUMBER_MAJOR   1
#define VER_NUMBER_MINOR   1
#define VER_NUMBER_RELEASE 0

#ifndef VER_NUMBER_BUILD
#define VER_NUMBER_BUILD   0
#endif

#define _VERSTR(m)  #m
#define VERSTR(m)   _VERSTR(m)
#define _VERSTR4(a, b, c, d) a.b.c.d
#define VERSTR4(a, b, c, d)  _VERSTR4(a, b, c, d)
#define _VER_STRVER VERSTR4(VER_NUMBER_MAJOR,VER_NUMBER_MINOR,VER_NUMBER_RELEASE,VER_NUMBER_BUILD)

#define VER_NUMBER_LIST   VER_NUMBER_MAJOR, VER_NUMBER_MINOR, VER_NUMBER_RELEASE, VER_NUMBER_BUILD
#define VER_NUMBER_STRING VERSTR(_VER_STRVER)


#endif/*VERSION_H*/
