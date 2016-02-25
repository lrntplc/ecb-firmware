PRG		= cmd_controller
OUTDIR		= ../out
MCUTARGET	= atmega8
MCUFREQ		= 8000000

CC		= avr-gcc
LD		= avr-ld
OBJCOPY		= avr-objcopy
SIZE		= avr-size

INCLUDEDIR	= ../include
CFLAGS		= -Wall -Os -DF_CPU=$(MCUFREQ) -DI2C_ADDRESS=0x13 -mmcu=$(MCUTARGET) -I$(INCLUDEDIR) -MMD -MP -ffunction-sections -fdata-sections
LDFLAGS	 	= -Wl,--gc-sections

OBJDIR		= $(OUTDIR)/$(PRG)

SRCDIR		= .
SRCS		= $(shell find $(SRCDIR) -type f -name "*.c")
OBJS		= $(patsubst $(SRCDIR)/%, $(OBJDIR)/%, $(SRCS:.c=.o))

COMMONDIR	= ../common
COMMONS		= i2c_slave.c util.c i2c_handlers.c
OBJS		+= $(addprefix $(OBJDIR)/,$(COMMONS:.c=.o))

vpath %.c $(SRCDIR) $(COMMONDIR)

.PHONY: all
all: hex

$(OUTDIR)/%.hex: $(OBJDIR)/$(PRG).elf
	$(OBJCOPY) -O ihex $< $@

$(OBJDIR)/%.elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(SIZE) -t $@

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: hex
hex: $(OUTDIR)/$(PRG).hex

.PHONY: flash
flash: $(OUTDIR)/$(PRG).hex
	avrprog2 -m ../avrprog2_xmls/atmega8.xml -f $(MCUFREQ) --flash w:$^

.PHONY: clean
clean:
	@rm -rf $(OBJDIR) $(OUTDIR)/*.hex

-include $(OBJDIR)/*.d
