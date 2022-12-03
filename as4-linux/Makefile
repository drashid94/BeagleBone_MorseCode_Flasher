# Makefile for building embedded application.
# by Brian Fraser
# Edit this file to compile extra C files into their own programs.
TARGET= as4-morsecode.out
SOURCES= morsecode.c main.c memMap.c display.c util.c
PUBDIR = $(HOME)/cmpt433/public/myApps
OUTDIR = $(PUBDIR)
CROSS_TOOL = arm-linux-gnueabihf-
CC_CPP = $(CROSS_TOOL)g++
CC_C = $(CROSS_TOOL)gcc
CFLAGS = -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -Wall

# -pg for supporting gprof profiling.
#CFLAGS += -pg
all: cmp
cmp:
	$(CC_C) $(CFLAGS) $(SOURCES) -o $(OUTDIR)/$(TARGET) $(LFLAGS) -lpthread
clean:
	rm -f $(OUTDIR)/$(TARGET)