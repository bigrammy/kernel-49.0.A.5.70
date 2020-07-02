#
# Makefile for all kinds of Fingerprint
#

#[Kent][FPS][17080801][Begin]read fps ID pin driver
obj-y     += fps_variant.o
#[Kent][FPS][17080801][end]read fps ID pin driver

obj-$(CONFIG_GOODIX_FINGERPRINT) += goodix/
obj-$(CONFIG_FPC_FINGERPRINT) += fpc/
#[Kent][FPS][17042101][Begin]add elan fps driver
obj-$(CONFIG_ELAN_FINGERPRINT) += elan/
#[Kent][FPS][17042101][End]add elan fps driver
#[Kent][FPS][17051801][Begin]add egis fps driver
obj-$(CONFIG_EGIS_FINGERPRINT) += egis/
#[Kent][FPS][17051801][End]add egis fps driver
