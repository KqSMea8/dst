#include <linux/init.h>
#include <linux/module.h>

static int m_init(void)
{
	printk(KERN_INFO "hello world\n");
	return 0;
}

static void m_exit(void)
{
	printk(KERN_INFO "goodbye world\n");
}

module_init(m_init)
module_exit(m_exit)
