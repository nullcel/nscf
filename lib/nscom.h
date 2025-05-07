#ifndef NSCOM_H
#define NSCOM_H

#include "nscom-list.h"
#include <sys/epoll.h>
#include <pthread.h>

#define MAX_NSCOM_NAME (32)
#define MAX_NSCOM_PATH (32)
#define MAX_EPOLL_EVENTS (1024)
#define NSCOM_LISTEN_BACKLOG (1)

enum nscom_conn_type {
    NSCOM_CONN_TYPE_NONE,
    NSCOM_CONN_TYPE_CLIENT,
    NSCOM_CONN_TYPE_SERVER,
    NSCOM_CONN_TYPE_MAX
};

struct nscom {
    char name[MAX_NSCOM_NAME];
    int conn_type; /* conn type: client/server */
    char path[MAX_NSCOM_PATH]; /* path: unix path is /tmp/tapx.sock, tcp path is 192.168.100.100:5566 */
    int epollfd;
    pthread_t thread_id;
    struct epoll_event events[MAX_EPOLL_EVENTS];
    int sock_family;
    int sock_type; /* socket type, SOCK_STREAM/SOCK_DGRAM... */
    int sock_protocol;
    struct nscom_class *class;
    struct list_head conn_list;
};

struct nscom_class {
    char *type; /* nscom type: unix_sock/tcp_sock/tcp_sock_ssl... */
    int (*init)(void);
    int (*run)(struct nscom *nscom_);
    struct nscom * (*alloc)(void);
    int (*dealloc)(struct nscom *nscom_);
    int (*construct)(struct nscom *nscom_);
    int (*destruct)(struct nscom *nscom_);
    int (*recv)(int fd, struct nscom *nscom_, void *msg);
    int (*send)(int fd, struct nscom *nscom_, const void *msg, size_t count);
    int (*msg_handler)(struct nscom *nscom_, void *msg);
};

int nscom_initialize(void);
int nscom_open(char *name, int conn_type, char *path, char *class_type, struct nscom **nscomp);
int nscom_close(struct nscom *nscom_);
int nscom_class_register(struct nscom_class *new_class);

#endif /* NSCOM_H */