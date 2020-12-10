CUDA_INSTALL_PATH	=				/usr/local/cuda
CC					=				g++
OPTFLAG				=				-O2 -fomit-frame-pointer -ftree-vectorize -ftree-vectorizer-verbose=0 -funroll-loops
NVCC				=				${CUDA_INSTALL_PATH}/bin/nvcc
FLAGS				=				${OPTFLAG} -I ${CUDA_INSTALL_PATH}/include -Wall -g
NVFLAGS				=				-O2 -arch=sm_20 -I ${CUDA_INSTALL_PATH}/include --compiler-options -fno-strict-aliasing --ptxas-options=-v -g
LFLAGS				=				-L ${CUDA_INSTALL_PATH}/lib${LIBSUFFIX} -lm -lstdc++ -lcudart
CLEAN_FILES			=				arg_check.o grid.o main.o update_cpu.o update_gpu.o heat
BITS = $(shell getconf LONG_BIT)
ifeq (${BITS},64)
	LIBSUFFIX := 64
endif

#	-g flag enables debugging mode
#	-c flag generates object code for separate files
heat: arg_check.o grid.o main.o update_cpu.o update_gpu.o
	${CC} ${LFLAGS} -fopenmp -o $@ $^ -lm

#	create/compile the individual files >>separately<< 
arg_check.o: arg_check.cpp
	${CC} -c ${FLAGS} -o $@ $^

grid.o: grid.cpp
	${CC} -c ${FLAGS} -o $@ $^

main.o: main.cpp
	${CC} -c ${FLAGS} -o $@ $^
	
update_cpu.o: update_cpu.cpp
	${CC} -c ${FLAGS} -fopenmp -o $@ $^

update_gpu.o: update_gpu.cu
	${NVCC} ${NVFLAGS} -DUNIX -c $^ -o $@

#	clean house
clean:
	rm -f $(CLEAN_FILES) *.dat
