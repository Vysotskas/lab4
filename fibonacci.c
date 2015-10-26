#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sergey Gerasimov");


static int fibonacci_init(void);
static void fibonacci_exit(void);

static ssize_t read_number(struct file *, char *, size_t, loff_t *, unsigned long long *);

static ssize_t input_read(struct file *, char *, size_t, loff_t *);
static ssize_t input_write(struct file *, const char *, size_t, loff_t *);
static ssize_t result_read(struct file *, char *, size_t, loff_t *);

#define INPUT_NAME "fibonacci_input"
#define RESULT_NAME "fibonacci_result"

#define BUF_SIZE 32

static int input_major = -1;
static int result_major = -1;

static int input_error_code = 0;
static char *input_error_message;

static char buf[BUF_SIZE] = { 0 };

static long n = 0;
static unsigned long long res = 0;

static struct file_operations input_fops = {
  .owner = THIS_MODULE,
  .read = input_read,
  .write = input_write
};
static struct file_operations result_fops = {
  .owner = THIS_MODULE,
  .read = result_read
};


static int fibonacci_init()
{
  input_major = register_chrdev(0, INPUT_NAME, &input_fops);
  if (input_major < 0) {
    printk("Input device is not created");
    fibonacci_exit();
    return input_major;
  }

  result_major = register_chrdev(0, RESULT_NAME, &result_fops);
  if (result_major < 0) {
    printk("Result device is not created");
    fibonacci_exit();
    return result_major;
  }

  return 0;
}

static void fibonacci_exit()
{
  if (result_major > 0) {
    unregister_chrdev(result_major, RESULT_NAME);
  }

  if (input_major > 0) {
    unregister_chrdev(input_major, INPUT_NAME);
  }
}


static ssize_t input_write(struct file *filp, const char *buff, size_t len, loff_t *off)
{
  long i;
  unsigned long long a;
  unsigned long long tmp;

  if (len > BUF_SIZE) {
    input_error_code = -EINVAL;
    input_error_message = "Too long input string";
    return input_error_code;
  }
  if (copy_from_user(buf, buff, len)) {
    input_error_code = -EINVAL;
    input_error_message = "copy_from_user error";
    return input_error_code;
  }
  if (kstrtol(buf, 10, &n) == -EINVAL) {
    input_error_code = -EINVAL;
    input_error_message = "Input string is not the number";
    return input_error_code;
  }

  if (n < 0) {
    input_error_code = -EINVAL;
    input_error_message = "Input number must be non-negative";
    return input_error_code;
  }

  a = 0;
  res = 1;

  if (n == 0) {
    res = 0;
  }

  for (i = 0; i < n - 1; ++i) {
    tmp = res;
    res = res + a;
    if (res < tmp) {
      input_error_code = -EINVAL;
      input_error_message = "Too big input number";
      return input_error_code;
    }
    a = tmp;
  }

  return len;
}

static ssize_t read_number(struct file *filp, char *buff, size_t len, loff_t *off,unsigned long long *num)
{
  ssize_t size;
  long length;

  if (*off != 0) {
    return 0;
  }

  if (input_error_code < 0) {
    copy_to_user(buff, input_error_message, strlen(input_error_message));
    length = strlen(input_error_message);
    *off = length;
    return length;
  }


  snprintf(buf, BUF_SIZE, "%llu", *num);
  size = strlen(buf);
  if (size >= len) {
    return -EINVAL;
  }
  if (copy_to_user(buff, buf, size)) {
    return -EINVAL;
  }
  *off = size;
  return size;
}

static ssize_t input_read(struct file *filp, char *buff, size_t len, loff_t * off)
{
  unsigned long long tmp;

  tmp = n;
  return read_number(filp, buff, len, off, &tmp);
}

static ssize_t result_read(struct file *filp, char *buff, size_t len, loff_t * off)
{
  return read_number(filp, buff, len, off, &res);
}

module_init(fibonacci_init);
module_exit(fibonacci_exit);
