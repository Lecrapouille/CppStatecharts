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

###############################################################################
# Location of the project directory and Makefiles
#
P := .
M := $(P)/.makefile

###############################################################################
# Project definition
#
include $(P)/Makefile.common
TARGET_NAME := $(PROJECT_NAME)
TARGET_DESCRIPTION := UML statechart framework for C++
ORCHESTRATOR_MODE := 1

include $(M)/project/Makefile

###################################################
# Internal libs to compile in the correct order
#
INTERNAL_LIBS := $(call internal-lib,statechart)
DIRS_WITH_MAKEFILE := $(P)/src

###################################################
# Generic Makefile rules
#
include $(M)/rules/Makefile

###################################################
# Extra rules: compile after everything
# Application depends on the statechart library
#
EXAMPLES = $(sort $(dir $(wildcard $(P)/doc/examples/*/.)))

.PHONY: examples
examples: $(DIRS_WITH_MAKEFILE)
	@$(call print-from,"Compiling examples",$(PROJECT_NAME),$(EXAMPLES))
	@for i in $(EXAMPLES);     \
	do                          \
		$(MAKE) -C $$i all;     \
	done;

post-build:: examples
