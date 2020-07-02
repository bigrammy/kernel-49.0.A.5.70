#drivers/input/touchscreen/mediatek/focaltech_ts/focaltech_test/src
#
# Makefile for test module
#
# Linux driver folder

ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/$(CONFIG_TOUCHSCREEN_MTK_FTS_DIRECTORY)
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
ccflags-y += -I$(srctree)/drivers/base

#[Arima_8100][allen_yu] FT5436 driver AATS test implement 20170606 begin
obj-y += focaltech_test.o
obj-y += focaltech_test_main.o
obj-y += focaltech_test_ini.o
obj-y += focaltech_test_global.o
obj-y += focaltech_test_detail_threshold.o
obj-y += focaltech_ic_table.o
#[Arima_8100][allen_yu] 20170606 end