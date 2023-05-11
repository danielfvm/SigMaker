CC=gcc
CFLAGS=-Wall -Werror
LIBS=-ludis86
NAME=sigmaker
INSTALL_PATH=/usr/bin/$(NAME)

all: $(NAME)

$(NAME): SigMaker.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

install:
	cp $(NAME) $(INSTALL_PATH)

uninstall:
	rm $(INSTALL_PATH)

clean:
	rm -f $(NAME)
