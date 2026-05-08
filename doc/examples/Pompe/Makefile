###############################################################################
## UML statechart framework (https://github.com/Lecrapouille/CppStatecharts)
## Copyright (c) 2026 Quentin Quadrat (lecrapouille@gmail.com)
###############################################################################

P := ../../..
M := $(P)/.makefile

include $(P)/Makefile.common
TARGET_NAME := Pompe
TARGET_DESCRIPTION := Dead-Man pump controller example
include $(M)/project/Makefile

CURRENT_DIR := $(P)/doc/examples/Pompe
LIBRARY_DIR := $(P)/src

INCLUDES += $(CURRENT_DIR) $(P)/include
VPATH += $(CURRENT_DIR) $(LIBRARY_DIR)
INTERNAL_LIBS += $(call internal-lib,statechart)

SRC_FILES = $(CURRENT_DIR)/pompe.cpp

include $(M)/rules/Makefile
