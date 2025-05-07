#ifndef NSCOM_UTILS_H
#define NSCOM_UTILS_H

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

enum nscom_errno {
    NSCOM_EOK, /* normal return */
    NSCOM_ENOEXIST, /* something not exist */
    NSCOM_EEXIST, /* something exist */
    NSCOM_ESYSCALL, /* syscall error */
    NSCOM_ECALL, /* call error */
};

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const typeof(((type *)0)->member) * __mptr = (ptr);	\
	(type *)((char *)__mptr - offsetof(type, member)); })
#endif

int nscom_pthread_create(pthread_t *thread_id, const char *thread_name, void *(*start_func) (void *), void *arg);
int nscom_pthread_join(pthread_t thread_id, void **retval);

#endif /* NSCOM_UTILS_H */