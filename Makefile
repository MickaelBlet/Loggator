MAKEFLAG	=	--no-print-directory

BUILD_DIR	=	./build/

MODULE	=	\
			Loggator

ifeq (exe_test,$(firstword $(MAKECMDGOALS)))
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
endif
ifndef RUN_ARGS
  RUN_ARGS := $(MODULE)
endif

all:		MODE = debug
all:		$(MODULE)

help:
	@echo "Makefile options:"
	@echo
	@echo "all      #launch default (depend default mode) module rules."
	@echo "debug    #launch debug module rules."
	@echo "release  #launch release module rules."
	@echo "test     #launch test module rules."
	@echo "exe_test #launch exe_test module rules."
	@echo "clean    #launch clean module rules."
	@echo "fclean   #launch fclean module rules."
	@echo "re       #launch fclean and all module rules."
	@echo
	@$(foreach module,$(MODULE),printf "%-20s #launch default rule for $(module) module\n" "$(module)";)
	@echo
	@echo "advance example:"
	@echo "make moduleName MODE=release   #call makefile of moduleName and depend with release mode"
	@echo "make exe_test moduleName       #call makefile of moduleName with exe_test mode"
	@echo

Loggator:
	$(MAKE) $(MAKEFLAG) $(MODE) -C $(BUILD_DIR)$@

#------------------------------------------------------------------------------

debug:		MODE = debug
debug:		$(MODULE)

release:	MODE = release
release:	$(MODULE)

test:		MODE = test
test:		$(MODULE)

exe_test:	MODE = debug
exe_test:	$(RUN_ARGS)
	$(foreach module, $(RUN_ARGS), $(MAKE) $(MAKEFLAG) exe_test -C $(BUILD_DIR)$(module);)

clean:		MODE = clean
clean:		$(MODULE)

fclean:		MODE = fclean
fclean:		$(MODULE)

re:			fclean
	$(MAKE) $(MAKEFLAG) all

.PHONY:		all help debug release test exe_test clean fclean re $(MODULE)