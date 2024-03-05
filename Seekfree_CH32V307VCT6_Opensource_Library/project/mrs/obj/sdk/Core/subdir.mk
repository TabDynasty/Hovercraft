################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/015"}
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/the\ boat/The_hovercraft/Hovercraft/Seekfree_CH32V307VCT6_Opensource_Library/libraries/sdk/Core/core_riscv.c 

OBJS += \
./sdk/Core/core_riscv.o 

C_DEPS += \
./sdk/Core/core_riscv.d 


# Each subdirectory must supply rules for building sources it contributes
sdk/Core/core_riscv.o: E:/the\ boat/The_hovercraft/Hovercraft/Seekfree_CH32V307VCT6_Opensource_Library/libraries/sdk/Core/core_riscv.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_components" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

