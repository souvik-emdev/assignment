################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../CC26X2R1_LAUNCHXL_TIRTOS.cmd 

SYSCFG_SRCS += \
../pwmled2.syscfg 

C_SRCS += \
../CC26X2R1_LAUNCHXL_fxns.c \
../main_tirtos.c \
../pwmled2.c \
./syscfg/ti_devices_config.c \
./syscfg/ti_drivers_config.c 

GEN_FILES += \
./syscfg/ti_devices_config.c \
./syscfg/ti_drivers_config.c 

GEN_MISC_DIRS += \
./syscfg/ 

C_DEPS += \
./CC26X2R1_LAUNCHXL_fxns.d \
./main_tirtos.d \
./pwmled2.d \
./syscfg/ti_devices_config.d \
./syscfg/ti_drivers_config.d 

OBJS += \
./CC26X2R1_LAUNCHXL_fxns.obj \
./main_tirtos.obj \
./pwmled2.obj \
./syscfg/ti_devices_config.obj \
./syscfg/ti_drivers_config.obj 

GEN_MISC_FILES += \
./syscfg/ti_drivers_config.h \
./syscfg/syscfg_c.rov.xs 

GEN_MISC_DIRS__QUOTED += \
"syscfg\" 

OBJS__QUOTED += \
"CC26X2R1_LAUNCHXL_fxns.obj" \
"main_tirtos.obj" \
"pwmled2.obj" \
"syscfg\ti_devices_config.obj" \
"syscfg\ti_drivers_config.obj" 

GEN_MISC_FILES__QUOTED += \
"syscfg\ti_drivers_config.h" \
"syscfg\syscfg_c.rov.xs" 

C_DEPS__QUOTED += \
"CC26X2R1_LAUNCHXL_fxns.d" \
"main_tirtos.d" \
"pwmled2.d" \
"syscfg\ti_devices_config.d" \
"syscfg\ti_drivers_config.d" 

GEN_FILES__QUOTED += \
"syscfg\ti_devices_config.c" \
"syscfg\ti_drivers_config.c" 

C_SRCS__QUOTED += \
"../CC26X2R1_LAUNCHXL_fxns.c" \
"../main_tirtos.c" \
"../pwmled2.c" \
"./syscfg/ti_devices_config.c" \
"./syscfg/ti_drivers_config.c" 

SYSCFG_SRCS__QUOTED += \
"../pwmled2.syscfg" 


