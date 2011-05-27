CXX=g++
INCLUDES=-I.
DEBUG=-g -O0 -fverbose-asm
STRICT=-pedantic -ansi -std -Wall
CXXFLAGS= -c $(DEBUG)	\
$(INCLUDES)
LDFLAGS=-Wall $(DEBUG)

HEADERS_GENERAL=arithmetic.h error.h file.h memory.h random.h	\
		utility.h
HEADERS_MATRIX=matrix.h mmio.h
HEADERS_TENSOR=storage.h tensor.h
HEADERS=$(HEADERS_GENERAL) $(HEADERS_MATRIX) $(HEADERS_TENSOR)

SOURCES_GENERAL=arithmetic.cc error.cc file.cc memory.cc mmio.cc	\
	random.cc utility.cc
SOURCES_MATRIX=matrix_arithmetic.cc matrix_clear.cc		\
	matrix_compatible.cc matrix_copy.cc matrix_free.cc	\
	matrix_malloc.cc matrix_partition.cc matrix_supported.cc	\
	matrix_read.cc matrix_write.cc
SOURCES_STORAGE=storage_convert.cc storage_compressed.cc	\
	storage_ekmr.cc storage_malloc.cc
SOURCES_TENSOR=tensor_arithmetic.cc tensor_clear.cc		\
	tensor_compatible.cc tensor_convert.cc tensor_free.cc	\
	tensor_malloc.cc tensor_supported.cc tensor_read.cc	\
	tensor_write.cc tensor_utility.cc tensor_validate.cc
SOURCES=$(SOURCES_GENERAL) $(SOURCES_MATRIX) $(SOURCES_STORAGE)	\
	$(SOURCES_TENSOR)

ASSEMBLER=$(SOURCES:.cc=.s)
OBJECTS=$(ASSEMBLER:.s=.o)
CONVERSION=convert
EXECUTABLE=tensor

all: $(HEADERS) $(SOURCES) $(CONVERSION) $(EXECUTABLE)

asm: $(HEADERS) $(ASSEMBLER) $(OBJECTS) $(EXECUTABLE).o
	$(CXX) $(LDFLAGS) $(OBJECTS) $(EXECUTABLE).o -o $(EXECUTABLE)
	dsymutil $(EXECUTABLE)

$(CONVERSION): $(HEADERS) $(OBJECTS) $(CONVERSION).o
	$(CXX) $(LDFLAGS) $(OBJECTS) $@.o -o $@
	dsymutil $@

$(EXECUTABLE): $(HEADERS) $(OBJECTS) $(EXECUTABLE).o
	$(CXX) $(LDFLAGS) $(OBJECTS) $@.o -o $@
	dsymutil $@

.cc.o: $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -o $@

.cc.s: $(HEADERS)
	$(CXX) $(CXXFLAGS) -S $<
	$(CXX) $(CXXFLAGS) $@ -o $(@:.s=.o)

rebuild: clean all

clean:
	rm -rf *~ *\# *.dSYM/ $(ASSEMBLER) $(OBJECTS) $(CONVERSION)	\
	$(EXECUTABLE) *.o annotate.*.h cache.*.dat a.out

##
# Local Variables:
# mode: Makefile
# End:
##
