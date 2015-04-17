#ifndef __STAMINA_H__
#define __STAMINA_H__

#include "udis86.h"

#define debug(fmt...)				\
	pr_info("[" KBUILD_MODNAME "] " fmt)

#define to_ptr(x)				\
	(void *)((unsigned long)(x))

typedef struct {
	unsigned long magic;
	unsigned long nr_syscall;
	unsigned long stack[0];
} stamina_t;

typedef struct {

	unsigned long usage;
	unsigned long stack_min;
	unsigned long stack_max;

} scusage_t;

int stamina_init(void);
void stamina_cleanup(void);

#endif /* __STAMINA_H__ */
