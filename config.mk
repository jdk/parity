# Output Name
TARGET := parity

# Sources
SOURCES := src/main.c

# Libraries
#LIBRARIES := usbClay

# Options
OPTIONS :=

# Default Build Host
HOST 	:= osx

# Configurations (the first one is the default)
CONFIGS := release debug

# Configuration Specific Options
RELEASE_OPTIONS := CONFIG_RELEASE

DEBUG_OPTIONS := CONFIG_DEBUG DEBUG_PRINTS
