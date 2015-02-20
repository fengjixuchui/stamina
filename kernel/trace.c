#include <linux/sched.h>
#include <linux/magic.h>

#include <asm/stacktrace.h>

#include "stamina.h"

void ServiceTraceEnter(struct pt_regs *regs)
{
	unsigned long *s;
	unsigned long *n;

	get_bp(s);

	for (n = end_of_stack(current); n != s; n++) {
		*n = STACK_END_MAGIC;
	}
}

void ServiceTraceLeave(struct pt_regs *regs)
{
	unsigned long *s;
	unsigned long *n;

	get_bp(s);

	for (n = end_of_stack(current); n != s; n++) {
		if (*n != STACK_END_MAGIC)
			break;
	}

	debug("Task \"%s\" stack usage %lu / %lu\n", current->comm, \
	      (THREAD_SIZE / sizeof(unsigned long)) - (n - end_of_stack(current)), (THREAD_SIZE / sizeof(unsigned long)));
}
