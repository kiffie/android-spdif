#
# Makefile for Android SPDIF output
# MCU: PIC32MX2xx
#

CC=pic32-gcc
BIN2HEX = pic32-bin2hex
OBJDUMP = pic32-objdump

#CC = xc32-gcc
#BIN2HEX = xc32-bin2hex
#OBJDUMP = xc32-objdump

PRG = android-spdif

#PROC = 32MX270F256D
PROC = 32MX250F128B
#PROC = 32MX470F512H

CFLAGS = -Wall -g -mprocessor=$(PROC) -O2 -DSYS_CLOCK=40000000 \
         -Iinclude -IMicrochip/Include


LDFLAGS = $(CFLAGS) -Wl,-Map=$(PRG).map,--memorysummary,$(PRG).mem,--defsym=_min_heap_size=4096

OBJECTS = main.o pic32_config.o rc5_receiver.o spdif_out.o spdif-encoder.o usb_config.o \
          mips_irq.o terminal.o term_printf.o timer.o\
          usb_host.o usb_host_android.o usb_host_audio_v1.o


DEPS    = $(patsubst %.o, %.d, $(notdir $(OBJECTS)))

VPATH 	:= Microchip/USB

.PHONY: all
all: $(PRG).hex lst

%.o: %.c
	$(CC) -c $(CFLAGS) -MMD -o $@ $<

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@


$(PRG).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^
	
$(PRG).hex: $(PRG).elf
	$(BIN2HEX) $<

lst: $(PRG).lst

clean:
	rm -f $(OBJECTS) $(DEPS) $(PRG).elf \
	      $(PRG).map $(PRG).map $(PRG).hex $(PRG).lst $(PRG).mem

-include $(DEPS)
