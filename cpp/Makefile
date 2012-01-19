# definitions
CC = g++
CCFLAGS = -Wall
OBJS = fileno.o main.o mmio.o

# targets and prerequisites
.PHONY: all
all: tensor

tensor: $(OBJS)
	$(CC) $^ -o $@

# default rule for compiling .cc to .o
%.o: %.cc
	$(CC) -c $(CCFLAGS) $< 

# generate the prerequistes and append to the desired file
.prereq: $(OBJS:.o=.cc) $(wildcard *.h) Makefile
	rm -f .prereq
	$(CC) $(CCFLAGS) -MM $(OBJS:.o=.cc) >> ./.prereq 

# include the generated prerequisite file
include .prereq

.PHONY: clean clean-all
clean:
	rm -f *.o
	rm -f *~ *# .#*

clean-all: clean
	rm -f tensor

