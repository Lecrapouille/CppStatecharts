###############################################################################
## UML statechart framework (https://github.com/Lecrapouille/CppStatecharts)
## Copyright (c) 2026 Quentin Quadrat (lecrapouille@gmail.com)
###############################################################################

###############################################################################
# Location of the project directory and Makefiles
#
P := ..
M := $(P)/.makefile

###############################################################################
# Project definition: build libstatechart.a / libstatechart.so
#
include $(P)/Makefile.common
TARGET_NAME := statechart
TARGET_DESCRIPTION := UML statechart framework library
include $(M)/project/Makefile

###############################################################################
# Library sources
#
VPATH += $(P)/src
INCLUDES += $(P)/include
LIB_FILES := $(wildcard $(P)/src/*.cpp)

include $(M)/rules/Makefile
