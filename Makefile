CC := gcc
NAME := main
SOURCEDIR := src
BUILDDIR := build

SRCS := $(shell find $(SOURCEDIR) -name '*.c')
OBJS := $(patsubst $(SOURCEDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

CFLAGS := -Wall -I$(SOURCEDIR) -ggdb

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(NAME)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm $(NAME)
	rm -rf $(BUILDDIR)
	mkdir $(BUILDDIR)

-include $(OBJS:.o=.d)
