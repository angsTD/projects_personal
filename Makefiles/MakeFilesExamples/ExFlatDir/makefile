# This is a comment in this makefile
# As more advanced features are added to this sample make file , add comments to clarify its intended use

#INCLUDE_LIBRARY = ./sampleLibrary
#VPATH = $(INCLUDE_LIBRARY)
APP 	= sampleexe

CC = g++
INCLUDES  = -I../inc
DEBUG = -g

CFLAGS = -c -Wall $(INCLUDES) 

$(APP) : samMain.o sampleApi.o
	$(CC) -o sampleexe samMain.o sampleApi.o

#samMain.o : samMain.cc
#	$(CC) $(CFLAGS) samMain.cc

#sampleApi.o : #sampleApi.cc
#	$(CC) $(CFLAGS) sampleApi.cc

%.o : %.cc
	$(CC) $(CFLAGS) $< -o $@
clean : 
	rm sampleexe samMain.o sampleApi.o
