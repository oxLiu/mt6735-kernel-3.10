#
# Makefile for misc devices that really don't fit anywhere else.
#
include $(srctree)/drivers/misc/mediatek/Makefile.custom

obj-y += mt65xx_lcm_list.o

LCM_LISTS := $(subst ",,$(CONFIG_CUSTOM_KERNEL_LCM))
obj-y += $(foreach LCM,$(LCM_LISTS),$(LCM)/)

obj-y	+=  jd9161ba_wvga_dsi_vdo_drv/
obj-y	+=  hx8379c_wvga_dsi_vdo/