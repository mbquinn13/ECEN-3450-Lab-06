################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../convenience.c \
../explore.c \
../ir_behaviors.c \
../main.c \
../pr_behaviors.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
convenience.o \
explore.o \
ir_behaviors.o \
main.o \
pr_behaviors.o

OBJS_AS_ARGS +=  \
convenience.o \
explore.o \
ir_behaviors.o \
main.o \
pr_behaviors.o

C_DEPS +=  \
convenience.d \
explore.d \
ir_behaviors.d \
main.d \
pr_behaviors.d

C_DEPS_AS_ARGS +=  \
convenience.d \
explore.d \
ir_behaviors.d \
main.d \
pr_behaviors.d

OUTPUT_FILE_PATH +=ECEN_3450-Lab06-Quinn_Peterson.elf

OUTPUT_FILE_PATH_AS_ARGS +=ECEN_3450-Lab06-Quinn_Peterson.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler











./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 5.4.0
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DF_CPU=20000000UL -DDEBUG -DDEBUG -DDEBUG -DDEBUG -DDEBUG  -I"../../../capi324v22x-v2.06.000R/lib-includes" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.2.132\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega324p -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.2.132\gcc\dev\atmega324p" -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 5.4.0
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="ECEN_3450-Lab06-Quinn_Peterson.map" -Wl,-u,vfprintf -Wl,--start-group -Wl,-lcapi324v22x -Wl,-lprintf_flt -Wl,-lm  -Wl,--end-group -Wl,-L"../../../capi324v22x-v2.06.000R"  -Wl,--gc-sections -mmcu=atmega324p -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.2.132\gcc\dev\atmega324p"  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "ECEN_3450-Lab06-Quinn_Peterson.elf" "ECEN_3450-Lab06-Quinn_Peterson.hex"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "ECEN_3450-Lab06-Quinn_Peterson.elf" "ECEN_3450-Lab06-Quinn_Peterson.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "ECEN_3450-Lab06-Quinn_Peterson.elf" > "ECEN_3450-Lab06-Quinn_Peterson.lss"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "ECEN_3450-Lab06-Quinn_Peterson.elf" "ECEN_3450-Lab06-Quinn_Peterson.srec"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "ECEN_3450-Lab06-Quinn_Peterson.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "ECEN_3450-Lab06-Quinn_Peterson.elf" "ECEN_3450-Lab06-Quinn_Peterson.a" "ECEN_3450-Lab06-Quinn_Peterson.hex" "ECEN_3450-Lab06-Quinn_Peterson.lss" "ECEN_3450-Lab06-Quinn_Peterson.eep" "ECEN_3450-Lab06-Quinn_Peterson.map" "ECEN_3450-Lab06-Quinn_Peterson.srec" "ECEN_3450-Lab06-Quinn_Peterson.usersignatures"
	