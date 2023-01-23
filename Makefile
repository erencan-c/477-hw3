COMPILER=g++
RM=rm
SRC_EXTENSION=cpp
OBJ_EXTENSION=o
RUNNER=

INCDIR=./include
SRCDIR=.
OBJDIR=.
BUILDDIR=.
BINDIR=.
SRCS = $(wildcard $(SRCDIR)/*.$(SRC_EXTENSION))
OBJS = $(patsubst $(SRCDIR)/%.$(SRC_EXTENSION), $(OBJDIR)/%.$(OBJ_EXTENSION), $(SRCS))
DEPENDS = $(patsubst $(SRCDIR)/%.$(SRC_EXTENSION),%.d,$(SRCS))
HEADERS = $(wildcard $(INCDIR)/*.h)

CFLAGS=-I"./$(INCDIR)" -O3 -Wno-ignored-attributes -fopenmp -mavx2 -Wall
LDFLAGS=$(CFLAGS) -fPIC -lm -O3 -fopenmp -lGL -lGLU -lGLEW -lglfw -lX11 -lpthread -lXrandr -lXi -ldl -lfreetype

EXECNAME=hw3
ARGS=8 6 ./bunny.obj

rasterizer: $(OBJS)
	@$(COMPILER) $(filter-out %.h,$^) -o $(BINDIR)/$(EXECNAME) $(LDFLAGS)
	@echo LINKING $<
.PHONY: rasterizer

clean:
	@$(RM) $(OBJS)
	@echo RM $(OBJS)
.PHONY: clean

-include $(DEPENDS)

$(OBJDIR)/%.$(OBJ_EXTENSION): $(SRCDIR)/%.$(SRC_EXTENSION)
	@$(COMPILER) $(CFLAGS) -MMD -MP -MF $(patsubst %.$(SRC_EXTENSION),%.d,$<) -c $< -o $@
	@echo COMPILE $<

init:
	@echo Initializing Folders
	@mkdir $(INCDIR)
	@mkdir $(SRCDIR)
	@mkdir $(OBJDIR)
	@mkdir $(BUILDDIR)
	@mkdir $(BINDIR)
	@echo Initialization Completed
.PHONY: init

remove: check_remove
	@$(RM) -rf $(INCDIR)
	@$(RM) -rf $(SRCDIR)
	@$(RM) -rf $(OBJDIR)
	@$(RM) -rf $(BUILDDIR)	
	@$(RM) -rf $(BINDIR)
.PHONY: remove


check_remove:
	@echo Removing folders
	@echo -n "Are you sure? [y/N] " && read ans && [ $${ans:-N} = y ]

.PHONY: remove check_remove
