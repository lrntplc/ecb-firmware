PRG		= hb_controller
OUTDIR		= ../out
MCUTARGET	= atmega8
MCUFREQ		= 8000000

CC		= avr-gcc
LD		= avr-ld
OBJCOPY		= avr-objcopy
SIZE		= avr-size

INCLUDEDIR	= ../include
CFLAGS		= -Wall -Os -DF_CPU=$(MCUFREQ) -mmcu=$(MCUTARGET) -I$(INCLUDEDIR) -MMD -MP -ffunction-sections -fdata-sections
CFLAGS_TOP	= -DI2C_ADDRESS=0x11
CFLAGS_BOT	= -DI2C_ADDRESS=0x12
LDFLAGS	 	= -Wl,--gc-sections

OBJDIR		= $(OUTDIR)/$(PRG)
OBJDIR_TOP	= $(OUTDIR)/$(PRG)/top
OBJDIR_BOT	= $(OUTDIR)/$(PRG)/bottom

SRCDIR		= .
SRCS		= $(shell find $(SRCDIR) -type f -name "*.c")
OBJS		= $(patsubst $(SRCDIR)/%, $(OBJDIR)/%, $(SRCS:.c=.o))

COMMONDIR	= ../common
COMMONS		= spi.c i2c_slave.c tmr.c util.c
OBJS		+= $(addprefix $(OBJDIR)/,$(COMMONS:.c=.o))

OBJS_TOP	= $(OBJDIR_TOP)/i2c_handlers.o
OBJS_BOT	= $(OBJDIR_BOT)/i2c_handlers.o

vpath %.c $(SRCDIR) $(COMMONDIR)

.PHONY: all
all: hex

$(OUTDIR)/top_%.hex: $(OBJDIR_TOP)/top_$(PRG).elf
	$(OBJCOPY) -O ihex $< $@

$(OUTDIR)/bot_%.hex: $(OBJDIR_BOT)/bot_$(PRG).elf
	$(OBJCOPY) -O ihex $< $@

$(OBJDIR_TOP)/top_%.elf: $(OBJS) $(OBJS_TOP)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(SIZE) -t $@

$(OBJDIR_BOT)/bot_%.elf: $(OBJS) $(OBJS_BOT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(SIZE) -t $@

$(OBJS): | $(OBJDIR_TOP) $(OBJDIR_BOT)

$(OBJDIR_TOP) $(OBJDIR_BOT):
	mkdir -p $@

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR_TOP)/%.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS_TOP) -c -o $@ $<

$(OBJDIR_BOT)/%.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS_BOT) -c -o $@ $<

.PHONY: hex
hex: $(OUTDIR)/top_$(PRG).hex $(OUTDIR)/bot_$(PRG).hex

.PHONY: flash_top
flash_top: $(OUTDIR)/top_$(PRG).hex
	avrprog2 -m ../avrprog2_xmls/atmega8.xml -f $(MCUFREQ) --flash w:$^

.PHONY: flash_bottom
flash_bottom: $(OUTDIR)/bot_$(PRG).hex
	avrprog2 -m ../avrprog2_xmls/atmega8.xml -f $(MCUFREQ) --flash w:$^

.PHONY: clean
clean:
	@rm -rf $(OBJDIR) $(OUTDIR)/*.hex

-include $(OBJDIR)/*.d
