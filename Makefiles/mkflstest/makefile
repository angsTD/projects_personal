# This is a comment in this makefile
# As more advanced features are added to this sample make file , add comments to clarify its intended use

#INCLUDE_LIBRARY = ./sampleLibrary
#VPATH = $(INCLUDE_LIBRARY)
APP 	= sampleexe
SRCEXT	= cc
SRCDIR 	= src
OBJDIR	= obj
BINDIR	= bin

CC = g++
INCLUDES  = -I../inc
DEBUG = -g

CFLAGS = -c -Wall $(INCLUDES) 

#VPATH = /sampleLibrary/src:/sampleLibrary/inc:/core/src
#BUILDDIR = build

vpath %.h /sampleLibrary/inc
vpath %.cc /core/src
vpath %.cc /sampleLibrary/src

#%.o : %.cc
#	$(CC) $(CFLAGS) $^ -o $@

sampleexe : /core/src/samMain.o /sampleLibrary/src/sampleApi.o
	$(CC) -o sampleexe samMain.o sampleApi.o

samMain.o : samMain.cc sampleApi.h
	$(CC) $(CFLAGS) samMain.cc

sampleApi.o : sampleApi.cc sampleApi.h
	$(CC) $(CFLAGS) sampleApi.cc
clean : 
	rm sampleexe samMain.o sampleApi.o
