#include "nscom-unix-sock.h"
#include "nscom.h"
#include "nscom-conn.h"
#include "nscom-utils.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

struct nscom_unix_sock {
    struct nscom up;
};

static int
nscom_unix_sock_receive_conn(void *context)
{
    int ret = 0;
    struct nscom_conn *conn = (struct nscom_conn *) context;
    struct nscom *nscom = (struct nscom *) (conn->context);
    struct nscom_unix_sock *unix_sock = container_of(nscom, struct nscom_unix_sock, up);;
    char buf[NSCOM_UNIX_SOCK_MSG_BUF_LEN] = {0};

    ret = unix_sock->up.class->recv(conn->fd, &(unix_sock->up), buf);
    if (ret < 0) {
        printf("recv error, errno %d\n", errno);
        goto out;
    }
    if (ret == 0) {
        ret = nscom_conn_destruct(conn->fd, nscom->epollfd, &(conn->conn_node));
        close(conn->fd);
        printf("connection close by peer\n");
        goto out;
    }

    ret = unix_sock->up.class->msg_handler(&(unix_sock->up), buf);
    if (ret < 0) {
        printf("process message error\n");
        goto out;
    }

out:
    return ret;
}

static int
nscom_unix_sock_accept_conn(void *context)
{
    int ret = 0;
    struct nscom_conn *listen_conn = (struct nscom_conn *) context;
    struct nscom *nscom = (struct nscom *) (listen_conn->context);
    struct nscom_unix_sock *unix_sock = container_of(nscom, struct nscom_unix_sock, up);
    struct sockaddr_un sockaddr;
    socklen_t sockaddr_len = 0;
    int connfd = 0;

    memset(&sockaddr, 0, sizeof(struct sockaddr_un));
    connfd = accept(listen_conn->fd, (struct sockaddr *) &sockaddr, &sockaddr_len);
    if (connfd < 0) {
        printf("accept error, connfd is %d\n", connfd);
        ret = -NSCOM_ESYSCALL;
        goto err;
    }
    printf("accepted client, connection fd %d\n", connfd);
    ret = fcntl(connfd, F_SETFL, O_NONBLOCK);
    if (ret < 0) {
        printf("set fd non-blocking failed, errno %d\n", errno);
        goto err;
    }
    ret = nscom_conn_construct(connfd, nscom_unix_sock_receive_conn, (void *) &(unix_sock->up));
    if (ret < 0) {
        printf("nscom_conn_construct error\n");
        goto err;
    }
    
err:
    return ret;
}

static int
nscom_unix_sock_init(void)
{
    return 0;
}

static int
nscom_unix_sock_run(struct nscom *nscom_)
{
    return 0;
}

static struct nscom *
nscom_unix_sock_alloc(void)
{
    struct nscom_unix_sock *unix_sock = NULL;

    unix_sock = malloc(sizeof(struct nscom_unix_sock));
    if (unix_sock == NULL) {
        printf("nscom_unix_sock_alloc: malloc error\n");
        return NULL;
    }

    return &(unix_sock->up);
}

static int
nscom_unix_sock_dealloc(struct nscom *nscom_)
{
    struct nscom_unix_sock *unix_sock = container_of(nscom_, struct nscom_unix_sock, up);

    free(unix_sock);
}

static int
nscom_unix_sock_construct(struct nscom *nscom_)
{
    int ret = 0;
    int sockfd = 0;
    struct sockaddr_un sockaddr;
    struct nscom_unix_sock *unix_sock = container_of(nscom_, struct nscom_unix_sock, up);

    unix_sock->up.sock_family = AF_UNIX;
    unix_sock->up.sock_type = SOCK_STREAM;

    sockfd = socket(unix_sock->up.sock_family, unix_sock->up.sock_type, unix_sock->up.sock_protocol);
    
    memset(&sockaddr, 0, sizeof(struct sockaddr_un));
    sockaddr.sun_family = AF_UNIX;
    strncpy(sockaddr.sun_path, nscom_->path, sizeof(sockaddr.sun_path) - 1);

    ret = fcntl(sockfd, F_SETFL, O_NONBLOCK);
    if (ret < 0) {
        printf("set fd non-blocking failed, errno %d\n", errno);
        goto err;
    }

    if (unix_sock->up.conn_type == NSCOM_CONN_TYPE_SERVER) {
        (void)unlink(nscom_->path);
    
        ret = bind(sockfd, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr_un));
        if (ret < 0) {
            printf("listening socket bind error, errno %d\n", errno);
            goto err;
        }

        ret = listen(sockfd, NSCOM_LISTEN_BACKLOG);
        if (ret < 0) {
            printf("listening socket error, errno %d\n", errno);
            goto err;
        }

        ret = nscom_conn_construct(sockfd, nscom_unix_sock_accept_conn, (void *) &(unix_sock->up));
        if (ret < 0) {
            printf("nscom_conn_construct error\n");
            goto err;
        }
    } else if (unix_sock->up.conn_type == NSCOM_CONN_TYPE_CLIENT) {
        ret =  connect(sockfd, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr_un));
        if (ret < 0) {
            printf("connecting to server error, errno %d\n", errno);
            goto err;
        }
        printf("connected to server, fd %d\n", sockfd);

        ret = nscom_conn_construct(sockfd, nscom_unix_sock_receive_conn, (void *) &(unix_sock->up));
        if (ret < 0) {
            printf("nscom_conn_construct error\n");
            goto err;
        }
    } else {
        printf("connection type error, type is %d\n", unix_sock->up.conn_type);
        ret = -NSCOM_ENOEXIST;
        goto err;
    }

    return ret;

err:
    close(sockfd);
    return ret;
}

static int
nscom_unix_sock_destruct(struct nscom *nscom_)
{
    int ret = NSCOM_EOK;

    return ret;
}

static int
nscom_unix_sock_recv(int fd, struct nscom *nscom_, void *msg)
{
    int ret = 0;

    ret = read(fd, msg, NSCOM_UNIX_SOCK_MSG_BUF_LEN);
    
    return ret;
}

static int
nscom_unix_sock_send(int fd, struct nscom *nscom_, const void *msg, size_t count)
{
    int ret = 0;

    ret = write(fd, msg, count);

    return ret;
}

static int
nscom_unix_sock_msg_handler(struct nscom *nscom_, void *msg)
{
    printf("message process is: %s\n", (char *)msg);
    return 0;
}

static struct nscom_class nscom_unix_sock = {
    .type = "unix_sock",
    .init = nscom_unix_sock_init,
    .run = nscom_unix_sock_run,
    .alloc = nscom_unix_sock_alloc,
    .dealloc = nscom_unix_sock_dealloc,
    .construct = nscom_unix_sock_construct,
    .destruct = nscom_unix_sock_destruct,
    .recv = nscom_unix_sock_recv,
    .send = nscom_unix_sock_send,
    .msg_handler = nscom_unix_sock_msg_handler,
};

int
nscom_unix_sock_register(void)
{
    return nscom_class_register(&nscom_unix_sock);
}