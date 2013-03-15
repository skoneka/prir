/*********************************************************************
 * P.Bolek         maj 1995
 * Program PATIA
 * ~~~~~~~~~~~~~
 * Rownolegle, asynchroniczne rozwiazanie ukladu rownan liniowych
 * SYSV IPC (semafory i pamiec dzielona)
 *
 * sem.c -- operacje semaforowe
 *
 * $Id: sem.c,v 1.3 1995/05/25 15:26:24 ela16pbl Exp $
 *********************************************************************/
#include <sys/ipc.h>
#include <sys/sem.h>

void
semcall(int sid, int op, int nr)
{
	struct sembuf buf;

	buf.sem_num=nr;
	buf.sem_op=op;
	buf.sem_flg=0;
	semop(sid, &buf, 1);
}
