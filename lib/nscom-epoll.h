#ifndef NSCOM_EPOLL_H
#define NSCOM_EPOLL_H

#include <sys/epoll.h>

/* epoll wait timeout 50ms */
#define NSCOM_EPOLL_TIMEOUT (50)

int nscom_epoll_init(void);
void nscom_epoll_uninit(int epollfd);
int nscom_epoll_ops(int epollfd, int ops, int fd, void *nscom_conn);
int nscom_epoll_process(int epollfd, struct epoll_event *events, int maxevents,
                          int timeout);

#endif /* NSCOM_EPOLL_H */