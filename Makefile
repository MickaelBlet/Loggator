##
## Author: Mickaël BLET
##

#------------------------------------------------------------------------------
# common
#------------------------------------------------------------------------------

# choose your compilation mode at 'make' call
#	modes: (debug, release, lib_debug, lib_release, test)
#	default value: debug
MODE					=	lib_release

# define version of module
#	default value: 0.0.0
VERSION					=	1.2.2

# name of your binary
#	default value: (name of Makefile current directory)
BINARY_NAME				=	loggator

# name of your library
#	default value: (name of Makefile current directory)
LIBRARY_NAME			=	loggator

#------------------------------------------------------------------------------
# directories
#------------------------------------------------------------------------------

# destination path of your binaries (not forget the last '/')
#	default value: bin/
BINARY_DIRECTORY		=	./bin/

# destination path of your libraries (not forget the last '/')
#	default value: lib/
LIBRARY_DIRECTORY		=	./lib/

# source path (not forget the last '/')
#	default value: src/
SOURCE_DIRECTORY		=	./src/

# source test path (not forget the last '/')
#	default value: test/
TEST_DIRECTORY			=	./test/

# include path (not forget the last '/')
#	default value: include/
INCLUDE_DIRECTORY		=	./include/

# object path (not forget the last '/')
#	default value: obj/
OBJECT_DIRECTORY		=	./obj/

#------------------------------------------------------------------------------
# compilation
#------------------------------------------------------------------------------

# extention of source file
#	default value: .c
SOURCE_EXTENTION		=	.cpp

# exclude source for binary
BINARY_EXCLUDE_SOURCE	=	loggator_config.cpp \
							loggator_stream.cpp \
							loggator.cpp

# exclude source for library
LIBRARY_EXCLUDE_SOURCE	=	main.cpp

# exclude source for test
TEST_EXCLUDE_SOURCE		=

# compilation line:
#	$(COMPILER) $(FLAGS) ... $(OPTIONS)

DEBUG_COMPILER			=	$(CXX)
RELEASE_COMPILER		=	$(CXX)
TEST_COMPILER			=	$(CXX)

DEBUG_FLAGS				=	-std=c++11 -Wall -Wextra -ggdb3
RELEASE_FLAGS			=	-std=c++11 -Wall -Wextra -Werror -O2
TEST_FLAGS				=	-std=c++11 -Wall -Wextra -ggdb3

DEBUG_OPTIONS			=	-DVERSION=\"$(VERSION)\" -DLOGGATOR_FORMAT_BUFFER_SIZE=256 \
							-I$(INCLUDE_DIRECTORY)
RELEASE_OPTIONS			=	-DVERSION=\"$(VERSION)\" -DLOGGATOR_FORMAT_BUFFER_SIZE=256 \
							-I$(INCLUDE_DIRECTORY)
TEST_OPTIONS			=	-DVERSION=\"$(VERSION)\" -DLOGGATOR_FORMAT_BUFFER_SIZE=256 \
							-I$(INCLUDE_DIRECTORY) \
							-L$(LIBRARY_DIRECTORY) \
							-l$(LIBRARY_NAME)-debug \
							-lgtest -lgtest_main -lgmock -lpthread

DEBUG_DEPENDENCIES		=	
RELEASE_DEPENDENCIES	=	
TEST_DEPENDENCIES		=	$(LIBRARY_DIRECTORY)lib$(LIBRARY_NAME)-debug.a

include ./module.mk

exe_test:	test
	$(foreach bin,$(BINARIES_TEST),$(bin) || exit;)

PHONY:
