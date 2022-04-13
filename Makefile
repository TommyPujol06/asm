bin = emu
src = $(wildcard *.c)
obj = $(src:.c=.o)
CFLAGS = -g -Wall -Wextra
LDFLAGS =

.PHONY: all clean

all: $(bin)
	strip $(bin)

$(bin): $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	-rm $(bin) $(obj)
