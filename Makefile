##
## Author: Mickaël BLET
##

VERSION				=	1.2.0

NAME				=	loggator
BINARY_NAME			=	loggator
LIBRARY_NAME		=	loggator
BINARY_DIRECTORY	=	./bin/
LIBRARY_DIRECTORY	=	./lib/
SOURCE_DIRECTORY	=	./src/
TEST_DIRECTORY		=	./test/
INCLUDE_DIRECTORY	=	./include/
OBJECT_DIRECTORY	=	./obj/
SOURCE_EXTENTION	=	.cpp

BINARY_EXCLUDE_SOURCE	= loggator_config.cpp loggator_stream.cpp loggator.cpp
LIBRARY_EXCLUDE_SOURCE	= main.cpp
TEST_EXCLUDE_SOURCE		=

COMPILER			=	g++
VARIABLES			=	VERSION=\"$(VERSION)\" LOGGATOR_FORMAT_BUFFER_SIZE=256
COMMON_FLAGS		=	-std=c++11
DEBUG_FLAGS			=	-ggdb -Wall -Wextra
RELEASE_FLAGS		=	-O2 -Wall -Wextra -Werror
TEST_FLAGS			=	-std=c++11 -ggdb -Wall -Wextra

DEBUG_ARCHIVES		=
RELEASE_ARCHIVES	=

# local libraries (example: -lpthread)
DEBUG_LIBRARIES		=
RELEASE_LIBRARIES	=
TEST_LIBRARIES		=

include ./module.mk

exe_test:	test
	$(BINARY_DIRECTORY)$(BINARY_NAME)-test-$(VERSION)

fclean:
	$(RM) -r \
	$(OBJECT_DIRECTORY) \
	$(BINARY_DIRECTORY) \
	$(LIBRARY_DIRECTORY)

PHONY: