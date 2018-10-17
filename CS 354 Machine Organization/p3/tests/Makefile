C_FILES := $(wildcard *.c)
TARGETS := ${C_FILES:.c=}

all: ${TARGETS}

%: %.c
	gcc -I.. -g -m32 -Xlinker -rpath=.. -o $@ $< -L.. -lmem -std=gnu99

clean:
	rm -rf ${TARGETS} *.o
