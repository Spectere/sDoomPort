# sDoomPort Makefile
CC        = cc
CFLAGS    = -D_REENTRANT -I/usr/include/SDL2/ -lSDL2
OBJDIR    = ./obj
BINDIR    = ./bin
MKDIR     = mkdir -p

-include Makefile.override

OUTFILE   = sDoomPort

ALL_C    := $(shell find . -type f -name '*.c')
OBJFILES := $(addprefix $(OBJDIR)/,$(ALL_C:%.c=%.o))

all: $(OUTFILE)

$(OBJDIR)/%.o: %.c
	$(MKDIR) $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OUTFILE): $(OBJFILES)
	$(MKDIR) $(BINDIR)
	$(CC) $(CFLAGS) -o $(BINDIR)/$(OUTFILE) $^

clean:
	$(RM) -r $(OBJDIR)/*
	$(RM) $(BINDIR)/$(OUTFILE)

install:
	# currently unsupported

