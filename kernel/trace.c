#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/magic.h>
#include <linux/compat.h>
#include <linux/miscdevice.h>

#include <asm/stacktrace.h>
#include <asm/asm-offsets.h>

#ifdef CONFIG_X86_32
# define IS_IA32		1
#elif defined(CONFIG_IA32_EMULATION)
# define IS_IA32		is_compat_task()
#else
# define IS_IA32		0
#endif

#include "stamina.h"

scusage_t *table = NULL;

void ServiceTraceEnter(struct pt_regs *regs)
{
	unsigned long *s;
	unsigned long *n;
	stamina_t *stamp = (stamina_t *)(end_of_stack(current));
	unsigned long flags;

	if (IS_IA32) return;

	get_bp(s);

	local_irq_save(flags);
	for (n = stamp->stack; n != s; n++) {
		*n = STACK_END_MAGIC;
	}
	local_irq_restore(flags);

	stamp->nr_syscall = regs->orig_ax;
}

void ServiceTraceLeave(struct pt_regs *regs)
{
	int cpu = smp_processor_id();
	unsigned long *s;
	unsigned long *n;
	stamina_t *stamp = (stamina_t *)(end_of_stack(current));
	unsigned long flags;

	if (IS_IA32) return;

	get_bp(s);

	local_irq_save(flags);
	for (n = stamp->stack; n != s; n++) {
		if (*n != STACK_END_MAGIC)
			break;
	}
	local_irq_restore(flags);

	if (stamp->nr_syscall > __NR_syscall_max) { // wtf?
		printk("nr_syscall = %16lx\n", stamp->nr_syscall);
		return;
	}

	if (cpu == 0) { // FIXME: use percpu tables
		unsigned long stack_usage = (THREAD_SIZE / sizeof(unsigned long)) - (n - end_of_stack(current));

		if (!table[stamp->nr_syscall].stack_min || (table[stamp->nr_syscall].stack_min > stack_usage))
			table[stamp->nr_syscall].stack_min = stack_usage;
		if (!table[stamp->nr_syscall].stack_max || (table[stamp->nr_syscall].stack_max < stack_usage))
			table[stamp->nr_syscall].stack_max = stack_usage;

		table[stamp->nr_syscall].usage++;
	}

}

static int stamdev_open(struct inode *inode, struct file *file)
{
	printk("%s\n", __func__);
	return 0;
}

#define TABLE_SIZE	(__NR_syscall_max * sizeof(scusage_t))

static ssize_t stamdev_read(struct file *file, char __user *ptr, size_t len, loff_t *ppos)
{
	size_t nbytes;

	if (*ppos > TABLE_SIZE)
		return -EINVAL;

	nbytes = min_t(size_t, len, TABLE_SIZE - *ppos);
	copy_to_user(ptr, table + *ppos, nbytes);

	*ppos += nbytes;

	return nbytes;
}

static struct file_operations stamdev_fops = {
	.owner = THIS_MODULE,
	.open = stamdev_open,
	.read = stamdev_read,
};

static struct miscdevice stamdev = {
	.name = "stamina",
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &stamdev_fops,
	.mode = 0444,
};

int stamina_init(void)
{
	misc_register(&stamdev);

	table = (scusage_t *)kzalloc(__NR_syscall_max * sizeof(scusage_t), GFP_KERNEL);
	if (!table)
		return -ENOMEM;

	return 0;
}

void stamina_cleanup(void)
{
	misc_deregister(&stamdev);
	kfree(table);
}
