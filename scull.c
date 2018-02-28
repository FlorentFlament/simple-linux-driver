#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL v2");

static dev_t scull_dev;
static struct cdev scull_cdev;
static DEFINE_MUTEX(scull_lock);

static ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
  // Capping count to length of SCULL_MSG
  #define SCULL_MSG "Hello !\n"
  #define SCULL_MLEN 8
  count = count < SCULL_MLEN ? count : SCULL_MLEN;

  if (copy_to_user(buf, SCULL_MSG, count)) {
    printk(KERN_ERR "scull: Failed to copy data to user\n");
    return -EFAULT;
  }

  printk(KERN_INFO "scull: Sent %lu bytes to user\n", count);
  return count;
}

static ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
  char lbuf[64];
  count = count < 63 ? count : 63; // Read up to 63 chars into the 64 chars buffer

  if (copy_from_user(&lbuf, buf, count)) {
    printk(KERN_ERR "scull: Failed to copy data from user\n");
    return -EFAULT;
  }
  lbuf[count] = '\0';

  // Locking on a mutex to log the whole block
  mutex_lock(&scull_lock);
  printk(KERN_INFO "scull: Read %lu bytes from user\n", count);
  printk(KERN_INFO "scull: %s\n", lbuf);
  mutex_unlock(&scull_lock);
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
    goto out;
  }
  printk(KERN_INFO "scull: Char device number registered: %d %d\n", MAJOR(scull_dev), 0);

  // Allocating & registering cdev structure
  cdev_init(&scull_cdev, &scull_fops);
  scull_cdev.owner = THIS_MODULE;

  result = cdev_add(&scull_cdev, scull_dev, 1) < 0;
  if (result < 0) {
    printk(KERN_ERR "scull: Failed to add char device to system\n");
    goto error_cdev;
  }
  printk(KERN_INFO "scull: Char device successfuly added to system\n");

out:
  return result;

error_cdev:
  unregister_chrdev_region(scull_dev, 1);
  goto out;
}

static void scull_exit(void)
{
  cdev_del(&scull_cdev);
  printk(KERN_INFO "scull: Char device removed from system\n");

  unregister_chrdev_region(scull_dev, 1);
  printk(KERN_INFO "scull: Char device number unregistered: %d %d\n", MAJOR(scull_dev), 0);
  printk(KERN_INFO "scull: Bye !\n");
}

module_init(scull_init);
module_exit(scull_exit);
