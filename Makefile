##
## Author: Mickaël BLET
##

VERSION				=	1.2.0

NAME				=	loggator
BINARY_NAME			=	loggator
LIBRARY_NAME		=	loggator.a
BINARY_DIRECTORY	=	./bin/
LIBRARY_DIRECTORY	=	./lib/
SOURCE_DIRECTORY	=	./src/
TEST_DIRECTORY		=	./test/
INCLUDE_DIRECTORY	=	../../include/ ./include/
OBJECT_DIRECTORY	=	./obj/
SOURCE_EXTENTION	=	.cpp

BINARY_EXCLUDE_SOURCE	=
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

PRE_DEBUG::

PRE_RELEASE::

PHONY: