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
INCLUDES  = -I./sampleLibrary
DEBUG = -g

CFLAGS = -c -Wall $(INCLUDES) 

VPATH = sampleLibrary
BUILDDIR = build

sampleexe : samMain.o sampleApi.o
	$(CC) -o sampleexe samMain.o

#samMain.o : samMain.cc
#	$(CC) $(CFLAGS) samMain.cc

#sampleApi.o : #sampleApi.cc
#	$(CC) $(CFLAGS) sampleApi.cc

$(BUILDDIR)/%.o : %.cc
	$(CC) $(CFLAGS) $< -o $@
clean : 
	rm sampleexe samMain.o sampleApi.o
