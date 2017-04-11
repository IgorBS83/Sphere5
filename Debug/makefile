################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv5/tools/compiler/c6000_7.4.4

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./Sources/Algorithms/Tracking.obj" \
"./Sources/Algorithms/Energy.obj" \
"./Sources/Algorithms/Detection.obj" \
"./Sources/Algorithms/Calculations.obj" \
"./Sources/Algorithms/Calc_Connect.obj" \
"./Sources/System/TMR_Init.obj" \
"./Sources/System/Periph_Init.obj" \
"./Sources/System/PLL_Init.obj" \
"./Sources/System/INT_Init.obj" \
"./Sources/System/I2C_Init.obj" \
"./Sources/System/GPIO_Init.obj" \
"./Sources/System/EMIFA_Init.obj" \
"./Sources/System/EDMA3_Init.obj" \
"./Sources/main.obj" \
"./Sources/Protocol.obj" \
"./Sources/BootLoader.obj" \
"./Sources/AllBus.obj" \
"../Sources/C6455.cmd" \
-l"D:\Documents\CCS_Projects\MathFuncs\Release\MathFuncs.lib" \
-l"C:\ti\sprc234(C6455 Chip Support Library (CSL))\6455\default_package\csl_c6455\lib\csl_c6455.lib" \
-l"C:\ti\sprc234(C6455 Chip Support Library (CSL))\6455\default_package\csl_c64xplus_intc\lib\csl_c64xplus_intc.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include Sources/subdir_vars.mk
-include Sources/System/subdir_vars.mk
-include Sources/Algorithms/subdir_vars.mk
-include Sources/subdir_rules.mk
-include Sources/System/subdir_rules.mk
-include Sources/Algorithms/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Sphere5.out

# Tool invocations
Sphere5.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C6000 Linker'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv64+ --abi=coffabi -O2 -g --define=c6455 --display_error_number --diag_warning=225 --diag_wrap=off -z --stack_size=020000 -m"Sphere5.map" --heap_size=0x800 -i"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/lib" -i"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="Sphere5_linkInfo.xml" --rom_model -o "Sphere5.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(C6000_EXECUTABLE_OUTPUTS__QUOTED) "Sphere5.out"
	-$(RM) "Sources\AllBus.pp" "Sources\Protocol.pp" "Sources\main.pp" "Sources\System\EDMA3_Init.pp" "Sources\System\EMIFA_Init.pp" "Sources\System\GPIO_Init.pp" "Sources\System\I2C_Init.pp" "Sources\System\INT_Init.pp" "Sources\System\PLL_Init.pp" "Sources\System\Periph_Init.pp" "Sources\System\TMR_Init.pp" "Sources\Algorithms\Calc_Connect.pp" "Sources\Algorithms\Calculations.pp" "Sources\Algorithms\Detection.pp" "Sources\Algorithms\Energy.pp" "Sources\Algorithms\Tracking.pp" 
	-$(RM) "Sources\AllBus.obj" "Sources\BootLoader.obj" "Sources\Protocol.obj" "Sources\main.obj" "Sources\System\EDMA3_Init.obj" "Sources\System\EMIFA_Init.obj" "Sources\System\GPIO_Init.obj" "Sources\System\I2C_Init.obj" "Sources\System\INT_Init.obj" "Sources\System\PLL_Init.obj" "Sources\System\Periph_Init.obj" "Sources\System\TMR_Init.obj" "Sources\Algorithms\Calc_Connect.obj" "Sources\Algorithms\Calculations.obj" "Sources\Algorithms\Detection.obj" "Sources\Algorithms\Energy.obj" "Sources\Algorithms\Tracking.obj" 
	-$(RM) "Sources\BootLoader.pp" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-@echo 'Create flash image: TI-TXT'
	-"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/hex6x" C6Hex.cmd
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets

