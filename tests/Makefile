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
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
###############################################################################

###################################################
# Location of the project directory and Makefiles
#
P := ..
M := $(P)/.makefile

###################################################
# Project definition
#
include $(P)/Makefile.common
TARGET_NAME := $(PROJECT_NAME)-UnitTest
TARGET_DESCRIPTION := Unit tests for $(PROJECT_NAME)
COMPILATION_MODE = test
include $(M)/project/Makefile

###################################################
# Inform Makefile where to find *.cpp files
#
VPATH += $(P)/tests $(P)/src

###################################################
# Inform Makefile where to find header files
#
INCLUDES += $(P)/tests $(P)/src $(P)/include

###################################################
# Library sources (under src/) + unit test sources
#
SRC_FILES += $(wildcard $(P)/src/*.cpp)
SRC_FILES += $(call rwildcard,$(P)/tests,*.cpp)

###################################################
# Google Test and Google Mock specific warnings
#
USER_CXXFLAGS += -Wno-noexcept

###################################################
# Set Libraries.
#
PKG_LIBS += gmock gtest_main gtest

###################################################
# Generic Makefile rules
#
include $(M)/rules/Makefile