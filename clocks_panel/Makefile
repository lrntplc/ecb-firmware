PRG		= clocks_panel
OUTDIR		= ../out
MCUTARGET	= atmega8
MCUFREQ		= 8000000

CC		= avr-gcc
LD		= avr-ld
OBJCOPY	 	= avr-objcopy

INCLUDEDIR	= ../include
CFLAGS		= -Wall -Os -DF_CPU=$(MCUFREQ) -mmcu=$(MCUTARGET) -I$(INCLUDEDIR) -MMD -MP
LDFLAGS	 	=

COMMONDIR	= ../common
COMMONS		= $(COMMONDIR)/spi.c

SRCDIR		= .
SRCS		= $(shell find $(SRCDIR) -type f -name "*.c")

OBJDIR		= $(OUTDIR)/$(PRG)
OBJS		= $(patsubst $(SRCDIR)/%, $(OBJDIR)/%, $(SRCS:.c=.o))
OBJS		+= $(patsubst $(COMMONDIR)/%, $(OBJDIR)/%, $(COMMONS:.c=.o))

vpath %.c $(SRCDIR) $(COMMONDIR)

.PHONY: all
all: $(OUTDIR)/$(PRG).hex

$(OBJDIR)/$(PRG).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

.PHONY: hex
hex: $(OUTDIR)/$(PRG).hex

$(OUTDIR)/%.hex: $(OBJDIR)/%.elf
	$(OBJCOPY) -O ihex $< $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: flash
flash: $(OUTDIR)/$(PRG).hex
	avrprog2 -m ../avrprog2_xmls/atmega8.xml -f 8000000 --flash w:$^

.PHONY: clean
clean:
	@rm -rf $(OBJDIR) $(OUTDIR)/$(PRG).hex

-include $(OBJDIR)/*.d
