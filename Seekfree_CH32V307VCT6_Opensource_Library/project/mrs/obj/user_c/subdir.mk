################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Raquel/hht/智能车/十九届智能车气垫船/git/Hovercraft/Seekfree_CH32V307VCT6_Opensource_Library/project/user/src/isr.c \
D:/Raquel/hht/智能车/十九届智能车气垫船/git/Hovercraft/Seekfree_CH32V307VCT6_Opensource_Library/project/user/src/main.c 

OBJS += \
./user_c/isr.o \
./user_c/main.o 

C_DEPS += \
./user_c/isr.d \
./user_c/main.d 


# Each subdirectory must supply rules for building sources it contributes
user_c/isr.o: D:/Raquel/hht/智能车/十九届智能车气垫船/git/Hovercraft/Seekfree_CH32V307VCT6_Opensource_Library/project/user/src/isr.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_components" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
user_c/main.o: D:/Raquel/hht/智能车/十九届智能车气垫船/git/Hovercraft/Seekfree_CH32V307VCT6_Opensource_Library/project/user/src/main.c
	@	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall  -g -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\Libraries\doc" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Core" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Ld" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Peripheral" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\sdk\Startup" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\user\inc" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_common" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_device" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\project\code" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_driver" -I"D:\Raquel\hht\智能车\十九届智能车气垫船\git\Hovercraft\Seekfree_CH32V307VCT6_Opensource_Library\libraries\zf_components" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

