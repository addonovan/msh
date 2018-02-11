PRODUCT := msh
BINDIR := bin
INCDIR := include
SRCDIR := src
OBJDIR := obj

CC := gcc
LINKER := gcc
INCDIRS := -I$(INCDIR)
CFLAGS := -Wall -Werror

SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCFILES))

build: makedirs $(BINDIR)/$(PRODUCT)
.PHONY: build

$(BINDIR)/$(PRODUCT): $(OBJFILES)
	$(LINKER) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@

makedirs:
	mkdir -p $(BINDIR)
	mkdir -p $(OBJDIR)
.PHONY: makedirs

