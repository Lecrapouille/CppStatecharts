###############################################################################
## UML statechart framework (https://github.com/Lecrapouille/CppStatecharts)
## Copyright (c) 2026 Quentin Quadrat (lecrapouille@gmail.com)
###############################################################################

P := ../../..
M := $(P)/.makefile

include $(P)/Makefile.common
TARGET_NAME := RichMan
TARGET_DESCRIPTION := RichMan example: collecting coins
include $(M)/project/Makefile

CURRENT_DIR := $(P)/doc/examples/RichMan
LIBRARY_DIR := $(P)/src

INCLUDES += $(CURRENT_DIR) $(P)/include
VPATH += $(CURRENT_DIR) $(LIBRARY_DIR)
INTERNAL_LIBS += $(call internal-lib,statechart)

SRC_FILES = $(CURRENT_DIR)/richman.cpp

include $(M)/rules/Makefile
