#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#ifdef SOLARIS /* needed with at least Solaris 8 */
#include <siginfo.h>
#endif

void printStack(const char *where) {
	printf("%s stack: %p\n",where,&where);
}

void signalHandler(int cause, siginfo_t *HowCome, void *ucontext) {
	printStack("signal handler");
}

int main(){ 
	int i;
	struct itimerval itimer;
	
/* Install our SIGPROF signal handler */
        struct sigaction sa;
	
        sa.sa_sigaction = signalHandler;
        sigemptyset( &sa.sa_mask );
        sa.sa_flags = SA_SIGINFO; /* we want a siginfo_t */
        if (sigaction (SIGPROF, &sa, 0)) {
		perror("sigaction");
		exit(1);
        }
	
	printStack("main routine");

/* Request SIGPROF */
	itimer.it_interval.tv_sec=3;
	itimer.it_interval.tv_usec=10*1000;
	itimer.it_value.tv_sec=0;
	itimer.it_value.tv_usec=10*1000;
	setitimer(ITIMER_PROF, &itimer, NULL); 
	
/* Just wait a bit, which should get a few SIGPROFs */
        while(1)
        {
	}

        return(0);
}
