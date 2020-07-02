#drivers/input/touchscreen/mediatek/focaltech_ts/focaltech_test
#
#Makefile for test module
#
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/$(CONFIG_TOUCHSCREEN_MTK_FTS_DIRECTORY)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
ccflags-y += -I$(srctree)/drivers/base
	
#[Arima_8100][allen_yu] FT5436 driver AATS test implement 20170606 begin
obj-y += src/
obj-y += supported_ic/
#[Arima_8100][allen_yu] 20170606 end
