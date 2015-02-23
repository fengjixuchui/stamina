#include <linux/sched.h>
#include <linux/magic.h>

#include <asm/stacktrace.h>

#include "stamina.h"

void ServiceTraceEnter(struct pt_regs *regs)
{
	unsigned long *s;
	unsigned long *n;
	stamina_t *stamp = (stamina_t *)(end_of_stack(current));
	unsigned long flags;

	get_bp(s);

	local_irq_save(flags);
	for (n = stamp->stack; n != s; n++) {
		*n = STACK_END_MAGIC;
	}
	local_irq_restore(flags);

	stamp->nr_syscall = regs->ax;
}

void ServiceTraceLeave(struct pt_regs *regs)
{
	unsigned long *s;
	unsigned long *n;
	stamina_t *stamp = (stamina_t *)(end_of_stack(current));

	get_bp(s);

	for (n = stamp->stack; n != s; n++) {
		if (*n != STACK_END_MAGIC)
			break;
	}

	debug("nr_syscall:%04lx used %04lu / %04lu (%s)\n", stamp->nr_syscall, \
		(THREAD_SIZE / sizeof(unsigned long)) - (n - end_of_stack(current)), (THREAD_SIZE / sizeof(unsigned long)), current->comm);
}
