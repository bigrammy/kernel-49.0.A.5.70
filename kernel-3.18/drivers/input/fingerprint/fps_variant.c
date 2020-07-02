

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#endif

#include <linux/gpio.h>
#include <linux/mutex.h>
#include <linux/list.h>

//#include <mach/gpio.h>
//#include <plat/gpio-cfg.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/slab.h>
#include "fps_variant.h"


#if 1
#define DEBUG_PRINT(fmt, args...) pr_err("fps_var:"fmt, ## args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif

static struct fps_variant *fps_var = NULL;

int fps_variant_id(void)
{
	if(fps_var == NULL)
	{
		pr_err("%s: fps_variant should probe earlier then fps sensor\n", __func__);
		return 0;
	}

	return fps_var->variant;
}
EXPORT_SYMBOL(fps_variant_id);

static int fps_variant_verify_id(struct fps_variant *data)
{
	int errorno,fps_id;

	errorno = gpio_request(data->id_gpio,"fps_id_gpio");
	if(errorno<0)
	{
		pr_err("%s: id gpio request failed\n", __func__);
		return errorno;
	}

	errorno = gpio_direction_input(data->id_gpio);
	if(errorno < 0)
	{
		pr_err("%s: id gpio input failed\n", __func__);
		return -EINVAL;
	}

	fps_id = gpio_get_value(data->id_gpio);

	gpio_free(data->id_gpio);

	data -> variant = fps_id;
	DEBUG_PRINT("%s: fps variant = %d\n",
				__func__, data -> variant);
	return 0;
}

static int fps_variant_adjust_gpio(struct fps_variant *data)
{
	switch(data->variant)
	{
		case FPS_ELAN_ID://0
		{
			pinctrl_select_state(data->fps_variant_pinctrl, data->fp_id_low);
			break;
		}
		case FPS_EGIS_ID://1
		{
			pinctrl_select_state(data->fps_variant_pinctrl, data->fp_id_low);
			break;
		}
		default:
			break;
	}


	return 0;
}

static int fps_variant_parse_pinctrl(struct device *dev,
	struct fps_variant *data)
{
	int ret = 0;
	data->fps_variant_pinctrl = devm_pinctrl_get(dev);
	if (IS_ERR(data->fps_variant_pinctrl)) {
		ret = PTR_ERR(data->fps_variant_pinctrl);
		dev_err(dev, "fwq Cannot find fp id pinctrl!\n");
		return ret;
	}
	//pinctrl-names = "default", "fp_id_high", "fp_id_low","fp_id_in_float","fp_id_in_low","fp_id_in_high" ;
	data->fp_id_high = pinctrl_lookup_state(data->fps_variant_pinctrl, "fp_id_high");
	if (IS_ERR(data->fp_id_high)) {
		ret = PTR_ERR(data->fp_id_high);
		dev_err(dev, "fwq Cannot find fp fp_id_high!\n");
		return ret;
	}
	data->fp_id_low = pinctrl_lookup_state(data->fps_variant_pinctrl, "fp_id_low");
	if (IS_ERR(data->fp_id_low)) {
		ret = PTR_ERR(data->fp_id_low);
		dev_err(dev, "fwq Cannot find fp  fp_id_low!\n");
		return ret;
	}
//	data->fp_id_in_high = pinctrl_lookup_state(data->fps_variant_pinctrl, "fp_id_in_high");
//	if (IS_ERR(data->fp_id_in_high)) {
//		ret = PTR_ERR(data->fp_id_in_high);
//		dev_err(dev, "fwq Cannot find fp  fp_id_in_high!\n");
//		return ret;
//	}
//
//	pinctrl_select_state(data->fps_variant_pinctrl, data->fp_id_in_high);
	return 0;
}

static int fps_variant_parse_dt(struct device *dev,
	struct fps_variant *data)
{
	struct device_node *np = dev->of_node;
	//enum of_gpio_flags flags;
	int errorno = 0;
	int gpio;

	fps_variant_parse_pinctrl(dev,data);

	//gpio = of_get_named_gpio(np, "egis,reset-gpio", 0);
	of_property_read_u32_array(np, "id_gpio",
						   &gpio, 1);;
	if (gpio < 0) {
		errorno = gpio;
		DEBUG_PRINT("%s:ERROR id_gpio=%d\n",
					__func__, gpio);
		goto dt_exit;
	} else {
		data->id_gpio = gpio;
		DEBUG_PRINT("%s: id_gpio=%d\n",
			__func__, data->id_gpio);
	}

	DEBUG_PRINT("%s is successful\n", __func__);
	return errorno;
dt_exit:
	pr_err("%s is failed\n", __func__);
	return errorno;
}

static int fps_variant_probe(struct platform_device *pdev);
static int fps_variant_remove(struct platform_device *pdev);

static struct of_device_id fps_variant_match_table[] = {
	{ .compatible = "fps,fps_variant",},
	{},
};
MODULE_DEVICE_TABLE(of, fps_variant_match_table);

static struct platform_driver fps_variant_driver = {
	.driver = {
		.name		= "fps_variant",
		.owner		= THIS_MODULE,
		.of_match_table = fps_variant_match_table,
	},
    .probe =    fps_variant_probe,
    .remove =   fps_variant_remove,
};
module_platform_driver(fps_variant_driver);



static int fps_variant_remove(struct platform_device *pdev)
{


    return 0;
}


static int fps_variant_probe(struct platform_device *pdev)
{
//	struct device *dev = &pdev->dev;

	int retval;

	DEBUG_PRINT("%s initial\n", __func__);

	/* Allocate driver data */
	fps_var = kzalloc(sizeof(*fps_var), GFP_KERNEL);
	if (fps_var== NULL) {
		pr_err("%s - Failed to kzalloc\n", __func__);
		return -ENOMEM;
	}

	/* device tree call */
	if (pdev->dev.of_node) {
		retval = fps_variant_parse_dt(&pdev->dev, fps_var);
		if (retval) {
			pr_err("%s - Failed to parse DT\n", __func__);
			kfree(fps_var);
			fps_var = NULL;
			return retval;
		}

	}

	fps_variant_verify_id(fps_var);

	fps_variant_adjust_gpio(fps_var);

	fps_var->plat_dev = pdev;

	pr_err("%s - %s \n", __func__,fps_variant_driver.driver.name);

	return 0;
}
