#ifndef __STAMINA_H__
#define __STAMINA_H__

#include "udis86.h"

#define debug(fmt...)				\
	pr_info("[" KBUILD_MODNAME "] " fmt)

#define to_ptr(x)				\
	(void *)((unsigned long)(x))

#endif /* __STAMINA_H__ */
