#ifndef NSCOM_CONN_H
#define NSCOM_CONN_H

#include "nscom-list.h"

#include <linux/types.h>

struct nscom_conn {
    int fd;
    void *context;
    int (*handler)(void *ctx);
    struct list_head conn_node;
};

int nscom_conn_construct(int fd, int (*handler)(void *ctx), void *context);
int nscom_conn_destruct(int fd, int epollfd, struct list_head *list);

#endif /* NSCOM_CONN_H */