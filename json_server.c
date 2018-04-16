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

void recursion_guard_increment(int *count)
{
 int tmp = *count;
 tmp++;
 *count = tmp;
}

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


void json_parse(json_object * jobj, int *recursion_guard_count) {
    recursion_guard_increment(recursion_guard_count);    
    if (*recursion_guard_count > MAX_RECURSION_COUNT) {
       printf("==== MAX RECURSION LIMIT HIT ==== \n");
       return;
    }
    enum json_type type;
    json_object_object_foreach(jobj, key, val) {
        type = json_object_get_type(val);
        switch (type) {
        case json_type_null:
        {
            //printf("type: json_type_null, \n");
            syslog(LOG_INFO,"type: json_type_null, \n");
            json_object * tmp_null;
            json_object_object_get_ex(jobj, key, &tmp_null);
            printf("key: %s\n",key);
            printf("value: %s\n",json_object_get_string(tmp_null));
            syslog(LOG_INFO, "key: %s\n", key);
            syslog(LOG_INFO, "value: %s\n", json_object_get_string(tmp_null));
            break;
        }
        case json_type_object:
        {
            printf("type: json_type_object, \n");
            syslog(LOG_INFO,"type: json_type_object, \n");
            json_object * tmp_object;
            json_object_object_get_ex(jobj, key, &tmp_object);
            json_parse(tmp_object, recursion_guard_count);
            break;
        }
        case json_type_array:
        {
            printf("type: json_type_array, \n");
            syslog(LOG_INFO,"type: json_type_array, \n");
            json_object * tmp_array;
            json_object_object_get_ex(jobj, key, &tmp_array);
            printf("key: %s\n",key);
            syslog(LOG_INFO, "key: %s\n", key);
            int arraylen = json_object_array_length(tmp_array);
            printf("Array Length: %d\n",arraylen);
            syslog(LOG_INFO, "Array Length: %d\n", arraylen);
            int i;
            json_object * jvalue;
            for (i=0; i< arraylen; i++) {
                jvalue = json_object_array_get_idx(tmp_array, i);
                printf("value[%d]: %s\n",i, json_object_get_string(jvalue));
                syslog(LOG_INFO, "value[%d] : %s\n",i, json_object_get_string(jvalue));
                enum json_type type0 = json_object_get_type(jvalue);
                switch(type0) {
                case json_type_object:
                case json_type_array:
                {
                    json_parse(jvalue, recursion_guard_count);
                    break;
                }
                default:
                {}
                }
            }
            break;
        }
        case json_type_string:
        {
            printf("type: json_type_string, \n");
            syslog(LOG_INFO,"type: json_type_string, \n");
            json_object * tmp_string;
            json_object_object_get_ex(jobj, key, &tmp_string);
            printf("key: %s\n",key);
            printf("value: %s\n",json_object_get_string(tmp_string));
            syslog(LOG_INFO, "key: %s\n", key);
            syslog(LOG_INFO, "value: %s\n", json_object_get_string(tmp_string));
            break;
        }
        case json_type_boolean:
        {
            printf("type: json_type_boolean, \n");
            syslog(LOG_INFO,"type: json_type_boolean, \n");
            json_object * tmp_boolean;
            json_object_object_get_ex(jobj, key, &tmp_boolean);
            printf("key: %s\n",key);
            printf("value: %s\n",json_object_get_boolean(tmp_boolean)? " true " : " false ");
            syslog(LOG_INFO, "key: %s\n", key);
            syslog(LOG_INFO, "value: %s\n", json_object_get_string(tmp_boolean)? " true " : " false ");
            break;
        }
        case json_type_int:
        {
            printf("type: json_type_int, \n");
            syslog(LOG_INFO,"type: json_type_int, \n");
            json_object * tmp_int;
            json_object_object_get_ex(jobj, key, &tmp_int);
            printf("key: %s\n",key);
            printf("value: %d\n",json_object_get_int(tmp_int));
            syslog(LOG_INFO, "key: %s\n", key);
            syslog(LOG_INFO, "value: %s\n", json_object_get_string(tmp_int));
            break;
        }
        case json_type_double:
        {
            printf("type: json_type_double, \n");
            syslog(LOG_INFO,"type: json_type_double, \n");
            json_object * tmp_double;
            json_object_object_get_ex(jobj, key, &tmp_double);
            printf("key: %s\n",key);
            printf("value: %f\n",json_object_get_double(tmp_double));
            syslog(LOG_INFO, "key: %s\n", key);
            syslog(LOG_INFO, "value: %s\n", json_object_get_string(tmp_double));
            break;
        }
        default:
        {
            printf("type: default, \n");
            syslog(LOG_INFO,"type: default, \n");
        }
        }
    }
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
    static char client_message[BUFFER_MAX_SIZE]= {0};

    //Receive a message from client
    while((read_size = recv(sock , client_message , BUFFER_MAX_SIZE , 0)) > 0 )
    {
        syslog(LOG_INFO, "### Data received ###\n");
        // ######################################################################################################################
        syslog(LOG_INFO, "READ: >>>> %s\n <<<< ", client_message);
        int recursion_guard_count = 0;
        json_object * jobj_parse = json_tokener_parse(client_message);
        json_parse(jobj_parse, &recursion_guard_count);

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
