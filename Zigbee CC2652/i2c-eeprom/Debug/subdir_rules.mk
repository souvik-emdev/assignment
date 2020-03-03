################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/VB_Shared/Zigbee_CC2652_Resources/workspace_v9/empty_CC26X2R1_LAUNCHXL_tirtos_ccs" --include_path="D:/VB_Shared/Zigbee_CC2652_Resources/workspace_v9/empty_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_3_40_00_02/source" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_3_40_00_02/source/ti/posix/ccs" --include_path="C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/include" --define=DeviceFamily_CC26X2 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="D:/VB_Shared/Zigbee_CC2652_Resources/workspace_v9/empty_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-445064383:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-445064383-inproc

build-445064383-inproc: ../empty.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs930/ccs/utils/sysconfig/sysconfig_cli.bat" -s "C:/ti/simplelink_cc13x2_26x2_sdk_3_40_00_02/.metadata/product.json" -o "syscfg" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_devices_config.c: build-445064383 ../empty.syscfg
syscfg/ti_drivers_config.c: build-445064383
syscfg/ti_drivers_config.h: build-445064383
syscfg/syscfg_c.rov.xs: build-445064383
syscfg/: build-445064383

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/VB_Shared/Zigbee_CC2652_Resources/workspace_v9/empty_CC26X2R1_LAUNCHXL_tirtos_ccs" --include_path="D:/VB_Shared/Zigbee_CC2652_Resources/workspace_v9/empty_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_3_40_00_02/source" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_3_40_00_02/source/ti/posix/ccs" --include_path="C:/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/include" --define=DeviceFamily_CC26X2 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="D:/VB_Shared/Zigbee_CC2652_Resources/workspace_v9/empty_CC26X2R1_LAUNCHXL_tirtos_ccs/Debug/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


