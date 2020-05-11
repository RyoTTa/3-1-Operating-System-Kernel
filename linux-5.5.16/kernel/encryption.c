#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/sched/xacct.h>
#include <linux/fcntl.h>
#include <linux/file.h>
#include <linux/uio.h>
#include <linux/fsnotify.h>
#include <linux/security.h>
#include <linux/export.h>
#include <linux/syscalls.h>
#include <linux/pagemap.h>
#include <linux/splice.h>
#include <linux/compat.h>
#include <linux/mount.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/unistd.h>

static inline loff_t *file_ppos(struct file *file)
{
	return file->f_mode & FMODE_STREAM ? NULL : &file->f_pos;
}

int sys_swrite(int fd, char* buf, int len){

	struct fd f = fdget_pos(fd);
	ssize_t ret = -EBADF;
	char *data;
	int i;
	printk("sys_swrite()\n");
	data = kmalloc(sizeof(char)*len,GFP_KERNEL);
	copy_from_user(data,buf,len);

	for(i=0 ; i<len; i++){
		data[i] = ~data[i];
	}

	copy_to_user(buf,data,len);
	if (f.file) {
		loff_t pos, *ppos = file_ppos(f.file);
		if (ppos) {
			pos = *ppos;
			ppos = &pos;
		}
		ret = vfs_write(f.file, buf, len, ppos);
		if (ret >= 0 && ppos)
			f.file->f_pos = pos;
		fdput_pos(f);
	}
	return ret;
}

int sys_sread(int fd, char* buf, int len){

	struct fd f = fdget_pos(fd);
	ssize_t ret = -EBADF;
	char *data;
	int i;
	printk("sys_sread()\n");
	data = kmalloc(sizeof(char)*len,GFP_KERNEL);

	if (f.file) {
		loff_t pos, *ppos = file_ppos(f.file);
		if (ppos) {
			pos = *ppos;
			ppos = &pos;
		}
		ret = vfs_read(f.file, buf, len, ppos);
		if (ret >= 0 && ppos)
			f.file->f_pos = pos;
		fdput_pos(f);
	}
	copy_from_user(data,buf,len);

	for(i=0; i<strlen(data); i++){
		data[i] = ~data[i];
	}

	copy_to_user(buf,data,len);

	return ret;
}

SYSCALL_DEFINE3(swrite, int, fd, char*, buf, int, len)
{
	return sys_swrite(fd, buf, len);
}

SYSCALL_DEFINE3(sread, int, fd, char*, buf, int, len)
{
	return sys_sread(fd, buf, len);
}
