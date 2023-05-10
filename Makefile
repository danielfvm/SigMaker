CC=gcc
CFLAGS=-Wall -Werror
LIBS=-ludis86
INSTALL_PATH=/usr/bin/sigmaker

all: sigmaker

sigmaker: SigMaker.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

install:
	cp sigmaker $(INSTALL_PATH)

uninstall:
	rm $(INSTALL_PATH)

clean:
	rm -f sigmaker
