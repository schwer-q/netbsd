/*
 *	$Id: semctl.c,v 1.1 1993/11/14 12:40:32 cgd Exp $
 */     

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#if __STDC__
int semctl(int semid, int semnum, int cmd, union semun semun)
#else
int semctl(semid, int semnum, cmd, semun)
	int semid, semnum;
	int cmd;
	union semun semun;
#endif
{
	return (semsys(0, semid, semnum, cmd, &semun));
}
