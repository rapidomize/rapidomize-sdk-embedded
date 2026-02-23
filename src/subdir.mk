# Add inputs and outputs from these tool invocations to the build variables 

C_SRCS += \
src/protium.c \
src/objects.c \
src/mgmt.c \
src/util/ringbuf.c \
src/util/debug.c \
src/util/crc32.c    


OBJS += \
$(O_DIR)/src/protium.o \
$(O_DIR)/src/objects.o \
$(O_DIR)/src/mgmt.o \
$(O_DIR)/src/util/ringbuf.o \
$(O_DIR)/src/util/debug.o \
$(O_DIR)/src/util/crc32.o


C_DEPS += \
$(O_DIR)/src/protium.d \
$(O_DIR)/src/objects.d \
$(O_DIR)/src/mgmt.d \
$(O_DIR)/src/util/ringbuf.d \
$(O_DIR)/src/util/debug.d \
$(O_DIR)/src/util/crc32.d

# Each subdirectory must supply rules for building sources it contributes
$(O_DIR)/src/%.o: src/%.c
	@echo 'Building file: $<'
	mkdir -p $(O_DIR)/src/util
	@echo 'Invoking: Cross GCC Compiler'
	$(CROSS_CC_PREFIX)$(CC) $(PR_CFLAGS)  -c  $(PR_INCLUDES)   -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(O_DIR)/src/util/%.o: src/util/%.c
	@echo 'Building file: $<'
	mkdir -p $(O_DIR)/src/util
	@echo 'Invoking: Cross GCC Compiler'
	$(CROSS_CC_PREFIX)$(CC) $(PR_CFLAGS)  -c  $(PR_INCLUDES)   -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '