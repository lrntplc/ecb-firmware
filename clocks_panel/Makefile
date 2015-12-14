PRG		= clocks_panel
MCUTARGET	= atmega8
MCUFREQ		= 8000000

CC		= avr-gcc
LD		= avr-ld
OBJCOPY	 	= avr-objcopy

INCLUDE		= -I../include
CFLAGS		= -Wall -Os -DF_CPU=$(MCUFREQ) -mmcu=$(MCUTARGET) $(INCLUDE)
LDFLAGS	 	=

OBJ		= tmr.o chess_clock.o max7219.o clocks_panel.o ../common/spi.o

all: $(PRG).hex

$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

hex: $(PRG).hex

%.hex: %.elf
	$(OBJCOPY) -O ihex $< $@

flash: $(PRG).hex
	avrprog2 -m ../avrprog2_xmls/atmega8.xml -f 8000000 --flash w:$^

clean:
	@rm -f *.hex *.elf *.o
