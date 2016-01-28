HB_CTRL_DIR	= hb_controller
CMD_CTRL_DIR	= cmd_controller
DIRS		= $(HB_CTRL_DIR) $(CMD_CTRL_DIR)
BUILDDIRS	= $(DIRS:%=build-%)
CLEANDIRS	= $(DIRS:%=clean-%)

all: $(BUILDDIRS)

$(BUILDDIRS):
	make -C $(@:build-%=%)

clean: $(CLEANDIRS)
$(CLEANDIRS):
	make -C $(@:clean-%=%) clean

flash_top_ctrl:
	make -C $(HB_CTRL_DIR) flash_top

flash_bottom_ctrl:
	make -C $(HB_CTRL_DIR) flash_bottom

flash_command_ctrl:
	make -C $(CMD_CTRL_DIR) flash

.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(CLEANDIRS)
