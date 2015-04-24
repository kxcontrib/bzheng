/*
    gcc -I. -shared -fPIC -o proc.so proc.c c.o -DKXVER=3
*/
#define _GNU_SOURCE
#include "k.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <string.h>

void zombie_handler(int iSignal)
{
    signal(SIGCHLD,zombie_handler); //reset handler to catch SIGCHLD for next time;
    int status;
    pid_t w;
    char* msg;
    do {
        w = waitpid(0, &status, WUNTRACED | WCONTINUED | WNOHANG);
        if (w == -1) {
            return;
        }
        if (w == 0) {
            return;
        }
        if (WIFEXITED(status)) {
            msg="exited";
            status=WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            msg="killed by signal";
            status=WTERMSIG(status);
        } else if (WIFSTOPPED(status)) {
            msg="stopped by signal";
            status=WSTOPSIG(status);
        } else if (WIFCONTINUED(status)) {
        }
        r0(k(0,".proc.term",ki(w),ki(status),(K)0));
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    r0(k(0,".proc.end",ki(w),ks(msg),ki(status),(K)0));
    return;
}

K1(k_clean)
{
    zombie_handler(SIGCHLD);
    R (K)0;
}

K2(k_child)
{
    P((KC!=x->t)|(KC!=y->t),krr("type: [C;C]"))
    char *xstr,*ystr;
    pid_t pid, sid;
    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        R krr("fork");
    }
    /* return child PID to the parent process. */
    if (pid > 0) {
        signal(SIGCHLD,zombie_handler);
        xstr=calloc(x->n+1,sizeof(char));
        memcpy(xstr,kC(x),x->n);
        r0(k(0,xstr,(K)0));
        free(xstr);
        R ki((I)pid);
    } else {
        /* Change the current working directory.  This prevents the current
           directory from being locked; hence not being able to remove it. */
        if ((chdir("/tmp")) < 0) {
            R krr("chdir");
        }
        ystr=calloc(y->n+1,sizeof(char));
        memcpy(ystr,kC(y),y->n);
        r0(k(0,ystr,(K)0));
        free(ystr);
        /* Redirect standard files to /dev/null */
        freopen( "/dev/null", "r", stdin);
        freopen( "/dev/null", "w", stdout);
        freopen( "/dev/null", "w", stderr);
        R ki(0);
    }
}

K1(k_fork)
{
    pid_t pid;
    pid = fork();
    if (pid < 0)
        R krr("fork");
    R ki((I)pid);
}

K1(k_setsid)
{
    pid_t sid;
    sid = setsid();
    if (sid < 0)
        R krr("setsid");
    R ki((I)sid);
}

K1(k_getppid)
{
    pid_t ppid;
    ppid = getppid();
    if (ppid<0)
        R krr("getppid");
    R ki((I)ppid);
}

#define Q_ENTRY(i, name, def) kS(x)[i] = ss(name); kK(y)[i] = def
#define Q_FUNC(i, name, nargs) Q_ENTRY(i, substr(#name), dl(name, nargs))
#define Q_ENUM(i, name) Q_ENTRY(i, #name, ki(name))
ZK null() {K x=ka(101);xi=0;R x;}
S substr(S str){R (S)str+2;}

K1(proc)
{
    K y = ktn(0,6); x = ktn(KS,6);
    Q_ENTRY(0, "", null());
    Q_FUNC(1, k_fork, 1);
    Q_FUNC(2, k_setsid, 1);
    Q_FUNC(3, k_getppid, 1);
    Q_FUNC(4, k_child, 2);
    Q_FUNC(5, k_clean, 1);
    R xD(x,y);
}
