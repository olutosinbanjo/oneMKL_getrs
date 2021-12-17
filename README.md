# oneMKL_getrs
Homogeneous and Heterogeneous Implementations of a tridiagonal solver on Intel® Xeon® E-2176G with oneMKL getrs() routine

The [getrs()](https://oneapi-src.github.io/oneMKL/domains/lapack/getrs.html#onemkl-lapack-getrs) routine belongs to the oneapi::mkl::lapack namespace, it has a unified
shared memory (USM) and Buffer version. It solves a system of linear equations with an LU-factored square coefficient matrix, with multiple right-hand sides. 
The getrs() routine supports float, double, std::complex<float> and std::complex<double> precisions and can be executed on Host, CPU and GPU devices.
  
Since the getrs() routine works with an LU-factored square coefficient matrix the [getrf()](https://oneapi-src.github.io/oneMKL/domains/lapack/getrf.html#onemkl-lapack-getrf) routine must be called first to compute the LU-factorization of the matrix as A = P*L*U, where P is a permutation matrix, L is the lower triangular matrix with unit diagonal elements (lower trapezoidal if m > n for an m*n matrix) and U is the upper triangular matrix(upper trapezoidal if m < n for an m*n matrix). 
This routine uses partial pivoting with row interchanges, it has a USM and Buffer version and can be executed on Host, CPU and GPU devices like the getrs() routine.
                                                                                                                     
The programs here are implementations of a Tridiagonal solver carried out for an equation of the type, A*X = B, using the buffer and USM versions and all computations are done using double precision. 
                                                                                                                       
## Implementations

Homogeneous implementation: Implementation is executed on either host or gpu device.

Heterogeneous implementation: Queues are used to specify what section of code uses a particular device. 

1. [getrs_usm.cpp](https://github.com/olutosinbanjo/oneMKL_getrs/blob/b7e9a682fa0b619c6bebe2bab996d18f8cf2df87/getrs_usm.cpp): Homogeneous implementation based on Unified Shared Memory version of getrs(). This implementation can be executed on either host or gpu device.

2. [getrs_buffer.cpp](https://github.com/olutosinbanjo/Tridiagonal_solvers/blob/a93de190468e794b69260ae10ef831f02ae0aedd/getrs_solver/getrs_buffer.cpp): Homogeneous implementation based on buffer version of getrs implementation

3. [getrs_usm_het.cpp](https://github.com/olutosinbanjo/Tridiagonal_solvers/blob/a93de190468e794b69260ae10ef831f02ae0aedd/getrs_solver/getrs_usm_het.cpp):  Heterogeneous implementation based on Unified Shared Memory version of getrs(). Queues are used to define that:

  -. The factorization of the matrix is done on the GPU device;
  
  -. The solution of the matrix is computed on the Host device (in other words, do the getrf() on the GPU and the getrs() on the CPU).

For simplicity, I will call this USM Heterogeneous Version 1.

4.  [getrs_usm_het2.cpp](https://github.com/olutosinbanjo/Tridiagonal_solvers/blob/a93de190468e794b69260ae10ef831f02ae0aedd/getrs_solver/getrs_usm_het2.cpp):  Heterogeneous implementation based on Unified Shared Memory version of getrs(). Queues are used to define that:

  -. Memory for the matrix as well as the pivot array is allocated on the GPU device while memory for the right-hand side array is allocated on the Host device;
  
  -. The pointer to the scratchpad memory which is used for storing intermediate results as well as the size of the scratchpad memory for getrf() is allocated on the GPU device while that of getrs() is allocated on the Host device;
  
  -. Both getrf and getrs are performed on the Host device.

For simplicity, I will call this USM Heterogeneous Version 2.

5. [getrs_buffer_het.cpp](https://github.com/olutosinbanjo/Tridiagonal_solvers/blob/5cdb8e76d9ffa07cd31c2a567a6af6385c35f234/getrs_solver/getrs_buffer_het.cpp): Heterogeneous implementation based on buffer version of getrs(). Queues are used to define that:

-. The factorization of the matrix is done on the GPU device;
  
  -. The solution of the matrix is computed on the Host device (in other words, do the getrf() on the GPU and the getrs() on the CPU).

For simplicity, I will call this BUFFER Heterogeneous Version 1.

## Test Report

For timing analysis of the various implementations, see [getrs.pdf](https://github.com/olutosinbanjo/Tridiagonal_solvers/blob/5ad7f795dd7107a3860103532630e3040c29d46e/getrs_solver/getrs.pdf)
