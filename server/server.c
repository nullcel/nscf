#include "nscom.h"
#include "nscom-unix-sock.h"
#include "nscom-conn.h"
#include "nscom-utils.h"
#include "nscom-list.h"

#include <stdio.h>

int
main(int argc, char *argv[])
{
    int ret = NSCOM_EOK;
    struct nscom *nscom = NULL;
    struct nscom_conn *srv_conn = NULL;
    struct nscom_conn *conn = NULL;
    char msg[NSCOM_UNIX_SOCK_MSG_BUF_LEN] = {0};

    ret = nscom_initialize();
    if (ret < 0) {
        printf("nscom initialize failed, errno %d\n", ret);
        goto err;
    }

    ret = nscom_unix_sock_register();
    if (ret < 0) {
        printf("nscom unix sock register failed, errno %d", ret);
        goto err;
    }

    ret =  nscom_open("unix-sock-server", NSCOM_CONN_TYPE_SERVER, "/tmp/nscom/unix_server.sock", "unix_sock", &nscom);
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

        srv_conn = list_first_entry(&nscom->conn_list, struct nscom_conn, conn_node);
        list_for_each_entry (conn, &nscom->conn_list, conn_node) {
            if ((srv_conn != conn) && (srv_conn->fd > 0)) {
                break;
            }
        }
        ret = nscom->class->send(conn->fd, nscom, (const void *)msg, sizeof(msg));
        printf("send %d bytes\n", ret);
    }

    ret = nscom_close(nscom);
    if (ret < 0) {
        printf("nscom close failed, errno %d\n", ret);
        goto err;
    }

err:
    return ret;
}