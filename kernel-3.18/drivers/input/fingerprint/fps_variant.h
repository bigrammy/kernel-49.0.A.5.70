#ifndef _FPS_VARIANT_H_
#define _FPS_VARIANT_H_

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>

#define FPS_ID_GPIO 120

#define FPS_ELAN_ID 0
#define FPS_EGIS_ID 1

struct fps_variant {
	struct platform_device *plat_dev;
	unsigned int id_gpio;	    /* interrupt GPIO pin number */
	unsigned int variant;
	//pinctrl-names = "default", "fp_id_high", "fp_id_low","fp_id_in_float","fp_id_in_low","fp_id_in_high" ;
	struct pinctrl          *fps_variant_pinctrl;
	struct pinctrl_state    *fp_id_high;
	struct pinctrl_state    *fp_id_low;
	struct pinctrl_state    *fp_id_in_high;

};


int fps_variant_id(void);

#endif
