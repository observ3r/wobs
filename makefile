IDIR =./include
CC=gcc
CFLAGS=-I$(IDIR) `pkg-config --cflags gtk+-2.0`

SDIR=./src
ODIR=./src/obj
LDIR =./lib

LIBS=`pkg-config --libs gtk+-2.0`

_DEPS = wobs.h wlan.h global.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = wobs.o wlan.o add_dev.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

wobs: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
