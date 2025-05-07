#include "nscom-conn.h"
#include "nscom-epoll.h"
#include "nscom-utils.h"
#include "nscom.h"

int
nscom_conn_construct(int fd, int (*handler)(void *ctx), void *context)
{
    int ret = NSCOM_EOK;
    struct nscom_conn *new_conn = malloc(sizeof(struct nscom_conn));
    struct nscom *nscom = (struct nscom *) context;

    if (new_conn == NULL) {
        printf("nscom_conn_construct: malloc error, errno %d\n", errno);
        ret = -NSCOM_ESYSCALL;
        goto out;
    }

    new_conn->fd = fd;
    new_conn->context = context;
    new_conn->handler = handler;
    list_add_tail(&(new_conn->conn_node), &(nscom->conn_list));

    ret = nscom_epoll_ops(nscom->epollfd, EPOLL_CTL_ADD, fd, new_conn);
    if (ret < 0) {
        printf("add new connection to epoll failed, errno %d\n", ret);
        goto out;
    }

out:
    return ret;
}

int
nscom_conn_destruct(int fd, int epollfd, struct list_head *list)
{
    int ret = 0;
    struct nscom_conn *conn = NULL;
    struct nscom_conn *next = NULL;

    list_for_each_entry_safe(conn, next, list, conn_node) {
        if (conn->fd == fd) {
            ret = nscom_epoll_ops(epollfd, EPOLL_CTL_DEL, fd, conn);
            if (ret < 0) {
                printf("del connection from epoll failed %d\n", ret);
                ret = -1;
                goto out;
            }
            list_del(&conn->conn_node);
            break;
        }
    }

out:
    return ret;
}



