################################################################################
# MRS Version: {"version":"1.8.4","date":"2023/02/015"}
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/the\ boat/无线图传/CH32V307VCT6/CH32V307VCT6/libraries/zf_components/seekfree_assistant.c \
E:/the\ boat/无线图传/CH32V307VCT6/CH32V307VCT6/libraries/zf_components/seekfree_assistant_interface.c 

OBJS += \
./zf_components/seekfree_assistant.o \
./zf_components/seekfree_assistant_interface.o 

C_DEPS += \
./zf_components/seekfree_assistant.d \
./zf_components/seekfree_assistant_interface.d 


# Each subdirectory must supply rules for building sources it contributes
zf_components/seekfree_assistant.o: E:/the\ boat/无线图传/CH32V307VCT6/CH32V307VCT6/libraries/zf_components/seekfree_assistant.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -I"E:\the boat\无线图传\CH32V307VCT6\CH32V307VCT6\libraries\zf_components" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
zf_components/seekfree_assistant_interface.o: E:/the\ boat/无线图传/CH32V307VCT6/CH32V307VCT6/libraries/zf_components/seekfree_assistant_interface.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"E:\the boat\The_hovercraft\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -I"E:\the boat\无线图传\CH32V307VCT6\CH32V307VCT6\libraries\zf_components" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

