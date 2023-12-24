###############################################################################
## UML statechart framework (https://github.com/Lecrapouille/CppStatecharts)
## Copyright (c) 2026 Quentin Quadrat (lecrapouille@gmail.com)
##
## Based on the Java UML statechart framework
## (https://github.com/klangfarbe/UML-Statechart-Framework-for-Java)
## Copyright (C) 2006-2013 Christian Mocek (christian.mocek@gmail.com)
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
###############################################################################

###############################################################################
# Location of the project directory and Makefiles
#
P := ../../..
M := $(P)/.makefile

###############################################################################
# Project definition
#
include $(P)/Makefile.common
TARGET_NAME := GettingStarted
TARGET_DESCRIPTION := Getting started - UML statechart framework for C++
include $(M)/project/Makefile

###############################################################################
# Inform Makefile where to find *.cpp files
CURRENT_DIR := $(P)/doc/examples/GettingStarted
LIBRARY_DIR := $(P)/src/Statechart


###############################################################################
# Inform Makefile where to find header files
#
INCLUDES += $(CURRENT_DIR) $(P)/include

###############################################################################
# Inform Makefile where to find *.cpp files
#
VPATH += $(CURRENT_DIR) $(LIBRARY_DIR)

###################################################
# Set third-party libraries. Order matters: dependencies last
#
INTERNAL_LIBS += $(call internal-lib,statechart)

###############################################################################
# Make the list of files to compile (application sources only)
#
SRC_FILES = $(CURRENT_DIR)/getting-started.cpp

###############################################################################
# Sharable information between all Makefiles
#
include $(M)/rules/Makefile