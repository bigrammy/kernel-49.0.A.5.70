/************************************************************************
* Copyright (C) 2012-2015, Focaltech Systems (R)��All Rights Reserved.
*
* File Name: focaltech_test_config_ft5x46.h
*
* Author: Software Development Team, AE
*
* Created: 2015-07-14
*
* Abstract: Set Config for FT5X46\FT5X46i\FT5526\FT3X17\FT5436\FT3X27\FT5526i\FT5416\FT5426\FT5435
*
************************************************************************/
#ifndef _CONFIG_FT5X46_H
#define _CONFIG_FT5X46_H

#include "focaltech_test_main.h"

/*-----------------------------------------------
FT5X46��FT5X22��ͬһϵ��оƬ
FT5422\FT5X22���ڲ��з�ʹ��оƬ����
FT5X46���г�ʹ��оƬ����
------------------------------------------------*/
struct stCfg_FT5X22_TestItem
{
	bool FW_VERSION_TEST;
	bool FACTORY_ID_TEST;
	bool PROJECT_CODE_TEST;
	bool IC_VERSION_TEST;
	bool RAWDATA_TEST;
	bool ADC_DETECT_TEST;
	bool SCAP_CB_TEST;
	bool SCAP_RAWDATA_TEST;
	bool CHANNEL_NUM_TEST;
	bool INT_PIN_TEST;
	bool RESET_PIN_TEST;
	bool NOISE_TEST;
	bool WEAK_SHORT_CIRCUIT_TEST;
	bool UNIFORMITY_TEST;
	bool CM_TEST;

	bool RAWDATA_MARGIN_TEST;
	bool PANEL_DIFFER_TEST;
	bool PANEL_DIFFER_UNIFORMITY_TEST;
	
	bool LCM_ID_TEST;

	bool TE_TEST;
	bool SITO_RAWDATA_UNIFORMITY_TEST;
	bool PATTERN_TEST;
/*[Arima_8100][bozhi_lin] touch aats test LCD noise implement 20170320 begin*/
	bool LCD_NOISE_TEST;
/*[Arima_8100][bozhi_lin] 20170320 end*/
};
struct stCfg_FT5X22_BasicThreshold
{
	BYTE FW_VER_VALUE;
	BYTE Factory_ID_Number;
	char Project_Code[32];
	BYTE IC_Version;
	BYTE LCM_ID;
	int RawDataTest_low_Min;
	int RawDataTest_Low_Max;
	int RawDataTest_high_Min;
	int RawDataTest_high_Max;
	BYTE RawDataTest_SetLowFreq;
	BYTE RawDataTest_SetHighFreq;
	int AdcDetect_Max;
	//int RxShortTest_Min;
	//int RxShortTest_Max;
	//int TxShortTest_Min;
	//int TxShortTest_Max;
	int SCapCbTest_OFF_Min;
	int SCapCbTest_OFF_Max;
	int SCapCbTest_ON_Min;
	int SCapCbTest_ON_Max;
	bool SCapCbTest_LetTx_Disable;
	BYTE SCapCbTest_SetWaterproof_OFF;
	BYTE SCapCbTest_SetWaterproof_ON;
	//int SCapDifferTest_Min;
	//int SCapDifferTest_Max;
	//BYTE SCapDifferTest_CbLevel;
	int SCapRawDataTest_OFF_Min;
	int SCapRawDataTest_OFF_Max;
	int SCapRawDataTest_ON_Min;
	int SCapRawDataTest_ON_Max;
	bool SCapRawDataTest_LetTx_Disable;
	BYTE SCapRawDataTest_SetWaterproof_OFF;
	BYTE SCapRawDataTest_SetWaterproof_ON;
	bool bChannelTestMapping;
	bool bChannelTestNoMapping;
	BYTE ChannelNumTest_TxNum;
	BYTE ChannelNumTest_RxNum;
	BYTE ChannelNumTest_TxNpNum;
	BYTE ChannelNumTest_RxNpNum;
	BYTE ResetPinTest_RegAddr;
	BYTE IntPinTest_RegAddr;
	BYTE IntPinTest_TestNum;
	int NoiseTest_Max;
	int GloveNoiseTest_Coefficient;
	int NoiseTest_Frames;
	int NoiseTest_Time;
	BYTE NoiseTest_SampeMode;
	BYTE NoiseTest_NoiseMode;
	BYTE NoiseTest_ShowTip;
	bool bNoiseTest_GloveMode;
	int NoiseTest_RawdataMin;
	unsigned char Set_Frequency;
	bool bNoiseThreshold_Choose;
	int NoiseTest_Threshold;
	int NoiseTest_MinNgFrame;
	//BYTE SCapClbTest_Frame;
	//int SCapClbTest_Max;
	//int RxCrosstalkTest_Min;
	//int RxCrosstalkTest_Max;
	//int RawDataRxDeviationTest_Max;
	//int RawDataUniformityTest_Percent;
	//int RxLinearityTest_Max;
	//int TxLinearityTest_Max;
	//int DifferDataUniformityTest_Percent;
	int WeakShortTest_CG;
	int WeakShortTest_CC;
	//int WeakShortTest_ChannelNum;
	bool Uniformity_CheckTx;
	bool Uniformity_CheckRx;
	bool Uniformity_CheckMinMax;
	int  Uniformity_Tx_Hole;
	int  Uniformity_Rx_Hole;
	int  Uniformity_MinMax_Hole;
    bool CMTest_CheckMin;
	bool CMTest_CheckMax;
	int  CMTest_MinHole;
	int  CMTest_MaxHole;

	int RawdataMarginTest_Min;
	int RawdataMarginTest_Max;

	int PanelDifferTest_Min;
	int PanelDifferTest_Max;

	bool PanelDiffer_UniformityTest_Check_Tx;
	bool PanelDiffer_UniformityTest_Check_Rx;
	bool PanelDiffer_UniformityTest_Check_MinMax;
	int  PanelDiffer_UniformityTest_Tx_Hole;
	int  PanelDiffer_UniformityTest_Rx_Hole;
	int  PanelDiffer_UniformityTest_MinMax_Hole;

	bool SITO_RawdtaUniformityTest_Check_Tx;
	bool SITO_RawdtaUniformityTest_Check_Rx;
	int  SITO_RawdtaUniformityTest_Tx_Hole;
	int  SITO_RawdtaUniformityTest_Rx_Hole;

	bool bPattern00;
	bool bPatternFF;
	bool bPattern55;
	bool bPatternAA;
	bool bPatternBin;

/*[Arima_8100][bozhi_lin] touch aats test LCD noise implement 20170320 begin*/
	int Lcd_Noise_MaxFrame;
	int Lcd_Noise_Conficient;
	int Lcd_Noise_Noise_Mode;
	int Lcd_Noise_MaxNgPoint;
	int Lcd_Noise_FrameNum;

	bool Lcd_Noise_NoiseThresholdMode;
	int Lcd_Noise_NoiseCoefficient;
	int Lcd_Noise_NoiseMax;
	int Lcd_Noise_SetFrequency;

	int Lcd_Noise_Check_Continue_NG;
	int Lcd_Noise_Continue_NG_Frame;
	int Lcd_Noise_Continue_Coefficient;
/*[Arima_8100][bozhi_lin] 20170320 end*/
};
enum enumTestItem_FT5X22
{
	Code_FT5X22_ENTER_FACTORY_MODE,//����IC���ر��Ĳ�����
	Code_FT5X22_DOWNLOAD,//����IC���ر��Ĳ�����
	Code_FT5X22_UPGRADE,//����IC���ر��Ĳ�����
	Code_FT5X22_FACTORY_ID_TEST,
	Code_FT5X22_PROJECT_CODE_TEST,
	Code_FT5X22_FW_VERSION_TEST,
	Code_FT5X22_IC_VERSION_TEST,
	Code_FT5X22_RAWDATA_TEST,
	Code_FT5X22_ADCDETECT_TEST,
	Code_FT5X22_SCAP_CB_TEST,
	Code_FT5X22_SCAP_RAWDATA_TEST,
	Code_FT5X22_CHANNEL_NUM_TEST,
	Code_FT5X22_INT_PIN_TEST,
	Code_FT5X22_RESET_PIN_TEST,
	Code_FT5X22_NOISE_TEST,
	Code_FT5X22_WEAK_SHORT_CIRCUIT_TEST,
	Code_FT5X22_UNIFORMITY_TEST,
	Code_FT5X22_CM_TEST,
	Code_FT5X22_RAWDATA_MARGIN_TEST,
	Code_FT5X22_WRITE_CONFIG,//����IC���ر��Ĳ�����
	Code_FT5X22_PANELDIFFER_TEST,
	Code_FT5X22_PANELDIFFER_UNIFORMITY_TEST,
	Code_FT5X22_LCM_ID_TEST,
	Code_FT5X22_JUDEG_NORMALIZE_TYPE,
	Code_FT5X22_TE_TEST,
	Code_FT5X22_SITO_RAWDATA_UNIFORMITY_TEST,
    	Code_FT5X22_PATTERN_TEST,
/*[Arima_8100][bozhi_lin] touch aats test LCD noise implement 20170320 begin*/
	Code_FT5X22_LCD_NOISE_TEST,
/*[Arima_8100][bozhi_lin] 20170320 end*/
};

extern struct stCfg_FT5X22_TestItem g_stCfg_FT5X22_TestItem;
extern struct stCfg_FT5X22_BasicThreshold g_stCfg_FT5X22_BasicThreshold;

void OnInit_FT5X22_TestItem(char *strIniFile);
void OnInit_FT5X22_BasicThreshold(char *strIniFile);
void SetTestItem_FT5X22(void);

#endif