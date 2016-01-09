CLOCKS_PANEL_DIR = hb_controller
DIRS		 = $(CLOCKS_PANEL_DIR)
BUILDDIRS	 = $(DIRS:%=build-%)
CLEANDIRS	 = $(DIRS:%=clean-%)

all: $(BUILDDIRS)

$(BUILDDIRS):
	make -C $(@:build-%=%)

clean: $(CLEANDIRS)
$(CLEANDIRS):
	make -C $(@:clean-%=%) clean

flash_clocks_panel:
	make -C $(CLOCKS_PANEL_DIR) flash

.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(CLEANDIRS)
