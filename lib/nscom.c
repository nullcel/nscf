#include "nscom.h"
#include "nscom-utils.h"
#include "nscom-list.h"
#include "nscom-epoll.h"

struct nscom_registered_class {
    struct list_head class_node;
    struct nscom_class *class;
};

static struct list_head registered_classes;
static bool nscom_exit = false;

static struct nscom_registered_class *
nscom_lookup_class(char *type)
{
    struct nscom_registered_class *rc = NULL;

    list_for_each_entry (rc, &registered_classes, class_node) {
        if (!strcmp(rc->class->type, type)) {
            return rc;
        }
    }

    return NULL;
}

int
nscom_class_register(struct nscom_class *new_class)
{
    int ret = NSCOM_EOK;

    if (nscom_lookup_class(new_class->type)) {
        printf("duplicate register nscom class, %s\n", new_class->type);
        ret = -NSCOM_EEXIST;
        goto out;
    }

    ret = new_class->init() ? new_class->init() : NSCOM_EOK;
    if (ret) {
        printf("new class init error, errno %d\n", ret);
        goto out;
    }

    struct nscom_registered_class *rc;
    rc = malloc(sizeof(*rc));
    if (!rc) {
        printf("malloc error\n");
        ret = -NSCOM_ESYSCALL;
        goto out;
    }
    list_add_tail(&(rc->class_node), &registered_classes);
    rc->class = new_class;

out:
    return ret;
}

static void *
nscom_loop(void *arg)
{
    struct nscom *nscom_ = (struct nscom *) arg;

    while (!nscom_exit) {
        nscom_epoll_process(nscom_->epollfd, nscom_->events, MAX_EPOLL_EVENTS, NSCOM_EPOLL_TIMEOUT);
    }
    return NULL;
}

int
nscom_initialize(void)
{
    int ret = NSCOM_EOK;

    INIT_LIST_HEAD(&registered_classes);

    return ret;
}

int
nscom_open(char *name, int conn_type, char *path, char *class_type, struct nscom **nscomp)
{
    int ret = NSCOM_EOK;
    struct nscom_registered_class *rc = NULL;
    struct nscom *nscom_ = NULL;

    rc = nscom_lookup_class(class_type);
    if (!rc) {
        printf("unknown sock class type %s\n", class_type);
        ret = -NSCOM_ENOEXIST;
        goto err1;
    }
    
    nscom_ = rc->class->alloc();
    if (!nscom_) {
        printf("nscom_ alloc error\n");
        ret = -NSCOM_ESYSCALL;
        goto err1;
    }

    memset(nscom_, 0, sizeof(struct nscom));
    snprintf(nscom_->name, sizeof(nscom_->name), "%s", name);
    nscom_->conn_type = conn_type;
    snprintf(nscom_->path, sizeof(nscom_->path), "%s", path);
    nscom_->epollfd = nscom_epoll_init();
    nscom_->class = rc->class;
    INIT_LIST_HEAD(&(nscom_->conn_list));

    ret = rc->class->construct(nscom_);
    if (ret) {
        printf("nscom construct error, name %s", name);
        ret = -NSCOM_ECALL;
        goto err2;
    }

    ret = nscom_pthread_create(&(nscom_->thread_id), "nscom_events_thread", nscom_loop, (void *)nscom_);
    if (ret < 0) {
        printf("nscom_pthread_create failed, errno %d\n", errno);
        goto err2;
    }

    *nscomp = nscom_;

    return ret;

err2:
    rc->class->dealloc(nscom_);
err1:
    return ret;
}

int
nscom_close(struct nscom *nscom_)
{
    int ret = NSCOM_EOK;

    nscom_exit = true;

    ret = nscom_pthread_join(nscom_->thread_id, NULL);
    if (ret < 0) {
        printf("pthread join failed, errno %d\n", errno);
    }

    ret = nscom_->class->destruct(nscom_);
    if (ret < 0) {
        printf("nscom destruct failed, errno %d\n", errno);
    }

    ret = nscom_->class->dealloc(nscom_);
    if (ret < 0) {
        printf("nscom dealloc failed, errno %d\n", errno);
    }

    return ret;
}