# Sample makefile for project
# Builds "myprogram.bin" from myprogram.c (edit PROGRAM to change)
# Additional source file(s) mymodule.c (edit SOURCES to change)
# Link against your libmango + reference libmango (edit LDLIBS, LDFLAGS to change)

PROGRAM = game.bin #test_mpu.bin  # game.bin #  #  #game.bin   
TEST_MPU_PROGRAM = test_mpu.bin

SOURCES = $(PROGRAM:.bin=.c) mpu6050.c i2c.c math_float.c game_graphics.c random.c

all: $(PROGRAM)

# Flags for compile and link
ARCH 	= -march=rv64im -mabi=lp64
ASFLAGS = $(ARCH)
CFLAGS 	= $(ARCH) -g -Og -I$$CS107E/include $$warn $$freestanding -fno-omit-frame-pointer -fstack-protector-strong
LDFLAGS = -nostdlib -L$$CS107E/lib -T memmap.ld
LDLIBS 	= -lmango -lmango_gcc

OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))

# Rules and recipes for all build steps

# Extract raw binary from elf executable
%.bin: %.elf
	riscv64-unknown-elf-objcopy $< -O binary $@

# Link program executable from all common objects
%.elf: $(OBJECTS) libmymango.a
	riscv64-unknown-elf-gcc $(LDFLAGS) $^ $(LDLIBS) -o $@

# Compile C source to object file
%.o: %.c
	riscv64-unknown-elf-gcc $(CFLAGS) -c $< -o $@

# Assemble asm source to object file
%.o: %.s
	riscv64-unknown-elf-as $(ASFLAGS) $< -o $@

# Build and run the application binary
run: $(PROGRAM)
	mango-run $<

# Test the mpu functions
#test_mpu: $(TEST_MPU_PROGRAM)
#	mango_run $<

# Remove all build products
clean:
	rm -f *.o *.bin *.elf *.list *~

# this rule will provide better error message when
# a source file cannot be found (missing, misnamed)
$(SOURCES):
	$(error cannot find source file `$@` needed for build)

libmymango.a:
	$(error cannot find libmymango.a Change to mylib directory to build, then copy here)

.PHONY: all clean run
.PRECIOUS: %.elf %.o

# disable built-in rules (they are not used)
.SUFFIXES:

export warn = -Wall -Wpointer-arith -Wwrite-strings -Werror \
              -Wno-error=unused-function -Wno-error=unused-variable \
              -fno-diagnostics-show-option
export freestanding = -ffreestanding -nostdinc \
                      -isystem $(shell riscv64-unknown-elf-gcc -print-file-name=include)
