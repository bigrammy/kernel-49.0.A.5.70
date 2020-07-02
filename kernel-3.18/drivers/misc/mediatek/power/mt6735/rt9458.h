#ifndef __RT9458_CHARGER_H
#define __RT9458_CHARGER_H

#define RT9458_SLAVE_ADDR	0x6A
#define RT9458_VENDOR_ID	0x00
#define RT9458_CHIP_REV_E1	0x01
#define RT9458_CHIP_REV_E2	0x02

enum rt9458_reg_addr {
	RT9458_REG_CTRL1,
	RT9458_REG_CTRL2,
	RT9458_REG_CTRL3,
	RT9458_REG_DEVID,
	RT9458_REG_CTRL4,
	RT9458_REG_CTRL5,
	RT9458_REG_CTRL6,
	RT9458_REG_CTRL7,
	RT9458_REG_IRQ1,
	RT9458_REG_IRQ2,
	RT9458_REG_IRQ3,
	RT9458_REG_MASK1,
	RT9458_REG_MASK2,
	RT9458_REG_MASK3,
	RT9458_REG_CTRL8 = 0x11,
	RT9458_REG_IRQSTATUS = 0x15,
	RT9458_REG_MAX,
};

/* uA */
#define RT9458_ICHG_NUM		8
#define RT9458_ICHG_MAX		1550000
#define RT9458_ICHG_MIN		500000
#define RT9458_ICHG_STEP	150000

/* uA */
#define RT9458_IEOC_NUM		8
#define RT9458_IEOC_MAX		400000
#define RT9458_IEOC_MIN		50000
#define RT9458_IEOC_STEP	50000

/* uV */
#define RT9458_MIVR_NUM		7
#define RT9458_MIVR_MAX		4700000
#define RT9458_MIVR_MIN		4100000
#define RT9458_MIVR_STEP	100000

/* uV */
#define RT9458_VPREC_NUM	6
#define RT9458_VPREC_MAX	3000000
#define RT9458_VPREC_MIN	2000000
#define RT9458_VPREC_STEP	200000

/* uA */
#define RT9458_IPREC_NUM	3
#define RT9458_IPREC_MAX	60000
#define RT9458_IPREC_MIN	20000
#define RT9458_IPREC_STEP	20000

/* uV */
#define RT9458_BOOST_VOREG_NUM	48
#define RT9458_BOOST_VOREG_MAX	5600000
#define RT9458_BOOST_VOREG_MIN	4425000
#define RT9458_BOOST_VOREG_STEP	25

/* uV */
#define RT9458_BOOST_VMREG_NUM	13
#define RT9458_BOOST_VMREG_MAX	5600000
#define RT9458_BOOST_VMREG_MIN	5300000
#define RT9458_BOOST_VMREG_STEP	25000

/* ========== CTRL1 0x00 ============ */
#define RT9458_SHIFT_OTG_POLAR 1
#define RT9458_SHIFT_PWR_RDY  2
#define RT9458_SHIFT_BOOST_MODE   3
#define RT9458_SHIFT_CHG_STAT     4

#define RT9458_MASK_OTG_POLAR (1 << RT9458_SHIFT_OTG_POLAR)
#define RT9458_MASK_PWR_RDY  (1 << RT9458_SHIFT_PWR_RDY)
#define RT9458_MASK_BOOST_MODE   (1 << RT9458_SHIFT_BOOST_MODE)
#define RT9458_MASK_CHG_STAT  0x30 

/* ========== CTRL2 0x01 ============ */
#define RT9458_SHIFT_OPA_MODE   0
#define RT9458_SHIFT_HZ_EN      1
#define RT9458_SHIFT_AICR_INT   2
#define RT9458_SHIFT_TE_EN      3
#define RT9458_SHIFT_HIGHER_OCP 4
#define RT9458_SHIFT_TE_SHTDM_EN 5
#define RT9458_SHIFT_AICR       6

#define RT9458_MASK_OPA_MODE   (1 << RT9458_SHIFT_OPA_MODE)
#define RT9458_MASK_HZ_EN      (1 << RT9458_SHIFT_HZ_EN)
#define RT9458_MASK_AICR_INT   (1 << RT9458_SHIFT_AICR_INT)
#define RT9458_MASK_TE_EN      (1 << RT9458_SHIFT_TE_EN)
#define RT9458_MASK_HIGHER_OCP (1 << RT9458_SHIFT_HIGHER_OCP)
#define RT9458_MASK_TE_SHTDM_EN (1 << RT9458_SHIFT_TE_SHTDM_EN)
#define RT9458_MASK_AICR      0xC0


/* ========== CTRL3 0x02 ============ */
#define RT9458_SHIFT_OTG_EN 0
#define RT9458_SHIFT_OTG_PL 1
#define RT9458_SHIFT_VOREG  2

#define RT9458_MASK_OTG_EN (1 << RT9458_SHIFT_OTG_EN)
#define RT9458_MASK_OTG_PL (1 << RT9458_SHIFT_OTG_PL)
#define RT9458_MASK_VOREG  0xFC

/* ========== CTRL4 0x04 ============ */
#define RT9458_SHIFT_RST  7
#define RT9458_MASK_RST   (1 << RT9458_SHIFT_RST) 

/* ========== CTRL5 0x05 ============ */
#define RT9458_SHIFT_IEOC    0
#define RT9458_SHIFT_TMR_EN 7

#define RT9458_MASK_TMR_EN  (1 << RT9458_SHIFT_TMR_EN)
#define RT9458_MASK_IEOC     0x07

/* ========== CTRL6 0x06 ============ */
#define RT9458_SHIFT_VPREC    0
#define RT9458_SHIFT_FIX_FREQ 3
#define RT9458_SHIFT_ICHG     4
#define RT9458_SHIFT_AICR_SEL 7

#define RT9458_MASK_VPREC    0x07
#define RT9458_MASK_FIX_FREQ (1 << RT9458_SHIFT_FIX_FREQ)
#define RT9458_MASK_ICHG     0x70
#define RT9458_MASK_AICR_SEL (1 << RT9458_SHIFT_AICR_SEL)

/* ========== CTRL7 0x07 ============ */
#define RT9458_SHIFT_VMREG   0
#define RT9458_SHIFT_CHG_EN  4
#define RT9458_SHIFT_BATD_EN 6

#define RT9458_MASK_VMREG   0x0F
#define RT9458_MASK_CHG_EN  (1 << RT9458_SHIFT_CHG_EN)
#define RT9458_MASK_BATD_EN (1 << RT9458_SHIFT_BATD_EN)

/*[Arima_8100][bozhi_lin] RID003699 OVP warning message to end-user 20170320 begin*/
/* ========== IRQ1 0x08 ============ */
#define RT9458_SHIFT_VINOVPI 6
#define RT9458_MASK_VINOVPI   (1 << RT9458_SHIFT_VINOVPI)
/*[Arima_8100][bozhi_lin] 20170320 end*/

/* ========== IRQ2 0x09 ============ */
#define RT9458_SHIFT_CHMIVRI	0

#define RT9458_MASK_CHMIVRI	(1 << RT9458_SHIFT_CHMIVRI)

/* ========== CTRL8 0x11 ============ */
#define RT9458_SHIFT_IPREC 0
#define RT9458_SHIFT_MIVR  4

#define RT9458_MASK_IPREC 0x03
#define RT9458_MASK_MIVR  0x70 

/* ========== IRQSTATUS 0x15 ============ */
#define RT9458_SHIFT_MIVR_STAT	0

#define RT9458_MASK_MIVR_STAT	(1 << RT9458_SHIFT_MIVR_STAT)

struct mtk_charger_info;

/* MTK charger interface */
typedef int (*mtk_charger_intf)(struct mtk_charger_info *mchr_info, void *data);

struct mtk_charger_info {
	const mtk_charger_intf *mchr_intf;
	const char *name;
	u8 device_id;
};

extern void mtk_charger_set_info(struct mtk_charger_info *info);

/*
 * The following interface are not related to charger
 * They are implemented in mtk_charger_intf.c
 */
extern int mtk_charger_sw_init(struct mtk_charger_info *info, void *data);
extern int mtk_charger_set_hv_threshold(struct mtk_charger_info *info, void *data);
extern int mtk_charger_get_hv_status(struct mtk_charger_info *info, void *data);
extern int mtk_charger_get_battery_status(struct mtk_charger_info *info, void *data);
extern int mtk_charger_get_charger_det_status(struct mtk_charger_info *info, void *data);
extern int mtk_charger_get_charger_type(struct mtk_charger_info *info, void *data);
extern int mtk_charger_get_is_pcm_timer_trigger(struct mtk_charger_info *info, void *data);
extern int mtk_charger_set_platform_reset(struct mtk_charger_info *info, void *data);
extern int mtk_charger_get_platform_boot_mode(struct mtk_charger_info *info, void *data);
extern int mtk_charger_set_power_off(struct mtk_charger_info *info, void *data);
extern int mtk_charger_get_power_source(struct mtk_charger_info *info, void *data);
extern int mtk_charger_get_csdac_full_flag(struct mtk_charger_info *info, void *data);
extern int mtk_charger_diso_init(struct mtk_charger_info *info, void *data);
extern int mtk_charger_get_diso_state(struct mtk_charger_info *info, void *data);
extern int mtk_charger_set_vbus_ovp_en(struct mtk_charger_info *info, void *data);
extern int mtk_charger_get_bif_vbat(struct mtk_charger_info *info, void *data);
extern int mtk_charger_set_chrind_ck_pdn(struct mtk_charger_info *info, void *data);
extern int mtk_charger_get_bif_tbat(struct mtk_charger_info *info, void *data);
extern int mtk_charger_set_dp(struct mtk_charger_info *info, void *data);
extern int mtk_charger_get_bif_is_exist(struct mtk_charger_info *info, void *data);

#endif /* __RT9458_CHARGER_H */
