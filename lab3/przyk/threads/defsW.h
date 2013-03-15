/*********************************************************************
 * P.Bolek         maj 1995
 * Program PATIA
 * ~~~~~~~~~~~~~
 * Rownolegle, asynchroniczne rozwiazanie ukladu rownan liniowych
 * Watki (Solaris threads)
 *
 * defs.h
 *
 *
 *********************************************************************/
#define DEBUG
#define RESULTS
#define MAXSIZE	1000
#define XSIZE	sizeof(double)*MAXSIZE
#define ASIZE   XSIZE*MAXSIZE

void freemem();
void getmem();
void readA();
void readb();
void init();
void *work();
