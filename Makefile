CXX=dpcpp

MKL_COPTS = -DMKL_ILP64 -I"${MKLROOT}/include"
MKL_LIBS = -L${MKLROOT}/lib/intel64 -lmkl_sycl -lmkl_intel_ilp64 -lmkl_sequential -lmkl_core -lsycl -lOpenCL -lpthread -lm -ldl
DPCPP_OPTS = $(MKL_COPTS) -fsycl -fsycl-device-code-split=per_kernel $(MKL_LIBS)
FLAGS =  $(DPCPP_OPTS)

EXEC_usm = getrs_usm
EXEC_buffer = getrs_buffer
EXEC_usm_het = getrs_usm_het
EXEC_usm_het2 = getrs_usm_het2
EXEC_buf_het = getrs_buf_het

SRC_usm = src/getrs_usm.cpp
SRC_buffer = src/getrs_buffer.cpp
SRC_usm_het = src/getrs_usm_het.cpp
SRC_usm_het2 = src/getrs_usm_het2.cpp
SRC_buf_het = src/getrs_buffer_het.cpp

build_all:
	$(CXX) $(FLAGS) -o $(EXEC_usm) $(SRC_usm)
	$(CXX) $(FLAGS) -o $(EXEC_buffer) $(SRC_buffer)
	$(CXX) $(FLAGS) -o $(EXEC_usm_het) $(SRC_usm_het)
	$(CXX) $(FLAGS) -o $(EXEC_usm_het2) $(SRC_usm_het2)
	$(CXX) $(FLAGS) -o $(EXEC_buf_het) $(SRC_buf_het)

build_usm:
	$(CXX) $(FLAGS) -o $(EXEC_usm) $(SRC_usm)

build_buffer:
	$(CXX) $(FLAGS) -o $(EXEC_buffer) $(SRC_buffer)

build_usm_het:
	$(CXX) $(FLAGS) -o $(EXEC_usm_het) $(SRC_usm_het)

build_usm_het2:
	$(CXX) $(FLAGS) -o $(EXEC_usm_het2) $(SRC_usm_het2)

build_buffer_het:
	$(CXX) $(FLAGS) -o $(EXEC_buf_het) $(SRC_buf_het)

clean_all:
	rm -i $(EXEC_usm) $(EXEC_buffer) $(EXEC_usm_het) $(EXEC_usm_het2) $(EXEC_buf_het)

clean_usm:
	rm-i $(EXEC_usm)
clean_buffer:
	rm -i $(EXEC_buffer)
clean_usm_het:
	rm -i $(EXEC_usm_het)
clean_usm_het2:
	rm -i $(EXEC_usm_het2)
clean_buf_het:
	rm -i $(EXEC_buf_het)
