TOP = $(shell pwd)
TARGET = run_timer

CC = g++

CCFLAGS = -pthread -m64

SRCDIR = ./src

_dummy := $(shell mkdir -p lib)
LDIR = ./lib
INCLUDEDIR = -I. -I./include

all: run_timer

run_timer: runTimerLib ccf PulseExtractor BaseRun SignalContainer
	$(CC) $(CCFLAGS) $(LDIR)/run_timer.o $(LDIR)/ccf.o $(LDIR)/PulseExtractor.o $(LDIR)/BaseRun.o $(LDIR)/SignalContainer.o -o run_timer.exe $(INCLUDEDIR) $(LIBDIR) $(LDFLAGS)

runTimerLib:
	$(CC) $(CCFLAGS) $(SRCDIR)/run_timer.cc -c -o $(LDIR)/run_timer.o $(INCLUDEDIR) $(LIBDIR) $(LDFLAGS)

ccf:
	$(CC) $(CCFLAGS) $(SRCDIR)/ccf.cc -c -o $(LDIR)/ccf.o $(INCLUDEDIR) $(LIBDIR) $(LDFLAGS)

PulseExtractor:
	$(CC) $(CCFLAGS) $(SRCDIR)/PulseExtractor.cc -c -o $(LDIR)/PulseExtractor.o $(INCLUDEDIR) $(LIBDIR) $(LDFLAGS)

BaseRun:
	$(CC) $(CCFLAGS) $(SRCDIR)/BaseRun.cc -c -o $(LDIR)/BaseRun.o $(INCLUDEDIR) $(LIBDIR) $(LDFLAGS)

SignalContainer:
	$(CC) $(CCFLAGS) $(SRCDIR)/SignalContainer.cc -c -o $(LDIR)/SignalContainer.o $(INCLUDEDIR) $(LIBDIR) $(LDFLAGS)

clean:
	rm -rf $(LDIR)/*o run_timer.exe
