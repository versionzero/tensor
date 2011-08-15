CXX=g++
INCLUDES=-I.
STRICT=-pedantic -Wall -Wno-variadic-macros
ifdef DEBUG
	EXTRA_DEBUG=-g
else
	EXTRA_DEBUG=-g -DNODEBUG
endif
EXTRA_CXXFLAGS=-c $(EXTRA_DEBUG) $(STRICT) $(INCLUDES)
EXTRA_LDFLAGS=-Wall $(EXTRA_DEBUG)

HEADERS_CACHE=address.h cache.h hash.h
HEADERS_GENERAL=arithmetic.h error.h file.h information.h latex.h	\
		memory.h operation.h random.h strings.h tool.h		\
		utility.h compatible.h
HEADERS_GENERATE=generate.h
HEADERS_MATRIX=matrix.h mmio.h
HEADERS_TENSOR=storage.h tensor.h
HEADERS_VECTOR=vector.h
HEADERS=$(HEADERS_CACHE) $(HEADERS_GENERAL) $(HEADERS_GENERATE)	\
	$(HEADERS_MATRIX) $(HEADERS_TENSOR) $(HEADERS_VECTOR)

SOURCES_CACHE=address.cc cache.cc hash.cc
SOURCES_GENERAL=arithmetic.cc compatible.cc error.cc file.cc		\
	information.cc latex.cc memory.cc mmio.cc			\
	operation_n_mode_product.cc operation_utility.cc random.cc	\
	strings.cc tool_convert.cc tool_effectuate.cc			\
	tool_generate.cc tool_timing.cc tool_utility.cc types.cc	\
	utility.cc
SOURCES_GENERATE=generate_tensor_from_matrix.cc
SOURCES_MATRIX=matrix_arithmetic.cc matrix_clear.cc			\
	matrix_compatible.cc matrix_copy.cc matrix_free.cc		\
	matrix_malloc.cc matrix_partition.cc matrix_supported.cc	\
	matrix_read.cc matrix_write.cc
SOURCES_STORAGE=tensor_storage_convert.cc			\
	tensor_storage_compressed.cc				\
	tensor_storage_compressed_slice.cc tensor_emit_latex.cc	\
	tensor_storage_ekmr.cc tensor_storage_malloc.cc		\
	tensor_storage_utility.cc tensor_storage_zzekmr.cc
SOURCES_TENSOR=tensor_arithmetic.cc tensor_clear.cc tensor_convert.cc	\
	tensor_free.cc tensor_malloc.cc tensor_supported.cc		\
	tensor_read.cc tensor_write.cc tensor_utility.cc		\
	tensor_validate.cc
SOURCES_VECTOR=vector_clear.cc vector_free.cc vector_malloc.cc	\
	vector_read.cc vector_write.cc
SOURCES=$(SOURCES_CACHE) $(SOURCES_GENERAL) $(SOURCES_GENERATE)	\
	$(SOURCES_MATRIX) $(SOURCES_STORAGE) $(SOURCES_TENSOR)	\
	$(SOURCES_VECTOR) main.cc

ASSEMBLER=$(SOURCES:.cc=.s)
OBJECTS=$(ASSEMBLER:.s=.o)
EXECUTABLE=tensor

all: $(HEADERS) $(SOURCES) $(EXECUTABLE)

asm: $(HEADERS) $(ASSEMBLER) $(OBJECTS)
	dsymutil $(EXECUTABLE)

# Use Clang's static analysis tool on our source
analysis:
	for FILE in $(wildcard *.cc); do \
		clang --analyze $${FILE}; \
	done;

$(EXECUTABLE): $(HEADERS) $(OBJECTS)
	$(CXX) $(EXTRA_LDFLAGS) $(LDFLAGS) $(OBJECTS) -o $@
	dsymutil $@

.cc.o: $(HEADERS)
	$(CXX) $(EXTRA_CXXFLAGS) $(CXXFLAGS) $< -o $@

.cc.s: $(HEADERS)
	$(CXX) $(CXXFLAGS) -S $<
	$(CXX) $(CXXFLAGS) $@ -o $(@:.s=.o)

debug:
	make DEBUG=1

rebuild: clean
	make -j8 all

clean:
	rm -rf *~ *\# *.dSYM/ $(ASSEMBLER) $(OBJECTS) $(CONVERSION)	\
		$(EXECUTABLE) *.o annotate.*.h cache.*.dat a.out	\
		*.plist release;

##
# Local Variables:
# mode: Makefile
# End:
##
