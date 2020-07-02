
#include <linux/delay.h>

#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "ov8856_mipiraw_Sensor.h"

#define PFX "OV8856_OTP"
#define LOG_INF(format, args...)	pr_err(PFX "[%s] " format "\n", __FUNCTION__, ##args)
//#define LOG_INF(format, args...)	do {} while (0)


#define OV8856_OTP_ADDR_AWB_FLAG        0x7010
#define OV8856_OTP_ADDR_AWB_BASE_1      0x7011
#define OV8856_OTP_ADDR_AWB_BASE_2      0x7019
	#define OV8856_OTP_OFFSET_MODULE_ID     0
	#define OV8856_OTP_OFFSET_LENS_ID       1
	#define OV8856_OTP_OFFSET_YEAR_MONTH    2
	#define OV8856_OTP_OFFSET_DAY           3
	#define OV8856_OTP_OFFSET_UNIT_RG	    4
	#define OV8856_OTP_OFFSET_UNIT_BG       5
	#define OV8856_OTP_OFFSET_GOLDEN_RG     6
	#define OV8856_OTP_OFFSET_GOLDEN_BG     7
#define OV8856_OTP_ADDR_VCM_FLAG        0x7021
#define OV8856_OTP_ADDR_VCM_BASE_1      0x7022
#define OV8856_OTP_ADDR_VCM_BASE_2      0x7025
	#define OV8856_OTP_OFFSET_VCMST_MSB     0
	#define OV8856_OTP_OFFSET_VCMED_MSB     1
	#define OV8856_OTP_OFFSET_VCM_LSB       2
#define OV8856_OTP_ADDR_LSC_FLAG        0x7028
#define OV8856_OTP_ADDR_LSC_BASE_1      0x7029
#define OV8856_OTP_ADDR_LSC_BASE_2      0x711A
#define OV8856_OTP_LENC_NUM	240

#define OV8856_OTP_TYPICAL_GOLDEN_RG      142
#define OV8856_OTP_TYPICAL_GOLDEN_BG      157
#define OV8856_OTP_TYPICAL_UNIT_RG        142
#define OV8856_OTP_TYPICAL_UNIT_BG        157

union ov8856_production_year_month {
	struct {
		u8 month:4;
		u8 year:4;
	} st;
	u8 value;
};

struct ov8856_otp_data {
	kal_uint16 flag; // bit[7]: info, bit[6]:wb, bit[5]:vcm, bit[4]:lenc
	kal_uint16 module_id;
	kal_uint16 lens_id;
	kal_uint16 year;
	kal_uint16 month;
	kal_uint16 day;
	kal_uint16 unit_rg;
	kal_uint16 unit_bg;
	kal_uint16 golden_rg;
	kal_uint16 golden_bg;

	kal_uint16 lenc[OV8856_OTP_LENC_NUM];
	kal_uint16 checksum;
	kal_uint16 VCM_start;
	kal_uint16 VCM_end;
	kal_uint16 VCM_dir;
};

extern kal_uint16 ov8856_read_sensor(kal_uint32 addr);
extern void ov8856_write_sensor(kal_uint32 addr, kal_uint32 para);

kal_uint16 ov8856_read_module_id(void)
{
	int otp_ctrl_value = 0, otp_flag, addr, group_id = 0;
	kal_uint16 module_id = 0;

	ov8856_write_sensor(0x0103, 0x01);
	ov8856_write_sensor(0x0100, 0x00);
	ov8856_write_sensor(0x0100, 0x01);

	ov8856_write_sensor(0x5001, (0x00 & 0x08) | (otp_ctrl_value & (~0x08)));

	// read OTP into buffer
	ov8856_write_sensor(0x3d84, 0xC0);
	ov8856_write_sensor(0x3d88, 0x70); // OTP start address
	ov8856_write_sensor(0x3d89, 0x10);
	ov8856_write_sensor(0x3d8A, 0x72); // OTP end address
	ov8856_write_sensor(0x3d8B, 0x0a);
	ov8856_write_sensor(0x3d81, 0x01); // load otp into buffer	ov8856_read_sensor(1);

	mdelay(10);

	// OTP base information and WB calibration data
	otp_flag = ov8856_read_sensor(OV8856_OTP_ADDR_AWB_FLAG);
	addr = 0;
	if ((otp_flag & 0x30) == 0x10) {
		group_id = 2;
		addr = OV8856_OTP_ADDR_AWB_BASE_2; // base address of info group 2
	} else if ((otp_flag & 0xc0) == 0x40) {
		group_id = 1;
		addr = OV8856_OTP_ADDR_AWB_BASE_1; // base address of info group 1
	}

	LOG_INF("AWB Basic Group ID=0x%x, Addr=0x%x, Flag=0x%x", group_id, addr, otp_flag);
	if (addr != 0) {
		module_id = ov8856_read_sensor(addr + OV8856_OTP_OFFSET_MODULE_ID);
	}
	else {
		module_id = 0;
	}

	LOG_INF("OTP.module_id=0x%x", module_id);
	return module_id;
}

static int ov8856_otp_read(struct ov8856_otp_data *data)
{
	int otp_ctrl_value, otp_flag, addr, group_id = 0, i, checksum2 = 0;
	union ov8856_production_year_month year_month;

	otp_ctrl_value = ov8856_read_sensor(0x5001);
	otp_ctrl_value = ov8856_read_sensor(0x5001);
	if (0 == otp_ctrl_value) {
		LOG_INF("OTP read error, exit OTP reading.");
		return 1;
	}
	ov8856_write_sensor(0x5001, (0x00 & 0x08) | (otp_ctrl_value & (~0x08)));

	// read OTP into buffer
	ov8856_write_sensor(0x3d84, 0xC0);
	ov8856_write_sensor(0x3d88, 0x70); // OTP start address
	ov8856_write_sensor(0x3d89, 0x10);
	ov8856_write_sensor(0x3d8A, 0x72); // OTP end address
	ov8856_write_sensor(0x3d8B, 0x0a);
	ov8856_write_sensor(0x3d81, 0x01); // load otp into buffer	ov8856_read_sensor(1);

	mdelay(10);

	// OTP base information and WB calibration data
	otp_flag = ov8856_read_sensor(OV8856_OTP_ADDR_AWB_FLAG);
	addr = 0;
	if ((otp_flag & 0x30) == 0x10) {
		group_id = 2;
		addr = OV8856_OTP_ADDR_AWB_BASE_2; // base address of info group 2
	} else if ((otp_flag & 0xc0) == 0x40) {
		group_id = 1;
		addr = OV8856_OTP_ADDR_AWB_BASE_1; // base address of info group 1
	}

	LOG_INF("AWB Basic Group ID=0x%x, Addr=0x%x, Flag=0x%x", group_id, addr, otp_flag);
	if (addr != 0) {
		data->flag = 0xC0; // valid info and AWB in OTP
		data->module_id = ov8856_read_sensor(addr + OV8856_OTP_OFFSET_MODULE_ID);
		data->lens_id = ov8856_read_sensor( addr + OV8856_OTP_OFFSET_LENS_ID);
		year_month.value = ov8856_read_sensor( addr + OV8856_OTP_OFFSET_YEAR_MONTH);
		data->year = year_month.st.year;
		data->month = year_month.st.month;
		data->day = ov8856_read_sensor( addr + OV8856_OTP_OFFSET_DAY);
		data->unit_rg = ov8856_read_sensor(addr + OV8856_OTP_OFFSET_UNIT_RG);
		data->unit_bg = ov8856_read_sensor(addr + OV8856_OTP_OFFSET_UNIT_BG);
		data->golden_rg = ov8856_read_sensor(addr + OV8856_OTP_OFFSET_GOLDEN_RG);
		data->golden_bg = ov8856_read_sensor(addr + OV8856_OTP_OFFSET_GOLDEN_BG);
	}
	else {
		data->flag = 0x00; // not info and AWB in OTP
		data->module_id = 0;
		data->lens_id = 0;
		data->year = 0;
		data->month = 0;
		data->day = 0;
		data->unit_rg = 0;
		data->unit_bg = 0;
		data->golden_rg = 0;
		data->golden_bg = 0;
	}
	LOG_INF("OTP.flag=0x%x", data->flag);
	LOG_INF("OTP.module_id=0x%x", data->module_id);
	LOG_INF("OTP.lens_id=0x%x", data->lens_id);
	LOG_INF("OTP.ProduceDate=201%d/%02d/%02d", data->year, data->month, data->day);
	LOG_INF("OTP.unit_rg=%d", data->unit_rg);
	LOG_INF("OTP.unit_bg=%d", data->unit_bg);
	LOG_INF("OTP.golden_rg=%d", data->golden_rg);
	LOG_INF("OTP.golden_bg=%d", data->golden_bg);

	// OTP VCM Calibration
	otp_flag = ov8856_read_sensor(OV8856_OTP_ADDR_VCM_FLAG);
	addr = 0;
	if ((otp_flag & 0x30) == 0x10) {
		group_id = 2;
		addr = OV8856_OTP_ADDR_VCM_BASE_2; // base address of VCM Calibration group 2
	} else if ((otp_flag & 0xc0) == 0x40) {
		group_id = 1;
		addr = OV8856_OTP_ADDR_VCM_BASE_1; // base address of VCM Calibration group 1
	}
	LOG_INF("VCM Group ID=0x%x, Addr=0x%x, Flag=0x%x", group_id, addr, otp_flag);
	if (addr != 0) {
		int otp_vcm_value;
		data->flag |= 0x20;
		otp_vcm_value = ov8856_read_sensor(addr + OV8856_OTP_OFFSET_VCM_LSB);
		data->VCM_start = (ov8856_read_sensor(addr + OV8856_OTP_OFFSET_VCMST_MSB) << 2) | ((otp_vcm_value >> 6) & 0x03);
		data->VCM_end = (ov8856_read_sensor(addr + OV8856_OTP_OFFSET_VCMED_MSB) << 2) | ((otp_vcm_value >> 4) & 0x03);
		data->VCM_dir = (otp_vcm_value>>2) & 0x03;
	} else {
		data->VCM_start = 0;
		data->VCM_end = 0;
		data->VCM_dir = 0;
	}
	LOG_INF("OTP.flag=0x%x", data->flag);
	LOG_INF("OTP.VCM_start=%d", data->VCM_start);
	LOG_INF("OTP.VCM_end=%d", data->VCM_end);
	LOG_INF("OTP.VCM_dir=%d", data->VCM_dir);

	// OTP Lenc Calibration
	otp_flag = ov8856_read_sensor(0x7028);
	addr = 0;

	if ((otp_flag & 0x30) == 0x10) {
		group_id = 2;
		addr = 0x711a; // base address of Lenc Calibration group 2
	} else if ((otp_flag & 0xc0) == 0x40) {
		group_id = 1;
		addr = 0x7029; // base address of Lenc Calibration group 1
	}
	LOG_INF("LENC Group ID=0x%x, Addr=0x%x, Flag=0x%x", group_id, addr, otp_flag);
	if (addr != 0) {
		for (i = 0; i < OV8856_OTP_LENC_NUM; i++) {
			data->lenc[i] = ov8856_read_sensor(addr + i);
			checksum2 += data->lenc[i];
		}
		for (i = 0; i < OV8856_OTP_LENC_NUM / 10; i++) {
			LOG_INF(" Read OTP.lenc[%02d~%02d]=0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", i*10, i*10 + 9,
					data->lenc[i*10],     data->lenc[i*10 + 1], data->lenc[i*10 + 2], data->lenc[i*10 + 3], data->lenc[i*10 + 4],
					data->lenc[i*10 + 5], data->lenc[i*10 + 6], data->lenc[i*10 + 7], data->lenc[i*10 + 8], data->lenc[i*10 + 9]
			);
		}
		checksum2 = (checksum2) % 255 + 1;
		data->checksum = ov8856_read_sensor(addr + OV8856_OTP_LENC_NUM);
		LOG_INF("OTP.checksum=0x%x, CheckSum=0x%x", data->checksum, checksum2);
		if (data->checksum == checksum2) {
			data->flag |= 0x10;
		}
	} else {
		for (i = 0; i < OV8856_OTP_LENC_NUM; i++) {
			data->lenc[i] = 0;
		}
	}
	for (i = 0x7010; i <= 0x720a; i++) {
		ov8856_write_sensor(i, 0); // clear OTP buffer, recommended use continuous write to accelarate
	}

	//set 0x5001[3] to '1'
	otp_ctrl_value = ov8856_read_sensor(0x5001);
	ov8856_write_sensor(0x5001, (0x08 & 0x08) | (otp_ctrl_value & (~0x08)));

	return 0;
}

extern void write_ov8856_sensor_lsc(kal_uint32 addr, kal_uint16 lsc[240]);
static void ov8856_otp_apply(struct ov8856_otp_data *data)
{
	int rg_ratio, bg_ratio, rg_golden, bg_golden, R_gain, G_gain, B_gain, Base_gain, otp_ctrl_value;
	// int i, read_value; // For log LSC

	// apply OTP WB Calibration
	LOG_INF("OTP.flag=0x%x", data->flag);
	if (data->flag & 0x40) {
		rg_golden = data->golden_rg == 0 ? OV8856_OTP_TYPICAL_GOLDEN_RG : data->golden_rg;
		bg_golden = data->golden_bg == 0 ? OV8856_OTP_TYPICAL_GOLDEN_BG : data->golden_bg;
		rg_ratio = data->unit_rg == 0 ? OV8856_OTP_TYPICAL_UNIT_RG : data->unit_rg;
		bg_ratio = data->unit_bg == 0 ? OV8856_OTP_TYPICAL_UNIT_BG : data->unit_bg;

		//calculate G gain
		R_gain = (rg_golden * 1000) / rg_ratio;
		B_gain = (bg_golden * 1000) / bg_ratio;
		G_gain = 1000;
		if (R_gain < 1000 || B_gain < 1000) {
			if (R_gain < B_gain)
				Base_gain = R_gain;
			else
				Base_gain = B_gain;
		} else {
			Base_gain = G_gain;
		}
		R_gain = 0x400 * R_gain / (Base_gain);
		B_gain = 0x400 * B_gain / (Base_gain);
		G_gain = 0x400 * G_gain / (Base_gain);
		LOG_INF("Apply R_gain=0x%x, B_gain=0x%x, G_gain=0x%x", R_gain, B_gain, G_gain);

		// update sensor WB gain
		if (R_gain > 0x400) {
			ov8856_write_sensor(0x5019, R_gain>>8);
			ov8856_write_sensor(0x501a, R_gain & 0x00ff);
		}
		if (G_gain > 0x400) {
			ov8856_write_sensor(0x501b, G_gain>>8);
			ov8856_write_sensor(0x501c, G_gain & 0x00ff);
		}
		if (B_gain > 0x400) {
			ov8856_write_sensor(0x501d, B_gain>>8);
			ov8856_write_sensor(0x501e, B_gain & 0x00ff);
		}
	}
	// apply OTP Lenc Calibration
	if (data->flag & 0x10) {
		otp_ctrl_value = ov8856_read_sensor(0x5000);
		otp_ctrl_value = 0x20 | otp_ctrl_value;
		ov8856_write_sensor(0x5000, otp_ctrl_value);

		write_ov8856_sensor_lsc(0x5900, data->lenc);

//		for (i = 0; i < 240; i++) { // For log LSC
//			read_value = ov8856_read_sensor(0x5900 + i);
//			LOG_INF("SENSOR[0x%x] = 0x%x", 0x5900 + i, read_value);
//		}
	}
}

u8 g_ov8856_otp_inited = 0;
struct ov8856_otp_data g_ov8856_otp_data;
void ov8856_otp_update(void)
{
	if (0 == g_ov8856_otp_inited) {
		if (0 == ov8856_otp_read(&g_ov8856_otp_data)) {
			g_ov8856_otp_inited = 1;
		}
	}
	ov8856_otp_apply(&g_ov8856_otp_data);
}
