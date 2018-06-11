##
## Author: Mickael Blet
## 
##

#------------------------------------------------------------------------------
# Can be modified out template

# Project directory
HOME_DIR		:=	$(if $(HOME_DIR),$(HOME_DIR),./)
# Local library dependency
LOCAL_LIB		:=	$(if $(LOCAL_LIB),$(LOCAL_LIB),-lpthread)
# Library dependency module
MODULE_LIB		:=	$(if $(MODULE_LIB),$(MODULE_LIB),)
# External library dependency
EXTERNAL_LIB	:=	$(if $(EXTERNAL_LIB),$(EXTERNAL_LIB),)

#------------------------------------------------------------------------------

# Get last dir name for set name of library
NAME			:=	$(notdir $(CURDIR))
# Name of bynary
BIN_NAME		=	$(NAME)
# Name of library
LIB_NAME		=	$(addsuffix .a, $(NAME))

# Compilator
CC				=	g++
# c++ version
STD				=	-std=c++11
# Flags Compilator
FLAG_DEBUG		=	-ggdb -Wall -Wextra
FLAG_RELEASE	=	-O2   -Wall -Wextra -Werror

# GoogleTest
TEST_LOCAL_LIB	=	-lgtest
TEST_LOCAL_LIB	+=	-lgtest_main
TEST_LOCAL_LIB	+=	-lgmock
TEST_LOCAL_LIB	+=	-lpthread

#------------------------------------------------------------------------------

# Lib directory
LIB_DIR			=	$(addprefix $(HOME_DIR), lib/)
DEBUG_LIB_DIR	=	$(addprefix $(LIB_DIR), debug/)
RELEASE_LIB_DIR	=	$(addprefix $(LIB_DIR), release/)
TEST_LIB_DIR	=	$(addprefix $(LIB_DIR), test/)

# Bin directory
BIN_DIR			=	$(addprefix $(HOME_DIR), bin/)
DEBUG_BIN_DIR	=	$(addprefix $(BIN_DIR), debug/)
RELEASE_BIN_DIR	=	$(addprefix $(BIN_DIR), release/)
TEST_BIN_DIR	=	$(addprefix $(BIN_DIR), test/)

# Build directory
BUILD_DIR		=	$(addprefix $(HOME_DIR), build/)

# Include directory
INCLUDE_DIR		=	$(addprefix $(HOME_DIR), include/)

# Src directory
SRC_DIR			=	$(addprefix $(HOME_DIR), src/)

# Test directory
TEST_DIR		=	$(addprefix $(HOME_DIR), test/)

#------------------------------------------------------------------------------

# Module sub-directory
MODULE_SUBDIR	:=	$(subst $(abspath $(CURDIR)/$(BUILD_DIR))/,,$(abspath $(CURDIR)/..)/)

# Include directory
M_INCLUDE_DIR	=	$(addsuffix $(MODULE_SUBDIR)$(NAME)/, $(INCLUDE_DIR))
# Src directory
M_SRC_DIR		=	$(addsuffix $(MODULE_SUBDIR)$(NAME)/, $(SRC_DIR))
# Test directory
M_TEST_DIR		=	$(addsuffix $(MODULE_SUBDIR)$(NAME)/, $(TEST_DIR))

# Obj compile directory
OBJ_DIR			=	./obj/

OBJ_DEBUG_DIR	=	$(addsuffix debug/, $(OBJ_DIR))
OBJ_RELEASE_DIR	=	$(addsuffix release/, $(OBJ_DIR))
OBJ_TEST_DIR	=	$(addsuffix test/, $(OBJ_DIR))

#------------------------------------------------------------------------------

# Module library directory
M_DEBUG_LIB_DIR			=	$(addsuffix $(MODULE_SUBDIR), $(DEBUG_LIB_DIR))
M_RELEASE_LIB_DIR		=	$(addsuffix $(MODULE_SUBDIR), $(RELEASE_LIB_DIR))
M_TEST_LIB_DIR			=	$(addsuffix $(MODULE_SUBDIR), $(TEST_LIB_DIR))

# Module binary directory
M_DEBUG_BIN_DIR			=	$(addsuffix $(MODULE_SUBDIR), $(DEBUG_BIN_DIR))
M_RELEASE_BIN_DIR		=	$(addsuffix $(MODULE_SUBDIR), $(RELEASE_BIN_DIR))
M_TEST_BIN_DIR			=	$(addsuffix $(MODULE_SUBDIR), $(TEST_BIN_DIR))

# Library dependency directory
DEBUG_LIB				=	$(addprefix $(DEBUG_LIB_DIR),	$(MODULE_LIB))
RELEASE_LIB				=	$(addprefix $(RELEASE_LIB_DIR),	$(MODULE_LIB))
TEST_LIB				=	$(addprefix $(TEST_LIB_DIR),	$(MODULE_LIB))

# Include path
DEBUG_INCLUDE_PATH		=	$(addprefix -I, $(INCLUDE_DIR) $(M_INCLUDE_DIR) $(M_SRC_DIR) $(dir $(INC)))
RELEASE_INCLUDE_PATH	=	$(addprefix -I, $(INCLUDE_DIR) $(M_INCLUDE_DIR) $(M_SRC_DIR) $(dir $(INC)))
TEST_INCLUDE_PATH		=	$(addprefix -I, $(INCLUDE_DIR) $(M_INCLUDE_DIR) $(M_SRC_DIR) $(dir $(INC)) $(M_TEST_DIR))

# Search all source files in module directory
FIND_SRC_FILE			!=	find $(M_SRC_DIR)	-name "*.cpp"
FIND_SRC_TEST_FILE		!=	find $(M_TEST_DIR)	-name "*.cpp"

# Source paths without prefix module directory
SRC						:=	$(subst $(M_SRC_DIR),,$(if $(SRC),$(SRC),$(FIND_SRC_FILE)))
SRC_TEST				:=	$(subst $(M_TEST_DIR),,$(if $(SRC_TEST),$(SRC_TEST),$(FIND_SRC_TEST_FILE)))
NOT_SRC_LIB				:=	$(subst $(M_SRC_DIR),,$(if $(NOT_SRC_LIB),$(NOT_SRC_LIB),%main.cpp))

# Object file list
OBJ_DEBUG				:=	$(addprefix $(OBJ_DEBUG_DIR),	$(SRC:.cpp=.o))
OBJ_RELEASE				:=	$(addprefix $(OBJ_RELEASE_DIR),	$(SRC:.cpp=.o))
OBJ_TEST				:=	$(addprefix $(OBJ_TEST_DIR),	$(SRC_TEST:.cpp=.o))
OBJ_EXCLUDE_LIB			:=	$(NOT_SRC_LIB:.cpp=.o)

# Object file list without "main.o"
OBJ_DEBUG_LIB			:=	$(addprefix $(OBJ_DEBUG_DIR),	$(filter-out $(OBJ_EXCLUDE_LIB), $(SRC:.cpp=.o)))
OBJ_RELEASE_LIB			:=	$(addprefix $(OBJ_RELEASE_DIR),	$(filter-out $(OBJ_EXCLUDE_LIB), $(SRC:.cpp=.o)))
OBJ_TEST_LIB			:=	$(addprefix $(OBJ_TEST_DIR),	$(filter-out $(OBJ_EXCLUDE_LIB), $(SRC_TEST:.cpp=.o)))

#------------------------------------------------------------------------------

COLOR_SHELL_DEBUG		=	$$(tput setaf 3)
COLOR_SHELL_RELEASE		=	$$(tput setaf 2)
COLOR_SHELL_TEST		=	$$(tput setaf 4)
COLOR_SHELL_RESET		=	$$(tput sgr0)

#------------------------------------------------------------------------------

# Default call
all:			debug

help:
	@echo "all:"
	@echo "lib_debug:"
	@echo "lib_release:"
	@echo "lib_test:"
	@echo "debug:"
	@echo "release:"
	@echo "test:"
	@echo "exe_test:"
	@echo "clean:"
	@echo "fclean:"
	@echo "re:"

# Create library directory
$(M_DEBUG_LIB_DIR) $(M_RELEASE_LIB_DIR) $(M_TEST_LIB_DIR):
	@mkdir -p $@

# Create binary directory
$(M_DEBUG_BIN_DIR) $(M_RELEASE_BIN_DIR) $(M_TEST_BIN_DIR):
	@mkdir -p $@

# Create debug object directory
$(OBJ_DEBUG_DIR):
	@mkdir -p $(sort $(dir $(OBJ_DEBUG)))
# Create release object directory
$(OBJ_RELEASE_DIR):
	@mkdir -p $(sort $(dir $(OBJ_RELEASE)))
# Create test object directory
$(OBJ_TEST_DIR):
	@mkdir -p $(sort $(dir $(OBJ_TEST)))

# Create debug object
$(OBJ_DEBUG_DIR)%.o:	$(M_SRC_DIR)%.cpp | $(OBJ_DEBUG_DIR)
	$(CC) $(STD) $(FLAG_DEBUG) -MMD -c $< -o $@ $(DEBUG_INCLUDE_PATH) 
# Create release object
$(OBJ_RELEASE_DIR)%.o:	$(M_SRC_DIR)%.cpp | $(OBJ_RELEASE_DIR)
	$(CC) $(STD) $(FLAG_RELEASE) -MMD -c $< -o $@ $(RELEASE_INCLUDE_PATH)
# Create test object
$(OBJ_TEST_DIR)%.o:		$(M_TEST_DIR)%.cpp | $(OBJ_TEST_DIR)
	$(CC) $(STD) $(FLAG_DEBUG) -MMD -c $< -o $@ $(TEST_INCLUDE_PATH)

# Call dependency module makefile
$(MODULE_LIB):
	$(MAKE) --no-print-directory $(MODE_LIB) -C $(BUILD_DIR)$(basename $@)

$(DEBUG_LIB):	MODE_LIB = lib_debug
$(DEBUG_LIB):	$(MODULE_LIB)
	@true

$(RELEASE_LIB):	MODE_LIB = lib_release
$(RELEASE_LIB):	$(MODULE_LIB)
	@true

$(TEST_LIB):	MODE_LIB = lib_test
$(TEST_LIB):	$(MODULE_LIB)
	@true

# Create debug library
$(M_DEBUG_LIB_DIR)$(LIB_NAME):		$(OBJ_DEBUG_LIB)	| $(M_DEBUG_LIB_DIR)
	@printf "$(COLOR_SHELL_DEBUG) /---------\\ \n -  DEBUG  - $(COLOR_SHELL_RESET)$(notdir $@)$(COLOR_SHELL_DEBUG)\n \\---------/ $(COLOR_SHELL_RESET)\n"
	ar rc $@ $^
# Create release library
$(M_RELEASE_LIB_DIR)$(LIB_NAME):	$(OBJ_RELEASE_LIB)	| $(M_RELEASE_LIB_DIR)
	@printf "$(COLOR_SHELL_RELEASE) /---------\\ \n - RELEASE - $(COLOR_SHELL_RESET)$(notdir $@)$(COLOR_SHELL_RELEASE)\n \\---------/ $(COLOR_SHELL_RESET)\n"
	ar rc $@ $^
# Create test library
$(M_TEST_LIB_DIR)$(LIB_NAME):		$(OBJ_DEBUG_LIB)	| $(M_TEST_LIB_DIR)
	@printf "$(COLOR_SHELL_TEST) /---------\\ \n -  TEST   - $(COLOR_SHELL_RESET)$(notdir $@)$(COLOR_SHELL_TEST)\n \\---------/ $(COLOR_SHELL_RESET)\n"
	ar rc $@ $^

# Create debug binary
$(M_DEBUG_BIN_DIR)$(BIN_NAME):	$(OBJ_DEBUG)	$(DEBUG_LIB)	|	$(M_DEBUG_BIN_DIR) $(M_DEBUG_LIB_DIR)$(LIB_NAME)
ifndef IS_LIBRARY
	@printf "$(COLOR_SHELL_DEBUG) /---------\\ \n -  DEBUG  - $(COLOR_SHELL_RESET)$(notdir $@)$(COLOR_SHELL_DEBUG)\n \\---------/ $(COLOR_SHELL_RESET)\n"
	$(CC) $(STD) $(FLAG_DEBUG) -o $@ $^ $(DEBUG_INCLUDE_PATH) $(LOCAL_LIB)
endif
# Create release binary
$(M_RELEASE_BIN_DIR)$(BIN_NAME):$(OBJ_RELEASE)	$(RELEASE_LIB)	|	$(M_RELEASE_BIN_DIR) $(M_RELEASE_LIB_DIR)$(LIB_NAME)
ifndef IS_LIBRARY
	@printf "$(COLOR_SHELL_RELEASE) /---------\\ \n - RELEASE - $(COLOR_SHELL_RESET)$(notdir $@)$(COLOR_SHELL_RELEASE)\n \\---------/ $(COLOR_SHELL_RESET)\n"
	$(CC) $(STD) $(FLAG_RELEASE) -o $@ $^ $(RELEASE_INCLUDE_PATH) $(LOCAL_LIB)
endif
# Create test binary
$(M_TEST_BIN_DIR)$(BIN_NAME):	$(OBJ_TEST)		$(DEBUG_LIB)	$(M_TEST_LIB_DIR)$(LIB_NAME)	|	$(M_TEST_BIN_DIR)
	@printf "$(COLOR_SHELL_TEST) /---------\\ \n -  TEST   - $(COLOR_SHELL_RESET)$(notdir $@)$(COLOR_SHELL_TEST)\n \\---------/ $(COLOR_SHELL_RESET)\n"
	$(CC) $(STD) $(FLAG_DEBUG) -o $@ $^ $(TEST_INCLUDE_PATH) $(TEST_LOCAL_LIB) $(LOCAL_LIB)

lib_debug:		$(M_DEBUG_LIB_DIR)$(LIB_NAME)

lib_release:	$(M_RELEASE_LIB_DIR)$(LIB_NAME)

lib_test:		$(M_TEST_LIB_DIR)$(LIB_NAME)

debug:			$(M_DEBUG_BIN_DIR)$(BIN_NAME)

release:		$(M_RELEASE_BIN_DIR)$(BIN_NAME)

test:			$(M_TEST_BIN_DIR)$(BIN_NAME)

exe_test:		test
	$(M_TEST_BIN_DIR)$(BIN_NAME)

clean:
	/bin/rm -rf \
	$(OBJ_DEBUG_DIR) \
	$(OBJ_RELEASE_DIR) \
	$(OBJ_TEST_DIR)

fclean:			clean
	/bin/rm -rf \
	$(M_DEBUG_LIB_DIR)$(LIB_NAME) \
	$(M_RELEASE_LIB_DIR)$(LIB_NAME) \
	$(M_TEST_LIB_DIR)$(LIB_NAME) \
	$(M_DEBUG_BIN_DIR)$(BIN_NAME) \
	$(M_RELEASE_BIN_DIR)$(BIN_NAME) \
	$(M_TEST_BIN_DIR)$(BIN_NAME)

re:				fclean
	$(MAKE) all

.PHONY:			all lib_debug lib_release lib_test debug release test exe_test clean fclean re $(MODULE_LIB)

-include $(OBJ_DEBUG:.o=.d)
-include $(OBJ_RELEASE:.o=.d)
-include $(OBJ_TEST:.o=.d)