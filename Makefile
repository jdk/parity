# Custom Functions
CONVERTSRCEXT = $(patsubst %.S,%.$1,$(patsubst %.c,%.$1,$2))
TOUPPER = $(shell echo $1 | \
	  sed 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/')
USCORESUB = $(shell echo $1 | sed 'y/ -./___/')

# For Cross Compilation
sinclude cross_make.mk

# Set default flags
CPPFLAGS := -Iinclude
CFLAGS   := -Wall -g #-O2 #-fPIC
ASFLAGS  :=
#LDFLAGS  := $(USBFLAGS)
#LDLIBS   := $(USBLIBS)

# Tools
CC 		:= $(PREFIX)gcc
LD 		:= $(PREFIX)ld
AS 		:= $(PREFIX)as
AR 		:= $(PREFIX)ar
OBJCOPY := $(PREFIX)objcopy
SIZE 	:= $(PREFIX)size

include config.mk # include after default flags so config.mk can append to them

# Add all the libraries defined in config.mk to LDLIBS
LDLIBS := $(LDLIBS) $(addprefix -l,$(LIBRARIES))

# Verbose Option
ifeq ($(VERBOSE),1)
	Q :=
else
	Q := @
endif

# Configuration
ifeq ($(words $(CONFIGS)),0)
$(error Must specify at least one config in config.mk)
endif
ifeq ($(CONFIG),)
CONFIG := $(word 1,$(CONFIGS))
endif
ifeq ($(findstring $(CONFIG),$(CONFIGS)),)
$(error Invalid config specified)
endif
OPTIONS += $($(call TOUPPER,$(CONFIG))_OPTIONS)

# Machine Name and Tool Versions
MACHINE := $(call USCORESUB,$(shell uname -sm))
CCNAME := $(call USCORESUB,$(notdir $(realpath $(shell which $(CC)))))

# Build Directory
#BUILDDIR := buildresults/$(MACHINE)/$(CCNAME)/$(CONFIG)
BUILDDIR := buildresults/$(HOST)/$(CONFIG)

# Add in the options
CPPFLAGS += $(addprefix -D,$(OPTIONS))

# Include the dependencies
ifneq ($(MAKECMDGOALS),clean)
sinclude $(addprefix $(BUILDDIR)/,$(call CONVERTSRCEXT,d,$(SOURCES)))
endif

### Main Rule ###
.DEFAULT_GOAL 	:= $(BUILDDIR)/$(TARGET)
install 		:  $(BUILDDIR)/$(TARGET)
all		 		:  $(BUILDDIR)/$(TARGET) install

$(BUILDDIR)/$(TARGET) : \
		$(addprefix $(BUILDDIR)/,$(call CONVERTSRCEXT,o,$(SOURCES)))
	@echo "[LINK]      $@"
	@echo "[CONFIG]    $(CONFIG)"
	@mkdir -p $(@D)
	$(Q)$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

################################################################################
### Auto Dependency Rules ###
$(BUILDDIR)/%.d : %.c
	@mkdir -p $(@D)
	@rm -f $@ && \
	$(CC) -MM $(CPPFLAGS) $< | \
	sed 's,\($(*F)\)\.o[ ]*:[ ]*,$(@D)/\1.o $@ : ,g' > $@

$(BUILDDIR)/%.d : %.S
	@mkdir -p $(@D)
	@rm -f $@ && \
	$(CC) -MM $(CPPFLAGS) $< | \
	sed 's,\($(*F)\)\.o[ ]*:[ ]*,$(@D)/\1.o $@ : ,g' > $@
################################################################################

################################################################################
### Implicit Rules ###
$(BUILDDIR)/%.o : %.c
	@mkdir -p $(@D)
	@echo "[CC]        $<"
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.o : %.S
	@mkdir -p $(@D)
	@echo "[AS]        $<"
	$(Q)$(CC) $(CPPFLAGS) -c -o $@ $<
################################################################################

### Utility Rules ###
.PHONY : clean
clean :
	-rm -rf buildresults

.PHONY : install
install :
	$(Q)sudo cp buildresults/osx/release/$(TARGET) /usr/bin
	@echo "[CP]        /usr/bin/$(TARGET)"


