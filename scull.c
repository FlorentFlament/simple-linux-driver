#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL v2");

static dev_t scull_dev;
static struct cdev *scull_cdev;

static ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
  #define SCULL_MSG "Hello !\n"
  #define SCULL_MLEN 8
  count = count < SCULL_MLEN ? count : SCULL_MLEN;
  count -= copy_to_user(buf, SCULL_MSG, count);
  printk(KERN_INFO "scull: Sent %lu bytes to user\n", count);
  return count;
}

static ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
  char lbuf[64];
  count = count < 63 ? count : 63; // Read up to 63 chars into the 64 chars buffer
  count -= copy_from_user(&lbuf, buf, count);
  lbuf[count] = '\0';
  printk(KERN_INFO "scull: Read %lu bytes from user\n", count);
  printk(KERN_INFO "scull: %s\n", lbuf);
  return count;
}

static struct file_operations scull_fops = {
  .owner = THIS_MODULE,
  .read = scull_read,
  .write = scull_write,
};

static int scull_init(void)
{
  int result;
  printk(KERN_INFO "scull: Hello !\n");

  result = alloc_chrdev_region(&scull_dev, 0, 1, "scull");
  if (result < 0) {
    printk(KERN_ERR "scull: Failed to register char device number\n");
    return result;
  }
  printk(KERN_INFO "scull: Char device number registered: %d %d\n", MAJOR(scull_dev), 0);

  // Allocating & registering cdev structure
  scull_cdev = cdev_alloc();
  scull_cdev->ops = &scull_fops;
  scull_cdev->owner = THIS_MODULE;
  result = cdev_add(scull_cdev, scull_dev, 1) < 0;
  if (result < 0) {
    printk(KERN_ERR "scull: Failed to add char device to system\n");
    unregister_chrdev_region(scull_dev, 1);
    return result;
  }
  printk(KERN_INFO "scull: Char device successfuly added to system\n");

  return 0;
}

static void scull_exit(void)
{
  cdev_del(scull_cdev);
  printk(KERN_INFO "scull: Char device removed from system\n");
  unregister_chrdev_region(scull_dev, 1);
  printk(KERN_INFO "scull: Char device number unregistered: %d %d\n", MAJOR(scull_dev), 0);
  printk(KERN_INFO "scull: Bye !\n");
}

module_init(scull_init);
module_exit(scull_exit);
