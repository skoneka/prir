/*********************************************************************
 * P.Bolek         maj 1995
 * Program PATIA
 * ~~~~~~~~~~~~~
 * Rownolegle, asynchroniczne rozwiazanie ukladu rownan liniowych
 * SYSV IPC (semafory i pamiec dzielona)
 *
 * defs.h
 *
 * $Id: defs.h,v 1.4 1995/05/26 06:31:42 ela16pbl Exp $
 *********************************************************************/
#define MAXSIZE	300
#define XSIZE	sizeof(double)*MAXSIZE
#define ASIZE   XSIZE*MAXSIZE

#define	XKEY	1001
#define	AKEY	1002
#define	BKEY	1003
#define SKEY	1004
#define LKEY	1005

#define SEMKEY	9999

#define PERM 0600	/* prawa dostepu do pamieci dzielonej i semaforow	*/

#define READ_WRITE	0	/* semafor do synch. zapisu i odczytu wektora x	*/
#define	STOP_TEST	1	/* semafor do synch. zapisu i odczytu
						   znacznikow testu stopu	*/

void semcall(int, int, int);
void freemem();
void getmem();
void readA();
void readb();
void work();
