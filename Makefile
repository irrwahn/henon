PROJECT := henon

CC      ?= gcc
INCS    :=
CFLAGS  := $(INCS) -W -Wall -Wextra -O2 -std=c99
# Compiler flags for auto dependency generation:
CFLAGS  += -MMD -MP

LD      := $(CC)
LIBS    :=
LDFLAGS := $(LIBS) -lm

STRIP   := strip
RM      := rm -f

BIN     := $(PROJECT)
SRC     := $(wildcard *.c)
OBJ     := $(SRC:%.c=%.o)
DEP     := $(OBJ:%.o=%.d)
SELF    := $(lastword $(MAKEFILE_LIST))


.PHONY: all clean distclean

all: $(BIN)

$(BIN): $(OBJ) $(SELF)
	$(LD) $(LDFLAGS) $(OBJ) -o $(BIN)
	$(STRIP) $(BIN)

%.o: %.c $(SELF)
	$(CC) -c $(CFLAGS) -o $*.o $*.c

clean:
	$(RM) $(BIN) $(OBJ) $(DEP)

distclean: clean
	$(RM) *.avi *.mp4 *.gif *.ppm
	$(RM) -r ppm

-include $(DEP)
