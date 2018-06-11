#------------------------------------------------------------------------------
FindRelativePath=$(if $(subst $(notdir $(2)),,$(3)),$(call FindRelativePath,$(1)../,$(abspath $(2)/..),$(3)),$(1))
#------------------------------------------------------------------------------
# Relative path
HOME_DIR		=	$(call FindRelativePath,../,$(CURDIR),build)

# Dependence local
LOCAL_LIB		=
# Dependence module library
MODULE_LIB		=
# Dependence external library
EXTERNAL_LIB	=
# File list not include in library
NOT_SRC_LIB		=
#------------------------------------------------------------------------------
include $(HOME_DIR)templateMakefile/Makefile
.PHONY: