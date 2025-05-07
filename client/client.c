#include "nscom.h"
#include "nscom-unix-sock.h"
#include "nscom-conn.h"
#include "nscom-utils.h"

#include <stdio.h>

int
main(int argc, char *argv[])
{
    int ret = NSCOM_EOK;
    struct nscom *nscom = NULL;
    struct nscom_conn *srv_conn = NULL;
    char msg[NSCOM_UNIX_SOCK_MSG_BUF_LEN] = {0};

    ret = nscom_initialize();
    if (ret < 0) {
        printf("nscom initialize failed, errno %d\n", ret);
        goto err;
    }

    ret = nscom_unix_sock_register();
    if (ret < 0) {
        printf("nscom register failed, errno %d", ret);
        goto err;
    }

    ret =  nscom_open("unix-sock-client", NSCOM_CONN_TYPE_CLIENT, "/tmp/nscom/unix_server.sock", "unix_sock", &nscom);
    if (ret < 0) {
        printf("nscom open failed, errno %d\n", ret);
        goto err;
    }

    for (;;) {
        scanf("%s", msg);
        printf("input: %s\n", msg);
        if (!strcmp(msg, "exit")) {
            printf("nscom exit\n");
            break;
        }

        list_for_each_entry (srv_conn, &nscom->conn_list, conn_node) {
            if (srv_conn->fd > 0) {
                break;
            }
        }
        ret = nscom->class->send(srv_conn->fd, nscom, (const void *)msg, sizeof(msg));
        printf("send %d bytes, msg is %s\n", ret, msg);
    }

    ret = nscom_close(nscom);
    if (ret < 0) {
        printf("nscom close failed, errno %d\n", ret);
        goto err;
    }

err:
    return ret;
}