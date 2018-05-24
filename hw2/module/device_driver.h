#ifndef _DEVICE_DRIVER_H_
#define _DEVICE_DRIVER_H_

#include <linux/ioctl.h>

#define DEV_DRIVER_MAJOR 242
#define DEV_DRIVER_NAME "dev_driver"

#define IOCTL_START_APP _IOW(DEV_DRIVER_MAJOR, 0, int)

#endif
