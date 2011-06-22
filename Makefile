CXX=clang
INCLUDES=-I.
DEBUG=-g #-DNODEBUG
STRICT=-pedantic -Wall -Wno-variadic-macros
CXXFLAGS=-c $(DEBUG) $(STRICT) $(INCLUDES)
LDFLAGS=-Wall $(DEBUG)

HEADERS_GENERAL=arithmetic.h error.h file.h information.h memory.h	\
		operation.h random.h tool.h utility.h compatible.h
HEADERS_MATRIX=matrix.h mmio.h
HEADERS_TENSOR=storage.h tensor.h
HEADERS_VECTOR=vector.h
HEADERS=$(HEADERS_GENERAL) $(HEADERS_MATRIX) $(HEADERS_TENSOR)	\
	$(HEADERS_VECTOR)

SOURCES_GENERAL=arithmetic.cc compatible.cc error.cc file.cc		\
	information.cc memory.cc mmio.cc operation_n_mode_product.cc	\
	operation_utility.cc random.cc tool.cc types.cc utility.cc
SOURCES_MATRIX=matrix_arithmetic.cc matrix_clear.cc			\
	matrix_compatible.cc matrix_copy.cc matrix_free.cc		\
	matrix_malloc.cc matrix_partition.cc matrix_supported.cc	\
	matrix_read.cc matrix_write.cc
SOURCES_STORAGE=storage_convert.cc storage_compressed.cc	\
	storage_ekmr.cc storage_malloc.cc storage_pkmr.cc	\
	storage_zzpkmr.cc
SOURCES_TENSOR=tensor_arithmetic.cc tensor_clear.cc tensor_convert.cc	\
	tensor_free.cc tensor_malloc.cc tensor_supported.cc		\
	tensor_read.cc tensor_write.cc tensor_utility.cc		\
	tensor_validate.cc
SOURCES_VECTOR=vector_clear.cc vector_free.cc vector_malloc.cc	\
	vector_read.cc vector_write.cc
SOURCES=$(SOURCES_GENERAL) $(SOURCES_MATRIX) $(SOURCES_STORAGE)	\
	$(SOURCES_TENSOR) $(SOURCES_VECTOR) main.cc

ASSEMBLER=$(SOURCES:.cc=.s)
OBJECTS=$(ASSEMBLER:.s=.o)
EXECUTABLE=tensor
UTILITIES=convert effectuate

all: $(HEADERS) $(SOURCES) $(EXECUTABLE) utilities

asm: $(HEADERS) $(ASSEMBLER) $(OBJECTS)
	dsymutil $(EXECUTABLE)

# Use Clang's static analysis tool on our source
analysis:
	for FILE in $(wildcard *.cc); do \
		clang --analyze $${FILE}; \
	done;

$(EXECUTABLE): $(HEADERS) $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@
	dsymutil $@

utilities:
	for UTILITY in $(UTILITIES); do \
		\ln -fsv $(EXECUTABLE) $${UTILITY}; \
		\ln -fsv $(EXECUTABLE).dSYM $${UTILITY}.dSYM; \
	done;

.cc.o: $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -o $@

.cc.s: $(HEADERS)
	$(CXX) $(CXXFLAGS) -S $<
	$(CXX) $(CXXFLAGS) $@ -o $(@:.s=.o)

rebuild: clean all

clean:
	rm -rf *~ *\# *.dSYM/ $(ASSEMBLER) $(OBJECTS) $(CONVERSION)	\
		$(EXECUTABLE) *.o annotate.*.h cache.*.dat a.out *.plist; \
	for UTILITY in $(UTILITIES); do \
		rm -fv $${UTILITY}; \
		rm -fv $${UTILITY}.dSYM; \
	done;

##
# Local Variables:
# mode: Makefile
# End:
##
