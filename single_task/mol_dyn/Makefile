VEO_HEADER_PATH=/opt/nec/ve/veos/include
VEO_LIB_PATH=/opt/nec/ve/veos/lib64
NEO_SYCL = /home/gp.sc.cc.tohoku.ac.jp/lijiahao/workspace/neoSYCL-2021/include

VEFLAGS=-I$(NEO_SYCL) -I$(VEO_HEADER_PATH) -lpthread -L$(VEO_LIB_PATH) -Wl,-rpath=$(VEO_LIB_PATH) -lveo

NCC=/opt/nec/ve/bin/ncc

CXX=g++
# CXXFLAGS = -std=c++17 -DDEBUG 
CXXFLAGS = -std=c++17

TARGET = mol_dyn kernel.so
all:$(TARGET)

mol_dyn:mol_dyn.o
	$(CXX) $(CXXFLAGS) $< -o $@ $(VEFLAGS)
mol_dyn.o:mol_dyn.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(VEFLAGS)
kernel.so:kernel.c
	$(NCC) -fpic -shared -o $@ $< -fopenmp

.PHONY: clean
clean:
	rm -rf *.o $(TARGET)	