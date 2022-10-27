all: fontconvert

CC     = gcc
CFLAGS = -Wall -I/nix/store/k9lw6px3nnwfpk44s7cqskfq525m5fkd-freetype-2.10.4-dev/include/ -L/nix/store/l12jbk7ajyx3b7sx0plb8r0jnpkcv14m-freetype-2.10.4/lib
LIBS   = -lfreetype

fontconvert: fontconvert.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@
	strip $@

clean:
	rm -f fontconvert
