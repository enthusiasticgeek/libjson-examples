//Copyright (c) 2018 Pratik M Tambe <enthusiasticgeek@gmail.com>
#include <stdio.h>
#include <string.h>    //strlen
#include <stdlib.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h>   //for threading , link with lpthread
#include <syslog.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <ctime>
#include <json/json.h>
#include "json_parse_test.h"

#define DAEMON_NAME "json_server"

void daemonShutdown();
void signal_handler(int sig);

const static int SERVER_PORT=8888;

#ifndef BUFFER_MAX_SIZE
#define BUFFER_MAX_SIZE 2048
#endif

#define _DAEMON_ 0

#define MAX_SIZE 1024
//Guard against infinite recursion
#define MAX_RECURSION_COUNT 50
//#define JSON_TOKENER_DEFAULT_DEPTH 32


int pidFilehandle;
//the thread function
void *connection_handler(void *);

void signal_handler(int sig)
{
    switch(sig)
    {
    case SIGHUP:
#if _DAEMON_
        syslog(LOG_WARNING, "Received SIGHUP signal.");
#else
        printf("Received SIGHUP signal.\n");
#endif
        daemonShutdown();
        break;
    case SIGINT:
    case SIGTERM:
#if _DAEMON_
        syslog(LOG_INFO, "Daemon exiting");
#else
        printf("Daemon exiting\n");
#endif
        daemonShutdown();
        exit(EXIT_SUCCESS);
        break;
    default:
#if _DAEMON_
        syslog(LOG_WARNING, "Unhandled signal %s", strsignal(sig));
#else
        printf("Unhandled signal %s\n", strsignal(sig));
#endif
        break;
    }
}


void daemonShutdown()
{
    close(pidFilehandle);
}

int main(int argc , char *argv[])
{  
    struct sigaction newSigAction;
    sigset_t newSigSet;

    /* Set signal mask - signals we want to block */
    sigemptyset(&newSigSet);
    sigaddset(&newSigSet, SIGCHLD);  /* ignore child - i.e. we don't need to wait for it */
    sigaddset(&newSigSet, SIGTSTP);  /* ignore Tty stop signals */
    sigaddset(&newSigSet, SIGTTOU);  /* ignore Tty background writes */
    sigaddset(&newSigSet, SIGTTIN);  /* ignore Tty background reads */
    sigaddset(&newSigSet, SIGPIPE);  /* ignore Tty pipe */
    sigprocmask(SIG_BLOCK, &newSigSet, NULL);   /* Block the above specified signals */

    /* Set up a signal handler */
    newSigAction.sa_handler = signal_handler;
    sigemptyset(&newSigAction.sa_mask);
    newSigAction.sa_flags = 0;

    /* Signals to handle */
    sigaction(SIGHUP, &newSigAction, NULL);     /* catch hangup signal */
    sigaction(SIGTERM, &newSigAction, NULL);    /* catch term signal */
    sigaction(SIGINT, &newSigAction, NULL);     /* catch interrupt signal */

#if _DAEMON_
    const char* pidfile ="/tmp/json_server.pid";
    const char* rundir ="/tmp";
    /* Debug logging
     setlogmask(LOG_UPTO(LOG_DEBUG));
     openlog(DAEMON_NAME, LOG_CONS, LOG_USER);
     */

    /* Logging */
    setlogmask(LOG_UPTO(LOG_INFO));
    openlog(DAEMON_NAME, LOG_CONS | LOG_PERROR, LOG_USER);

    syslog(LOG_INFO, "Daemon starting up");
    printf("Running as a daemon...\n");
    int pid, sid, i;
    char str[10];
    /* Check if parent process id is set */
    if (getppid() == 1)
    {
        /* PPID exists, therefore we are already a daemon */
        return -1;
    }

    /* Fork*/
    pid = fork();

    if (pid < 0)
    {
        /* Could not fork */
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        /* Child created ok, so exit parent process */
        printf("Child process created: %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    /* Child continues */

    //umask(027); /* Set file permissions 750 */
    umask(0);

    /* Get a new process group */
    sid = setsid();

    if (sid < 0)
    {
        exit(EXIT_FAILURE);
    }

    /* close all descriptors */
    for (i = getdtablesize(); i >= 0; --i)
    {
        close(i);
    }

    /* Route I/O connections */

    /* Open STDIN */
    i = open("/dev/null", O_RDWR);

    /* STDOUT */
    dup(i);

    /* STDERR */
    dup(i);

    chdir(rundir); /* change running directory */

    /* Ensure only one copy */
    pidFilehandle = open(pidfile, O_RDWR|O_CREAT, 0600);

    if (pidFilehandle == -1 )
    {
        /* Couldn't open lock file */
        syslog(LOG_INFO, "Could not open PID lock file %s, exiting", pidfile);
        exit(EXIT_FAILURE);
    }

    /* Try to lock file */
    if (lockf(pidFilehandle,F_TLOCK,0) == -1)
    {
        /* Couldn't get lock on lock file */
        syslog(LOG_INFO, "Could not lock PID lock file %s, exiting", pidfile);
        exit(EXIT_FAILURE);
    }


    /* Get and format PID */
    sprintf(str,"%d\n",getpid());

    /* write pid to lockfile */
    write(pidFilehandle, str, strlen(str));
#endif


    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SERVER_PORT);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);


    //Accept and incoming connection
    //puts("Waiting for incoming connections...");
    //c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
        syslog(LOG_INFO,"connection accepted\n");

        pthread_t sniffer_thread;
        new_sock = (int*)malloc(1);
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            syslog(LOG_INFO,"could not create thread\n");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        syslog(LOG_INFO,"accept failed\n");
        return 1;
    }

    return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    static unsigned char client_message[BUFFER_MAX_SIZE]= {0};

    //Receive a message from client
    while((read_size = recv(sock , client_message , BUFFER_MAX_SIZE , 0)) > 0 )
    {
        syslog(LOG_INFO, "### Data received ###\n");
        // ######################################################################################################################
        syslog(LOG_INFO, "READ: >>>> %s\n <<<< ", client_message);
        int recursion_guard_count = 0;
        json_object * jobj_parse = json_tokener_parse((char*)client_message);
        json_parse(jobj_parse, &recursion_guard_count);
        json_object_put(jobj_parse);

    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
}
