include $(srctree)/drivers/misc/mediatek/Makefile.custom

# Linux driver folder
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/ft6x36/
#ccflags-y += -I$(srctree)/drivers/misc/mediatek/mach/$(MTK_PLATFORM)/$(ARCH_MTK_PROJECT)/touchpanel/ft6x36/
#ccflags-y += -I$(srctree)/arch/arm/mach-$(MTK_PLATFORM)/$(ARCH_MTK_PROJECT)/touchpanel/ft6x36/


obj-y	+=  focaltech_ex_fun.o
obj-y	+=  focaltech_driver.o
obj-y	+=  focaltech_ctl.o
#obj-y += ft_gesture_lib.a






#include $(MTK_PATH_BUILD)/common.mk

#path := $(if $(filter yes,$(KBUILD_OUTPUT_SUPPORT)),$(srctree)/$(obj),$(obj))
#obj-y := $(patsubst %.c,%.o,$(subst $(path)/,,$(call wildcard2,$(path)/*.c)))
#obj-y += ft_gesture_lib.a