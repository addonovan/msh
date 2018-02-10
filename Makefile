PRODUCT := msh
BINDIR := bin
INCDIR := include
SRCDIR := src
OBJDIR := obj

CC := gcc
LINKER := gcc
INCDIRS := -I$(INCDIR)
CFLAGS := -Wall -Werror -pedantic

SRCFILES := $(wildcard $(SRCDIR)/*.c)
OBJFILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCFILES))

$(BINDIR)/$(PRODUCT): $(OBJFILES)
	$(LINKER) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@

