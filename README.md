## oneMKL_getrs for tridiagonal linear system
### Homogeneous and Heterogeneous Implementations of a tridiagonal solver on Intel® Xeon® E-2176G with oneMKL getrs() routine

The [getrs()](https://oneapi-src.github.io/oneMKL/domains/lapack/getrs.html#onemkl-lapack-getrs) routine belongs to the oneapi::mkl::lapack namespace, it has a unified
shared memory (USM) and Buffer version. It solves a system of linear equations with an LU-factored square coefficient matrix, with multiple right-hand sides. 
The getrs() routine supports float, double, std::complex<float> and std::complex<double> precisions and can be executed on Host, CPU and GPU devices.
  
Since the getrs() routine works with an LU-factored square coefficient matrix the [getrf()](https://oneapi-src.github.io/oneMKL/domains/lapack/getrf.html#onemkl-lapack-getrf) routine must be called first to compute the LU-factorization of the matrix as A = P*L*U, where P is a permutation matrix, L is the lower triangular matrix with unit diagonal elements (lower trapezoidal if m > n for an m*n matrix) and U is the upper triangular matrix(upper trapezoidal if m < n for an m*n matrix). 
This routine uses partial pivoting with row interchanges, it has a USM and Buffer version and can be executed on Host, CPU and GPU devices like the getrs() routine.
                                                                                                                     
The programs here are implementations of a Tridiagonal solver carried out for an equation of the type, A*X = B, using the buffer and USM versions and all computations are done using double precision. 
                                                                                                                       
### Implementations

Homogeneous implementation: Implementation is executed on either host or gpu device.

Heterogeneous implementation: Queues are used to specify what section of code uses a particular device. 

1. [getrs_usm.cpp](https://github.com/olutosinbanjo/oneMKL_getrs/blob/b30786fab72070924d22037e62b349c70fc0ee7a/src/getrs_usm.cpp): Homogeneous implementation based on Unified Shared Memory version of getrs(). This implementation can be executed on either host or gpu device.

2. [getrs_buffer.cpp](https://github.com/olutosinbanjo/oneMKL_getrs/blob/b30786fab72070924d22037e62b349c70fc0ee7a/src/getrs_buffer.cpp): Homogeneous implementation based on buffer version of getrs implementation

3. [getrs_buffer_het.cpp](https://github.com/olutosinbanjo/oneMKL_getrs/blob/b30786fab72070924d22037e62b349c70fc0ee7a/src/getrs_buffer_het.cpp): Heterogeneous implementation based on buffer version of getrs(). Queues are used to define that:

  -. The factorization of the matrix is done on the GPU device;
  
  -. The solution of the matrix is computed on the Host device (in other words, do the getrf() on the GPU and the getrs() on the CPU).


4. [getrs_usm_het.cpp](https://github.com/olutosinbanjo/oneMKL_getrs/blob/b30786fab72070924d22037e62b349c70fc0ee7a/src/getrs_usm_het.cpp):  Heterogeneous implementation based on Unified Shared Memory version of getrs(). Queues are used to define that:

  -. The factorization of the matrix is done on the GPU device;
  
  -. The solution of the matrix is computed on the Host device (in other words, do the getrf() on the GPU and the getrs() on the CPU).
  
  In this heterogeneous version, memory for the matrix and pivot array are accessed on the gpu device, while that of the right-hand side array is accessed on the host device. Also, the pointer to the scratchpad memory which is used for storing intermediate results as well as the size of the scratchpad memory for getrf() is accessed on the GPU device, while that of getrs() is accessed on the CPU device. 

For simplicity, this is called USM Heterogeneous Version 1 in the report.

5.  [getrs_usm_het2.cpp](https://github.com/olutosinbanjo/oneMKL_getrs/blob/b30786fab72070924d22037e62b349c70fc0ee7a/src/getrs_usm_het2.cpp):  Heterogeneous implementation based on Unified Shared Memory version of getrs(). Queues are used to define that:

  -. Memory for the matrix, pivot and right-hand side array is accessed on the Host device;
  
  -. The pointer to the scratchpad memory which is used for storing intermediate results as well as the size of the scratchpad memory for both getrf() and getrs() is accessed on the GPU device.
  
  -. Both getrf and getrs are computed on the Host device.

For simplicity, I will call this USM Heterogeneous Version 2.

### Test timing Report

For timing analysis of the various implementations, see [getrs_report.pdf](https://github.com/olutosinbanjo/oneMKL_getrs/blob/e2a4d7d9438d54d3539fa56013ca98a08cbbcad1/getrs_report.pdf)

### Build Program on the intel devcloud

1. Connect to devcloud

```bash
ssh devcloud
```

2. Clone this repository

```bash
git clone https://github.com/olutosinbanjo/oneMKL_getrs.git && cd oneMKL_getrs
```

3. Request computational node

```bash
qsub -I -l nodes=1:gpu:ppn=2 -d .
```

4. Build programs

```bash
./build.sh
```

5. Run executable (for example)

```bash
cd bin
./getrs_usm n (n = size of tridiagonal system)
```

6. Remove executables

```bash
./clean.sh
```

### Overall Best Implementation

Relative to the choice of device and data size. [See report](https://github.com/olutosinbanjo/oneMKL_getrs/blob/f45fe4f71195f6996e14dc7e4710bf341a646b7f/getrs_report.pdf)

### Intel DevCloud 

[Sign up for Intel DevCloud for oneAPI](https://www.intel.com/content/www/us/en/forms/idz/devcloud-enrollment/oneapi-request.html)

### Learn DPC++

J. Reinders, B. Ashbaugh, J. Brodman, M. Kinsner, J. Pennycook and X.
Tian, Data Parallel C++: Mastering DPC++ for Programming of
Heterogenous Systems using C++ and SYCL. Berkely, CA: Apress, 2021.
[Online]. Available: https://doi.org/10.1007/978-1-4842-5574-2 

