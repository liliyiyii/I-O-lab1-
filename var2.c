#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Proc Dev");
MODULE_DESCRIPTION("A char driver");
MODULE_VERSION("0.1");

#define BASEMINOR 0

static struct proc_dir_entry* entry;
dev_t devno;
#define NAME      "var2"
#define  PROCFS_MAX_SIZE   256
char proc_buffer[PROCFS_MAX_SIZE];
char g_buffer[PROCFS_MAX_SIZE*10];
static int major = 205;  
static int minor = 0;
static struct class *cls;
static struct device *var2_device;

int g_flag = 0;

int my_atoi(char* pstr)
{
	int Ret_Integer = 0;
	int Integer_sign = 1;
	
	if(pstr == NULL)
	{
		return 0;
	}
	
	while((*pstr) == ' ')
	{
		pstr++;
	}
	
	if(*pstr == '-')
	{
		Integer_sign = -1;
	}
	if(*pstr == '-' || *pstr == '+')
	{
		pstr++;
	}
	
	while(*pstr >= '0' && *pstr <= '9')
	{
		Ret_Integer = Ret_Integer * 10 + *pstr - '0';
		pstr++;
	}
	
	Ret_Integer = Integer_sign * Ret_Integer;
	
	return Ret_Integer;
}

static ssize_t proc_write(struct file *file, const char __user * ubuf, size_t count, loff_t* ppos) 
{
	//printk(KERN_DEBUG "Attempt to write proc file");
	size_t proc_buffer_size = count;
	g_flag = 1;
	if(proc_buffer_size > PROCFS_MAX_SIZE)
	{
		proc_buffer_size = PROCFS_MAX_SIZE;
	}
	memset(proc_buffer, 0x0, PROCFS_MAX_SIZE);
	if(copy_from_user(proc_buffer, ubuf, proc_buffer_size))
	{
		printk(KERN_INFO "write1 : proc_buffer : [%s]\n", proc_buffer);
		return -EFAULT;
	}
	printk(KERN_INFO "write2  : proc_buffer : [%s]\n", proc_buffer);
	return proc_buffer_size;
}

static ssize_t proc_read(struct file *file, char __user * ubuf, size_t count, loff_t* ppos) 
{
	int len = strlen(proc_buffer);
	char arg1[32] = {0};
	int j = 0;
	char arg2[32] = {0};
	int k = 0;
	int i = 0;
	int flag = 0;
	char op = '0';	
	
	if (*ppos > 0 || count < len)
	{
		return 0;
	}

	if (g_flag == 1)
	{
		for (i = 0; i < len; i++)
		{
			if (proc_buffer[i] >= '0' && proc_buffer[i] <= '9' && flag == 0)
			{
				arg1[j++] = proc_buffer[i];
			}
			else
			{
				if (flag == 0)
				{
					flag = 1;	
					op = proc_buffer[i];
					continue;
				}
			}

			if (flag == 1)
			{
				arg2[k++] = proc_buffer[i];
			}
		}
		arg1[j] = '\0';
		arg2[k] = '\0';

		switch(op)
		{
			case '+':
				sprintf(proc_buffer, "%d", my_atoi(arg1)+my_atoi(arg2));
				break;
			case '-':
				sprintf(proc_buffer, "%d", my_atoi(arg1)-my_atoi(arg2));
				break;
			case '*':
				sprintf(proc_buffer, "%d", my_atoi(arg1)*my_atoi(arg2));
				break;
			case '/':
				sprintf(proc_buffer, "%d", my_atoi(arg1)/my_atoi(arg2));
				break;
			default:
				break;
		}

		if (strlen(g_buffer) != 0)
			strcat(g_buffer, " ");
		strcat(g_buffer, proc_buffer);

		memset(proc_buffer, 0x0, PROCFS_MAX_SIZE);
		strcpy(proc_buffer, g_buffer);
		g_flag = 0;
	}
	
	if (copy_to_user(ubuf, proc_buffer, strlen(proc_buffer)) != 0)
	{
		return -EFAULT;
	}
	*ppos = len;
	return len;
}

loff_t proc_llseek (struct file *filp,loff_t offset,int whence)
{
	loff_t newpos;
	
	switch(whence)
	{
		case 0:
			newpos = offset;
			break;
		case 1:
			newpos = filp->f_pos + offset;
			break;
		case 2:
			newpos = PROCFS_MAX_SIZE - 1 + offset;
			break;
		default:
			return -EINVAL;
	}
	
	if((newpos < 0) || (newpos > PROCFS_MAX_SIZE))
	{
		return -EINVAL;
	}
	filp->f_pos = newpos;
	
	return newpos;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = proc_read,
	.write = proc_write,
	.llseek = proc_llseek,
};
	
static struct proc_ops proc_fops = {
	.proc_read = proc_read,
	.proc_write = proc_write,
	.proc_lseek = proc_llseek,
};
static int __init proc_var2_init(void)
{	
	int ret = 0;
	entry = proc_create("var2", 0444, NULL, &proc_fops);
	printk(KERN_INFO "%s: proc file is created\n", THIS_MODULE->name);

	devno = MKDEV(major, minor);
	ret = register_chrdev(major, "var2", &fops);

	cls = class_create(THIS_MODULE, "var2");
    if(IS_ERR(cls))
    {
        unregister_chrdev(major, "var2");
        return -EBUSY;
    }
    var2_device = device_create(cls, NULL, devno, NULL, "var2");
    if(IS_ERR(var2_device))
    {
        class_destroy(cls);
        unregister_chrdev(major,"var2");
        return -EBUSY;
    }

	
	return 0;
}

static void __exit proc_var2_exit(void)
{
	proc_remove(entry);
	device_destroy(cls, devno);
	class_destroy(cls);
	unregister_chrdev(major, "var2");
	printk(KERN_INFO "%s: proc file is deleted\n", THIS_MODULE->name);
}

module_init(proc_var2_init);
module_exit(proc_var2_exit);


