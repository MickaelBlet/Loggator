# Get last dir name for set name of library
# NAME            =   $(shell basename $$PWD)
NAME			=	Loggator
VERSION			=	0.0.0
DVERSION		=	-DVERSION=\"$(VERSION)\"
# Name of library
BIN_NAME		=	$(NAME)
LIB_NAME		=	$(addsuffix .a, $(NAME))

# Mode compilation (debug/release)
MODE			=	debug
# Compilator
CC				=	g++
# Optimisation compilator
OPTI			=	-O2
# c++ version
STD				=	-std=c++1y
# Flags Compilator
FLAG_DEBUG		=	-Wall -Wextra
FLAG_RELEASE	=	-Wall -Wextra -Werror
# debug Flag
DEBUG			=	-ggdb

# std Thread
LIB_EXTERNAL	=	-lpthread
# GoogleTest
LIB_EXT_TEST	=	-lgtest
LIB_EXT_TEST	+=	-lgmock

PROJECT_HOME	=	.

LIB_DEBUG_DIR	=	$(PROJECT_HOME)/lib/debug/
LIB_TEST_DIR	=	$(PROJECT_HOME)/lib/test/
LIB_RELEASE_DIR =	$(PROJECT_HOME)/lib/release/
BIN_DEBUG_DIR   =	$(PROJECT_HOME)/bin/debug/
BIN_TEST_DIR    =	$(PROJECT_HOME)/bin/test/
BIN_RELEASE_DIR	=	$(PROJECT_HOME)/bin/release/
BUILD_DIR		=	$(PROJECT_HOME)/build/
INC_DIR			=	$(PROJECT_HOME)/include/
INC_DIR_EXT		=	$(PROJECT_HOME)/external/
SRC_DIR			=	$(PROJECT_HOME)/src/
SRC_TEST_DIR	=	$(PROJECT_HOME)/src/test/

LIB_MODULE		=
# LIB_MODULE		+=	example.a

# LIB_EXT_MODULE	=	$(INC_DIR_EXT)path/example.a

LIB_DEBUG		=	$(addprefix $(LIB_DEBUG_DIR), $(LIB_MODULE))
LIB_TEST		=	$(addprefix $(LIB_DEBUG_DIR), $(LIB_MODULE))
LIB_RELEASE		=	$(addprefix $(LIB_RELEASE_DIR), $(LIB_MODULE))

OBJ_DEBUG_DIR	=	$(BUILD_DIR)obj_debug/
OBJ_TEST_DIR	=	$(BUILD_DIR)obj_test/
OBJ_RELEASE_DIR	=	$(BUILD_DIR)obj_release/

INC_FIND		=	$(shell find $(INC_DIR) -name "*.h" -o -name "*.hpp" -o -name "*.H" -o -name "*.hxx")
INC_FIND_EXT	=	$(shell find $(INC_DIR_EXT) -name "*.h" -o -name "*.hpp" -o -name "*.H" -o -name "*.hxx")
INC_PATH		=	$(if $(INC_FIND),$(addprefix -I, $(sort $(dir $(INC_FIND)))))
INC_PATH		+=	$(if $(INC_FIND_EXT),$(addprefix -I, $(sort $(dir $(INC_FIND_EXT)))))

SRC_PATH		=	$(shell find $(SRC_DIR) -name "*.cpp" -not -path "$(SRC_TEST_DIR)*" -o -name "*.C" -not -path "$(SRC_TEST_DIR)*" -o -name "*.cxx" -not -path "$(SRC_TEST_DIR)*")

SRC_TEST_PATH	=	$(shell find $(SRC_TEST_DIR) -name "*.cpp" -o -name "*.C" -o -name "*.cxx")

SRC 			=	$(subst $(SRC_DIR),,$(SRC_PATH))

SRC_TEST		=	$(subst $(SRC_TEST_DIR),,$(SRC_TEST_PATH))

OBJ_DEBUG		=	$(addprefix $(OBJ_DEBUG_DIR), $(SRC:.cpp=.o))

OBJ_TEST		=	$(addprefix $(OBJ_TEST_DIR), $(SRC_TEST:.cpp=.o))

OBJ_RELEASE		=	$(addprefix $(OBJ_RELEASE_DIR), $(SRC:.cpp=.o))

#==============================================================================

all: $(MODE)

$(BIN_DEBUG_DIR):
	@mkdir -p $@

$(BIN_RELEASE_DIR):
	@mkdir -p $@

$(BIN_TEST_DIR):
	@mkdir -p $@

$(LIB_DEBUG_DIR):
	@mkdir -p $@

$(LIB_TEST_DIR):
	@mkdir -p $@

$(LIB_RELEASE_DIR):
	@mkdir -p $@

$(OBJ_DEBUG_DIR):
	@mkdir -p $(sort $(dir $(OBJ_DEBUG)))

$(OBJ_RELEASE_DIR):
	@mkdir -p $(sort $(dir $(OBJ_RELEASE)))

$(OBJ_TEST_DIR):
	@mkdir -p $(sort $(dir $(OBJ_TEST)))

$(OBJ_DEBUG_DIR)%.o : $(SRC_DIR)%.cpp | $(OBJ_DEBUG_DIR)
	$(CC) $(DEBUG) $(STD) $(FLAG_DEBUG) $(DVERSION) -MMD -c $< -o $@ -I $(SRC_DIR) $(INC_PATH) 

$(OBJ_TEST_DIR)%.o : $(SRC_TEST_DIR)%.cpp | $(OBJ_TEST_DIR)
	$(CC) $(DEBUG) $(STD) $(FLAG_DEBUG) $(DVERSION) -MMD -c $< -o $@ -I $(SRC_DIR) $(INC_PATH)

$(OBJ_RELEASE_DIR)%.o : $(SRC_DIR)%.cpp | $(OBJ_RELEASE_DIR)
	$(CC) $(OPTI) $(STD) $(FLAG_RELEASE) $(DVERSION) -MMD -c $< -o $@ -I $(SRC_DIR) $(INC_PATH) 

$(LIB_DEBUG_DIR)$(LIB_NAME):	$(OBJ_DEBUG) | $(LIB_DEBUG_DIR)
	ar rc $@ $^

$(LIB_TEST_DIR)$(LIB_NAME):		$(OBJ_DEBUG) | $(LIB_TEST_DIR)
	ar rc $@ $^

$(LIB_RELEASE_DIR)$(LIB_NAME):	$(OBJ_RELEASE) | $(LIB_RELEASE_DIR)
	ar rc $@ $^

$(BIN_DEBUG_DIR)$(BIN_NAME):	$(OBJ_DEBUG) $(LIB_DEBUG) $(LIB_DEBUG_DIR)$(LIB_NAME) | $(BIN_DEBUG_DIR)
	@echo " /---------\\ \n -  DEBUG  - $(BIN_NAME)\n \\---------/ "
	$(CC) $(DEBUG) $(STD) $(FLAG_DEBUG) $(DVERSION) -o $@ $^ $(LIB_DEBUG) $(LIB_EXT_MODULE) $(LIB_EXTERNAL)

$(BIN_TEST_DIR)$(BIN_NAME):		$(OBJ_TEST) $(LIB_TEST) $(LIB_TEST_DIR)$(LIB_NAME) | $(BIN_TEST_DIR)
	@echo " /---------\\ \n -  TEST   - $(BIN_NAME)\n \\---------/ "
	$(CC) $(DEBUG) $(STD) $(FLAG_DEBUG) $(DVERSION) -o $@ $^ $(LIB_TEST) $(LIB_EXT_MODULE) $(LIB_EXT_TEST) $(LIB_EXTERNAL)

$(BIN_RELEASE_DIR)$(BIN_NAME):	$(OBJ_RELEASE) $(LIB_RELEASE) $(LIB_RELEASE_DIR)$(LIB_NAME) | $(BIN_RELEASE_DIR)
	@echo " /---------\\ \n - RELEASE - $(BIN_NAME)\n \\---------/ "
	$(CC) $(OPTI) $(STD) $(FLAG_DEBUG) $(DVERSION) -o $@ $^ $(LIB_RELEASE) $(LIB_EXT_MODULE) $(LIB_EXTERNAL)

debug:
	$(foreach lib, $(LIB_MODULE), $(MAKE) debug -C $(BUILD_DIR)$(shell basename $(lib) .a);)
	$(MAKE) $(BIN_DEBUG_DIR)$(BIN_NAME)

test:
	$(foreach lib, $(LIB_MODULE), $(MAKE) debug -C $(BUILD_DIR)$(shell basename $(lib) .a);)
	$(MAKE) $(BIN_TEST_DIR)$(BIN_NAME)

release:
	$(foreach lib, $(LIB_MODULE), $(MAKE) release -C $(BUILD_DIR)$(shell basename $(lib) .a);)
	$(MAKE) $(BIN_RELEASE_DIR)$(BIN_NAME)

exe_test:	test
	./$(BIN_TEST_DIR)$(BIN_NAME)

clean:
	/bin/rm -rf $(OBJ_DEBUG_DIR)
	/bin/rm -rf $(OBJ_TEST_DIR)
	/bin/rm -rf $(OBJ_RELEASE_DIR)

fclean:		clean
	/bin/rm -rf $(LIB_DEBUG_DIR)$(LIB_NAME)
	/bin/rm -rf $(BIN_DEBUG_DIR)$(BIN_NAME)
	/bin/rm -rf $(LIB_TEST_DIR)$(LIB_NAME)
	/bin/rm -rf $(BIN_TEST_DIR)$(BIN_NAME)
	/bin/rm -rf $(LIB_RELEASE_DIR)$(LIB_NAME)
	/bin/rm -rf $(BIN_RELEASE_DIR)$(BIN_NAME)

re:			fclean
	$(MAKE) all

.PHONY:		all debug release test exe_test clean fclean re

-include $(OBJ_DEBUG:.o=.d)
-include $(OBJ_TEST:.o=.d)
-include $(OBJ_RELEASE:.o=.d)