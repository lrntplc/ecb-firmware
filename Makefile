DIRS		= clocks_panel
BUILDDIRS	= $(DIRS:%=build-%)
CLEANDIRS	= $(DIRS:%=clean-%)

all: $(BUILDDIRS)

$(BUILDDIRS):
	make -C $(@:build-%=%)

clean: $(CLEANDIRS)
$(CLEANDIRS):
	make -C $(@:clean-%=%) clean

.PHONY: subdirs $(BUILDDIRS)
.PHONY: subdirs $(CLEANDIRS)
