#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/proc_fs.h>

#include <linux/slab.h>
#include <asm/uaccess.h>
#define BUFSIZE  1000
 
 
MODULE_DESCRIPTION("Module which uses functions from other module to demonstrate symbol exporting");
MODULE_AUTHOR("Anna Ayvazyan");
MODULE_LICENSE("GPL v2");

static struct proc_dir_entry *ent;
/* 
static ssize_t mywrite(struct file *file, const char __user *ubuf,size_t count, loff_t *ppos) 
{
	printk( KERN_DEBUG "write handler\n");
	return -1;
}
*/


//int globalVar = 31;
//static int staticVar = 61;

//int exportedVar = 91;
//EXPORT_SYMBOL(exportedVar);

static int irq=20;
module_param(irq,int,0660);
 
static int mode=1;
module_param(mode,int,0660);
 

int exportedFunc(int param);
//{
////	printk(KERN_DEBUG "we are in exportedFuc and the param is %d\n", param);
//	return 3;
//}


//EXPORT_SYMBOL(exportedFunc);

static ssize_t mywrite(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos) 
{


	printk( KERN_DEBUG "write handler\n");
	int num,c,i,m;
	char buf[BUFSIZE];
	if(*ppos > 0 || count > BUFSIZE)
		return -EFAULT;
	if(raw_copy_from_user(buf,ubuf,count))
		return -EFAULT;
	num = sscanf(buf,"%d %d",&i,&m);
	if(num != 2)
		return -EFAULT;
	irq = i; 
	mode = m;
	c = strlen(buf);
	*ppos = c;



	return c;
}

static ssize_t myread(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) 
{
	char buf[BUFSIZE];
	int len=0;
	printk( KERN_DEBUG "read handler\n");
	if(*ppos > 0 || count < BUFSIZE)
		return 0;
	len += sprintf(buf,"irq = %d\n",irq);
	len += sprintf(buf + len,"mode = %d\n",mode);
	len += sprintf(buf + len,"bufzise = %d\n", BUFSIZE);

	
	void * stuff;
	stuff = kmalloc(BUFSIZE, GFP_KERNEL);
	char* st = (char*)stuff;
	*st = 'a'; *(st + 1) = 'r'; *(st + 2) = 'n', *(st + 3) = 'b'; *(st + 4) = '\0';

        len += sprintf(buf + len, "Allocation finished! %zu, the range is %p-%p\n", ksize(stuff), stuff, stuff + ksize(stuff) - 1);
        len += sprintf(buf + len, "String written in alocated mem is %s\n", st);

	
 	kfree(stuff);



	if(raw_copy_to_user(ubuf,buf,len))
		return -EFAULT;
	*ppos = len;

	return len;
}


/*
static ssize_t myread(struct file *file, char __user *ubuf,size_t count, loff_t *ppos) 
{
	printk( KERN_DEBUG "read handler\n");
	return 0;
}
 */

static struct file_operations myops = 
{
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};
 
static int simple_init(void)
{
        printk(KERN_INFO "Hello, It is procFileMod init!\n");
        int retVal = exportedFunc(7);
        printk(KERN_INFO "returned value is %d\n", retVal);

	ent=proc_create("FriendAnnasMemDev", 0666, NULL, &myops);

 	
	return 0;
}
 
static void simple_cleanup(void)
{
        printk(KERN_INFO "Bye, It was procFileMod exit!\n");
	proc_remove(ent);
}
 
module_init(simple_init);
module_exit(simple_cleanup)






// source:  https://devarea.com/linux-kernel-development-creating-a-proc-file-and-interfacing-with-user-space/#.XkziL2gza70	
