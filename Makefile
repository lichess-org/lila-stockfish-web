CXX = em++
EXE = stockfishWeb

# make NODE=1 FLAGS="-O0 -g3 -sASSERTIONS=2 -sSAFE_HEAP --source-map-base=http://localhost:9090/src/"

FLAGS ?= -O3 -DNDEBUG --closure=1

ifeq ($(filter 1 true TRUE,$(NODE)),)
TARGET_FLAGS = -sENVIRONMENT=web,worker -sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency
else
TARGET_FLAGS = -sENVIRONMENT=node
endif

CXX_FLAGS = $(FLAGS) -pthread -msse -msse2 -mssse3 -msse4.1 -msimd128 -flto \
	-fno-exceptions -DUSE_SSE2 -DUSE_SSSE3 -DUSE_SSE41 -DUSE_POPCNT -DNNUE_EMBEDDING_OFF
	
LD_FLAGS = $(CXX_FLAGS)  --pre-js=src/initModule.js -sEXPORT_ES6 -sEXPORT_NAME=StockfishWeb \
	-sEXPORTED_FUNCTIONS='[_malloc]' -sEXPORTED_RUNTIME_METHODS='[stringToUTF8,UTF8ToString]' \
	-sINCOMING_MODULE_JS_API='[locateFile,print,printErr,wasmMemory,buffer,instantiateWasm]' \
	-sINITIAL_MEMORY=64MB -sALLOW_MEMORY_GROWTH -sSTACK_SIZE=2MB -sFILESYSTEM=0 -sSTRICT \
	-sALLOW_BLOCKING_ON_MAIN_THREAD=0 -sPROXY_TO_PTHREAD

TARGETS := sf-hce-nnue-40 sf-nnue-60 linrock-nnue-7 linrock-nnue-12 fsf-hce

.PHONY: build clean

#define make_target

$(TARGETS):
	$(eval SRCS := $(shell find fishes/$@/src -name '*.cpp' | grep -v 'tbprobe.cpp\|pyffish.cpp'))
	$(eval OBJS := $(SRCS:.cpp=.o) src/glue.o)
	@echo Building for target $@ with sources $(SRCS) and objects $(OBJS)
#	$(MAKE) $(OBJS)
	$(foreach src,$(SRCS),$(CXX) -Ifishes/$@/src $(CXX_FLAGS) -c $(src) -o $(src:.cpp=.o);)
	$(CXX) -o $@.js $(OBJS) $(LD_FLAGS) $(TARGET_FLAGS)
#	@mv $(EXE).wasm $(EXE).$(1).wasm 
ifeq ($(NODE),1)
	@cat src/wasm/createRequire.js $(EXE).worker.js > $(EXE).worker.js.tmp
	@mv $(EXE).worker.js.tmp $(EXE).worker.js
endif
#endef

#$(foreach target,$(TARGETS),$(eval $(call make_target,$(target))))

# build:
# 	SRCS=$$(find fishes/$(TARGET)/src -name '*.cpp' | grep -v 'tbprobe.cpp'); \
# 	OBJS="$${SRCS//.cpp/.o} src/glue.o"; \
# 	$(CXX) -o $(EXE).js $$OBJS $(LD_FLAGS) $(TARGET_FLAGS)
# 	@mv $(EXE).wasm $(EXE).$(TARGET).wasm 
# ifeq ($(NODE),1)
# 	@cat src/wasm/createRequire.js $(EXE).worker.js > $(EXE).worker.js.tmp
# 	@mv $(EXE).worker.js.tmp $(EXE).worker.js
# endif

#%.o: %.cpp
#	$(CXX) $(CXX_FLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS) $(EXE).js $(EXE).wasm* $(EXE).worker.js
