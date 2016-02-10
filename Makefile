PROJECT := henon

USE_PNG ?= 1

CC      ?= gcc
INCS    :=
CFLAGS  := -W -Wall -Wextra -O2 -std=c99
# Compiler flags for auto dependency generation:
CFLAGS  += -MMD -MP

LD      := $(CC)
LIBS    :=
LDFLAGS := -lm

ifeq ($(USE_PNG),1)
	CFLAGS  += -DUSE_PNG
	LDFLAGS	+= -lpng
endif

STRIP   := strip
RM      := rm -f

BIN     := $(PROJECT)
SRC     := $(wildcard *.c)
OBJ     := $(SRC:%.c=%.o)
DEP     := $(OBJ:%.o=%.d)
SELF    := $(lastword $(MAKEFILE_LIST))


.PHONY: all clean distclean demo

all: $(BIN)

$(BIN): $(OBJ) $(SELF)
	$(LD) $(LIBS) $(LDFLAGS) $(OBJ) -o $(BIN)
	$(STRIP) $(BIN)

%.o: %.c $(SELF)
	$(CC) -c $(INCS) $(CFLAGS) -o $*.o $*.c

clean:
	$(RM) $(BIN) $(OBJ) $(DEP)

distclean: clean
	$(RM) *.avi *.mp4 *.mkv *.gif *.ppm *.png
	$(RM) -r demo

demo: all
	./demo.sh

-include $(DEP)
