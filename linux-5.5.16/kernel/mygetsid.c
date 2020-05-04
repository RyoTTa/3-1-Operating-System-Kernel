#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>

int __sys_mygetsid(int *buf){
	int sid=2015114398;

	printk("sys_mygetsid()\n");
	*buf = sid;
	printk("sys_mygetsid() :%d\n", *buf);

	return 0;
}
SYSCALL_DEFINE1(mygetsid, int*, buf){
	
	return __sys_mygetsid(buf);
}
