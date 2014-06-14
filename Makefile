NACL_SDK_ROOT ?= $(HOME)/src/nacl_sdk/pepper_35

GETOS := python $(NACL_SDK_ROOT)/tools/getos.py
PNACL_TOOLCHAIN_PATH := $(NACL_SDK_ROOT)/toolchain/$(shell $(GETOS))_pnacl

CPPFLAGS = -I$(NACL_SDK_ROOT)/include
CXXFLAGS = -O2 -DTARGET_PPAPI -std=gnu++11 -Wall
LDFLAGS = -L$(NACL_SDK_ROOT)/lib/pnacl/Release -lppapi_cpp -lppapi -lppapi_gles2

OBJECTS = main_pnacl.bc gl.bc

all: main_pnacl.pexe

%.bc: %.cc
	$(PNACL_TOOLCHAIN_PATH)/bin/pnacl-clang++ $< -c $(CPPFLAGS) $(CXXFLAGS) $(OUTPUT_OPTION)

main_pnacl.pexe: $(OBJECTS)
	$(PNACL_TOOLCHAIN_PATH)/bin/pnacl-clang++ $(OBJECTS) $(OUTPUT_OPTION) $(CXXFLAGS) $(LDFLAGS)
	$(PNACL_TOOLCHAIN_PATH)/bin/pnacl-finalize $@ $(OUTPUT_OPTION)

clean:
	rm -f main_pnacl.pexe $(OBJECTS)

serve: all
	python $(NACL_SDK_ROOT)/tools/httpd.py -C $(CURDIR)
