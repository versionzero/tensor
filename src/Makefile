CXX=g++
SYMBOLS=echo
OS=`uname`
ifeq ($(OS), Darwin)
	CXX=clang
	CPPX11=-std=c++0x -stdlib=libc++
	SYMBOLS=dsymutil
endif
INCLUDES=-I.
STRICT=-pedantic -Wall -Wno-variadic-macros
EXTRA_DEBUG=-g
ifndef DEBUG
	EXTRA_DEBUG += -DNODEBUG
endif
ifndef SIMULATE
	EXTRA_DEBUG += -DNOSIMULATE
endif
EXTRA_CXXFLAGS=-c $(EXTRA_DEBUG) $(STRICT) $(INCLUDES) $(CPPX11)
EXTRA_LDFLAGS=-Wall $(EXTRA_DEBUG)

HEADERS_CACHE=address.h cache.h hash.h
HEADERS_GENERAL=arithmetic.h error.h file.h information.h latex.h	\
	 memory.h operation.h random.h strings.h timer.h tool.h		\
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
	strings.cc timer.cc tool_convert.cc tool_effectuate.cc		\
	tool_generate.cc tool_permute.cc tool_timing.cc			\
	tool_utility.cc types.cc utility.cc
SOURCES_GENERATE=generate_tensor_from_matrix.cc
SOURCES_MATRIX=matrix_arithmetic.cc matrix_clear.cc			\
	matrix_compatible.cc matrix_copy.cc matrix_free.cc		\
	matrix_malloc.cc matrix_partition.cc matrix_supported.cc	\
	matrix_read.cc matrix_write.cc
SOURCES_STORAGE=tensor_storage_convert.cc				\
	tensor_storage_compressed.cc					\
	tensor_storage_compressed_slice.cc				\
	tensor_storage_coordinate.cc tensor_storage_matrix_slice.cc	\
	tensor_emit_latex.cc tensor_storage_ekmr.cc			\
	tensor_storage_gundersen.cc tensor_storage_malloc.cc		\
	tensor_storage_utility.cc tensor_storage_zzekmr.cc
SOURCES_TENSOR=tensor_arithmetic.cc tensor_clear.cc tensor_convert.cc	\
	tensor_copy.cc tensor_free.cc tensor_malloc.cc			\
	tensor_ownership.cc tensor_permute.cc tensor_supported.cc	\
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

default: dependencies
	@echo The OS environment variable is set to [$(OS)].
	@make -j4 all

all: $(HEADERS) $(SOURCES) $(EXECUTABLE)

asm: $(HEADERS) $(ASSEMBLER) $(OBJECTS)
	$(SYMBOLS) $(EXECUTABLE)

# Use Clang's static analysis tool on our source
analysis:
	for FILE in $(wildcard *.cc); do \
		clang --analyze $${FILE}; \
	done;

$(EXECUTABLE): $(HEADERS) $(OBJECTS)
	$(CXX) $(EXTRA_LDFLAGS) $(LDFLAGS) $(OBJECTS) -o $@
	$(SYMBOLS) $@

.cc.o: $(HEADERS)
	$(CXX) $(EXTRA_CXXFLAGS) $(CXXFLAGS) -o $@ $<

#$(CXX) $(EXTRA_CXXFLAGS) $(CXXFLAGS) -MM -MF $(patsubst %.o,%.d,$@) $<

.cc.s: $(HEADERS)
	$(CXX) $(CXXFLAGS) -S $<
	$(CXX) $(CXXFLAGS) $@ -o $(@:.s=.o)

debug:
	@make -j4 DEBUG=1

rebuild: clean
	@make -j4 all

clean:
	@rm -rf *~ *\# *.dSYM/ $(ASSEMBLER) $(OBJECTS) $(CONVERSION)	\
		$(EXECUTABLE) *.o *.d annotate.*.h cache.*.dat a.out	\
		*.plist *.bak;

dependencies:
	@touch Makefile.d
	@makedepend -I/usr/Llvm-gcc-4.2/lib/gcc/i686-apple-darwin11/4.2.1/include -fMakefile.d -- $(EXTRA_CXXFLAGS) $(CXXFLAGS) -- $(SOURCES)

-include Makefile.d

##
# Local Variables:
# mode: Makefile
# End:
##

# DO NOT DELETE THIS LINE -- make depend depends on it.
