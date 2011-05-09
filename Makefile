tensor_storage_new.ccCXX=g++
INCLUDES=-I.
DEBUG=-g -O0 -fverbose-asm
STRICT=-pedantic -ansi -std -Wall
CXXFLAGS= -c $(DEBUG)	\
$(INCLUDES)
LDFLAGS=-Wall $(DEBUG)

HEADERS_GENERAL=error.h file.h memory.h random.h utility.h
HEADERS_MATRIX=matrix.h mmio.h
HEADERS_TENSOR=tensor.h
HEADERS=$(HEADERS_GENERAL) $(HEADERS_MATRIX) $(HEADERS_TENSOR)

SOURCES_GENERAL=error.cc file.cc memory.cc mmio.cc random.cc
SOURCES_MATRIX=matrix_arithmetic.cc matrix_clear.cc		\
	matrix_compatible.cc matrix_copy.cc matrix_delete.cc	\
	matrix_new.cc matrix_partition.cc matrix_supported.cc	\
	matrix_read.cc matrix_write.cc
SOURCES_TENSOR=tensor_arithmetic.cc tensor_clear.cc		\
	tensor_compatible.cc tensor_copy.cc tensor_delete.cc	\
	tensor_new.cc tensor_partition.cc tensor_supported.cc	\
	tensor_storage_new.cc tensor_read.cc tensor_write.cc
SOURCES=main.cc $(SOURCES_GENERAL) $(SOURCES_MATRIX) $(SOURCES_TENSOR)

ASSEMBLER=$(SOURCES:.cc=.s)
OBJECTS=$(ASSEMBLER:.s=.o)
EXECUTABLE=tensor

all: $(HEADERS) $(SOURCES) $(EXECUTABLE)

asm: $(HEADERS) $(ASSEMBLER) $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)
	dsymutil $(EXECUTABLE)

$(EXECUTABLE): $(HEADERS) $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@
	dsymutil $@

.cc.o: $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -o $@

.cc.s: $(HEADERS)
	$(CXX) $(CXXFLAGS) -S $<
	$(CXX) $(CXXFLAGS) $@ -o $(@:.s=.o)

rebuild: clean all

clean:
	rm -rf *~ *\# *.dSYM/ $(ASSEMBLER) $(OBJECTS) $(EXECUTABLE)	\
	annotate.*.h cache.*.dat a.out

##
# Local Variables:
# mode: Makefile
# End:
##
