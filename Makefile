############################################################################################################
# Few rules to follow when editing this file:
#
# 1. Shell commands must be indented with a tab
# 2. Before each target add ".PHONY: target_name" to disable default file target
# 3. Add target description prefixed with "##" on the same line as target definition for "help" target to work
# 4. Be aware that each make command is executed in separate shell
#
# Tips:
#
# * Add an @ sign to suppress output of the command that is executed
# * Define variable like: VAR := value
# * Reference variable like: $(VAR)
# * Reference environment variables like: $(ENV_VARIABLE)
#
#############################################################################################################
.DELETE_ON_ERROR:
.SHELLFLAGS 	:= -eu -o pipefail -c
SHELL			= /bin/bash
BUILDOUT		= ./build
BINOUT			= ./bin
MAKEFLAGS 		+= --warn-undefined-variables
MAKEFLAGS 		+= --no-builtin-rules
MAKEFLAGS		+= --no-print-directory

BOLD = \033[1m
CLEAR = \033[0m
CYAN = \033[36m
GREEN = \033[32m

##@: Default
.PHONY: help
help: ## Display this help
	@awk '\
		BEGIN {FS = ":.*##"; printf "Usage: make $(CYAN)<target>$(CLEAR)\n"} \
		/^[0-9a-zA-Z_\-\/]+?:[^#]*?## .*$$/ { printf "\t$(CYAN)%-20s$(CLEAR) %s\n", $$1, $$2 } \
		/^##@/ { printf "$(BOLD)%s$(CLEAR)\n", substr($$0, 5); }' \
		$(MAKEFILE_LIST)

##@: Build/Run

all: clean clox  ## ALL, builds the world

.PHONY: clean
clean: ## Clean-up build artifacts
	@echo -e "$(CYAN)--- clean...$(CLEAR)"
	@rm -rf ${BINOUT}
	@rm -rf ${BUILDOUT}

${BUILDOUT}:
	@mkdir -p ${BUILDOUT}
${BINOUT}:
	@mkdir -p ${BINOUT}

# Compile a debug build of clox.
debug: ${BUILDOUT} ${BINOUT} ## Clean-up build artifacts
	@echo -e "$(CYAN)--- debug...$(CLEAR)"
	@$(MAKE) -f tools/c.make NAME=cloxd MODE=debug SOURCE_DIR=src
	@cp build/cloxd ${BINOUT}/cloxd

# Compile the C interpreter.
clox: ${BUILDOUT} ${BINOUT} ## Clean-up build artifacts
	@echo -e "$(CYAN)--- clox...$(CLEAR)"
	@$(MAKE) -f tools/c.make NAME=clox MODE=release SOURCE_DIR=src
	@cp build/clox ${BINOUT}/clox # For convenience, copy the interpreter to the top level.
