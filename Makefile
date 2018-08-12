CFLAGS=`sdl2-config --cflags` -Wall -Wextra
LDFLAGS=`sdl2-config --libs` -lm

BIN=kmeans

.PHONY: all clean

all: ${BIN}

clean:
	-rm -f ${BIN}
