#ifndef _TYPEC_TUSB320_H_
#define _TYPEC_TUSB320_H_
#include <linux/bitops.h>
struct tusb320_device_info {
	int gpio_enb;
	int gpio_intb;
	int irq_intb;
	int irq_active;
	struct i2c_client *client;
	struct work_struct g_intb_work;
	struct delayed_work g_wdog_work;
	struct device *dev;
	struct dual_role_phy_instance *dual_role;
	struct dual_role_phy_desc *desc;
	bool trysnk_attempt;
	int reverse_state;
	bool sink_attached;
	bool clean_failded;
	bool clean_retry_count;
	struct completion reverse_completion;
	struct mutex mutex;
/*[Arima_8103][bozhi_lin] add tusb320 type-c show vendor id 20170517 begin*/
	char device_id[8];
/*[Arima_8103][bozhi_lin] 20170517 end*/
};
#define REVERSE_ATTEMPT 1
#define REVERSE_COMPLETE 2
#define DISABLE_SET 0
#define DISABLE_CLEAR 1
#define REGISTER_NUM    12
/* Register address */
#define TUSB320_REG_DEVICE_ID				0x00
/*[Arima_8103][bozhi_lin] add tusb320 type-c show vendor id 20170517 begin*/
#define TUSB320_REG_DEVICE_ID_END			0x07
/*[Arima_8103][bozhi_lin] 20170517 end*/
#define TUSB320_REG_CURRENT_MODE			0x08
#define TUSB320_REG_ATTACH_STATUS			0x09
#define TUSB320_REG_MODE_SET				0x0a
#define TUSB320_REG_DISABLE				    0x45
/* Register REG_CURRENT_MODE 08 */
#define TUSB320_REG_CUR_MODE_ADVERTISE_MASK		(BIT(7) | BIT(6))
#define TUSB320_REG_CUR_MODE_ADVERTISE_DEFAULT	0x00
#define TUSB320_REG_CUR_MODE_ADVERTISE_MID		BIT(6)
#define TUSB320_REG_CUR_MODE_ADVERTISE_HIGH		BIT(7)
#define TUSB320_REG_CUR_MODE_DETECT_MASK		(BIT(5) | BIT(4))
#define TUSB320_REG_CUR_MODE_DETECT_DEFAULT		0x00
#define TUSB320_REG_CUR_MODE_DETECT_MID			BIT(4)
#define TUSB320_REG_CUR_MODE_DETECT_HIGH		(BIT(5) | BIT(4))
/* Register REG_ATTACH_STATUS 09 */
#define TUSB320_REG_STATUS_MODE					(BIT(7) | BIT(6))
#define TUSB320_REG_STATUS_NOT_ATTACHED			0x00
#define TUSB320_REG_STATUS_AS_DFP				BIT(6)
#define TUSB320_REG_STATUS_AS_UFP				BIT(7)
#define TUSB320_REG_STATUS_TO_ACCESSORY			(BIT(7) | BIT(6))
#define TUSB320_REG_STATUS_CC					BIT(5)
#define TUSB320_REG_STATUS_INT					BIT(4)
#define TUSB320_REG_STATUS_DRP_DUTY_CYCLE		(BIT(2) | BIT(1))
#define TUSB320_REG_STATUS_DRP_DUTY_CYCLE_30	0x00
#define TUSB320_REG_STATUS_DRP_DUTY_CYCLE_40	BIT(1)
#define TUSB320_REG_STATUS_DRP_DUTY_CYCLE_50	BIT(2)
#define TUSB320_REG_STATUS_DRP_DUTY_CYCLE_60	(BIT(2) | BIT(1))
/* Register REG_MODE_SET 0a */
#define TUSB320_REG_SET_MODE					(BIT(5) | BIT(4))
#define TUSB320_REG_SET_BY_PORT					0x00
#define TUSB320_REG_SET_UFP						BIT(4)
#define TUSB320_REG_SET_DFP						BIT(5)
#define TUSB320_REG_SET_DRP						(BIT(5) | BIT(4))
#define TUSB320_REG_SET_SOFT_RESET				BIT(3)
#define TUSB320_REG_SET_DISABLE_RD_RP			BIT(2)
#endif /*_TYPEC_TUSB320_H_*/