################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Raquel/hht/智能车/十九届智能车气垫船/git/Hovercraft/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_components/seekfree_assistant.c \
D:/Raquel/hht/智能车/十九届智能车气垫船/git/Hovercraft/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_components/seekfree_assistant_interface.c 

OBJS += \
./zf_components/seekfree_assistant.o \
./zf_components/seekfree_assistant_interface.o 

C_DEPS += \
./zf_components/seekfree_assistant.d \
./zf_components/seekfree_assistant_interface.d 


# Each subdirectory must supply rules for building sources it contributes
zf_components/seekfree_assistant.o: D:/Raquel/hht/智能车/十九届智能车气垫船/git/Hovercraft/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_components/seekfree_assistant.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_components" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
zf_components/seekfree_assistant_interface.o: D:/Raquel/hht/智能车/十九届智能车气垫船/git/Hovercraft/Seekfree_CH32V307VCT6_Opensource_Library/libraries/zf_components/seekfree_assistant_interface.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_components" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

