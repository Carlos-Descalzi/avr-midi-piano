CC=avr-gcc
LD=avr-gcc
MCU=atmega32
F_CPU=16000000L
OPTLEVEL=5
CFLAGS=-mmcu=$(MCU) -std=gnu99 -O$(OPTLEVEL) -DF_CPU=$(F_CPU) -Wall
LDFLAGS=-mmcu=$(MCU) 
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
FORMAT=ihex
OBJS=main.o
#PORT=/dev/ttyS4
PORT=/dev/ttyS5

all: pianito lst

pianito: pianito.hex

lst: pianito.lst

clean:
	rm -rf *.hex *.elf *.o *.lst

%.hex: %.elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

pianito.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o pianito.elf

.o: .c
	$(CC) $(CFLAGS) -c $<

burn: pianito
	avrdude -c ponyser -p $(MCU) -P $(PORT) -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U flash:w:pianito.hex 

