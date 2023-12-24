###############################################################################
## UML statechart framework (https://github.com/Lecrapouille/CppStatecharts)
## Copyright (c) 2026 Quentin Quadrat (lecrapouille@gmail.com)
##
## Sub-project: static/shared library libstatechart.
###############################################################################

P := ..
M := $(P)/.makefile

include $(P)/Makefile.common

TARGET_NAME := statechart
TARGET_DESCRIPTION := UML statechart framework library

include $(M)/project/Makefile

INCLUDES := $(P)/include
# Built via `make -C src`; list *.cpp in this directory only (not src/foo.cpp paths).
LIB_FILES := $(sort $(notdir $(wildcard *.cpp)))

include $(M)/rules/Makefile
