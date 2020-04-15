################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../platform/radio/rail_lib/plugin/coexistence/common/coexistence.c 

OBJS += \
./platform/radio/rail_lib/plugin/coexistence/common/coexistence.o 

C_DEPS += \
./platform/radio/rail_lib/plugin/coexistence/common/coexistence.d 


# Each subdirectory must supply rules for building sources it contributes
platform/radio/rail_lib/plugin/coexistence/common/coexistence.o: ../platform/radio/rail_lib/plugin/coexistence/common/coexistence.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -std=c99 '-D__HEAP_SIZE=0x1700' '-DENABLE_LOGGING=1' '-DNVM3_DEFAULT_MAX_OBJECT_SIZE=512' '-D__STACK_SIZE=0x1000' '-DHAL_CONFIG=1' '-DMESH_LIB_NATIVE=1' '-DNVM3_DEFAULT_NVM_SIZE=24576' '-DEFR32BG13P632F512GM48=1' -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\Device\SiliconLabs\EFR32BG13P\Include" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\middleware\glib\dmd\ssd2119" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\radio\rail_lib\common" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\emlib\src" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\protocol\bluetooth\bt_mesh\inc" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\Device\SiliconLabs\EFR32BG13P\Source\GCC" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\radio\rail_lib\plugin\coexistence\common" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\protocol\bluetooth\bt_mesh\inc\soc" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\protocol\bluetooth\bt_mesh\src" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\hardware\kit\common\halconfig" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\middleware\glib\glib" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\CMSIS\Include" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\emdrv\sleep\src" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\hardware\kit\common\drivers" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\protocol\bluetooth\ble_stack\inc\soc" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\emdrv\common\inc" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\service\sleeptimer\src" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\emlib\inc" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\emdrv\uartdrv\inc" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\emdrv\sleep\inc" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\protocol\bluetooth\bt_mesh\inc\common" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\middleware\glib" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\emdrv\gpiointerrupt\inc" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\hardware\kit\common\bsp" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\emdrv\nvm3\inc" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\protocol\bluetooth\ble_stack\src\soc" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\radio\rail_lib\plugin\coexistence\hal\efr32" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\middleware\glib\dmd" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\emdrv\nvm3\src" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\radio\rail_lib\chip\efr32\efr32xg1x" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\common\inc" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\service\sleeptimer\inc" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\service\sleeptimer\config" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\hardware\kit\EFR32BG13_BRD4104A\config" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\emdrv\gpiointerrupt\src" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\radio\rail_lib\protocol\ieee802154" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\middleware\glib\dmd\display" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\radio\rail_lib\protocol\ble" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\halconfig\inc\hal-config" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\bootloader\api" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\Device\SiliconLabs\EFR32BG13P\Source" -I"C:\Users\pkshi\SimplicityStudio\v4_workspace\course-project-pshiralagi\platform\radio\rail_lib\plugin" -Os -fno-builtin -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"platform/radio/rail_lib/plugin/coexistence/common/coexistence.d" -MT"platform/radio/rail_lib/plugin/coexistence/common/coexistence.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


