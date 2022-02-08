/**
*
* oneMKL getrs USM version
*
* Use case - Tridiagonal linear system
*
* Type - Heterogeneous (version 2)
*
**/

// MKL/SYCL includes
#include "oneapi/mkl/lapack.hpp"

// includes
#include <iostream>
#include <time.h>

void GETRS(cl::sycl::queue &queue_gpu, cl::sycl::queue &queue_host, cl::sycl::device &device_gpu, cl::sycl::device &device_host, cl::sycl::context &context_gpu, cl::sycl::context &context_host, std::int64_t info, double *A, double *b, std::int64_t size, std::int64_t A_size, std::int64_t b_size);

int main(int argc, char *argv[])
{
	//Input size at compilation time
	std::int64_t size = atoi(argv[1]);

	std::int64_t i, j, INFO=0;

	//Asynchronous error handler
	auto error_handler = [&] (cl::sycl::exception_list exceptions) {
		for (auto const& e : exceptions) {
			try{
				std::rethrow_exception(e);
			}catch(oneapi::mkl::lapack::exception const& e) {
				// lapack related
				INFO = e.info();
				std::cout << "Unexpected eception caught during asynchronous cal to LAPACK operation:\n" 
					<< e.what() << "\ninfo: " << e.info() << std::endl;
			}catch(cl::sycl::exception const& e) {
				// not lapack related
				std::cout << "Unexpected exception caught during asynchronous operation:\n" 
					<< e.what() << std::endl;
				INFO = -1;
			}
		}
	};

	// Device information
	cl::sycl::queue queue_gpu({cl::sycl::gpu_selector{}}, error_handler);
	cl::sycl::queue queue_host({cl::sycl::host_selector{}}, error_handler);
	cl::sycl::context context_gpu = queue_gpu.get_context();
	cl::sycl::context context_host = queue_host.get_context();
	cl::sycl::device device_gpu = queue_gpu.get_device();
	cl::sycl::device device_host = queue_host.get_device();

	std::cout << "Device: "
		  << device_host.get_info<cl::sycl::info::device::name>()
		  <<std::endl;

	// Allocate and initialize arrays
	std::int64_t coeff_matrix_size = size * size;
	std::int64_t RHS_size = size * size;
	double *coeff_matrix = cl::sycl::malloc_shared<double>(coeff_matrix_size, queue_host);
	double *RHS = cl::sycl::malloc_shared<double>(RHS_size, queue_host);

	// Initialize A to zero
	if( coeff_matrix != NULL){
		for(i = 0; i < size; i++){
			for(j = 0; j < size; j++){
				coeff_matrix[i*size+j] = 0.0;
			}
		}
	}else{
		std::cerr << "Could not allocate memory to coefficient matrix! Out of memory!" << "\n" << std::endl;
		exit(1);
	}

	// Make coefficient matrix a tridiagonal matrix
	for(i = 0; i < size; i++){
		coeff_matrix[i*size+i] = 2.0;
		if(i > 0){
			coeff_matrix[i*size + i-1] = -1.0;}
	}
	for(i = 0; i < size-1; i++){
		if(i < size - 1){
			coeff_matrix[i*size + i+1] = -1.0;}
	}

	// Result array overwritten by the solution X
	if(RHS != NULL){
		for(i = 0; i < size; i++){
			RHS[i] = 1.0;}
	}else{
		std::cerr << "Could not allocate memory to RHS array! Out of memory!" << "\n" << std::endl;
		exit(1);
	}

	// Solve tridiagonal system
	clock_t start, end;

	start = clock();
	GETRS(queue_gpu, queue_host, device_gpu, device_host, context_gpu, context_host, INFO, coeff_matrix, RHS, size, coeff_matrix_size, RHS_size);
	end = clock();

	std::cout << "\n" << "Time taken by getrs heterogeneous USM version 2: " << double(end - start)/double(CLOCKS_PER_SEC) << " seconds. "<< std::endl;
        std::cout << "\n";	

	return 0;
}

void GETRS(cl::sycl::queue &queue_gpu, cl::sycl::queue &queue_host, cl::sycl::device &device_gpu, cl::sycl::device &device_host, cl::sycl::context &context_gpu, cl::sycl::context &context_host, std::int64_t info, double *A, double *b, std::int64_t size, std::int64_t A_size, std::int64_t b_size)
{	
		// oneMKL functions parameters
		std::int64_t m = size, n = size, nrhs = size, lda = size, ldb = size;

			// Allocate ipiv array
			std::int64_t ipiv_size = n * n;
			std::int64_t *ipiv = cl::sycl::malloc_shared<std::int64_t>(ipiv_size, queue_host);
				     
		       try{
			       // Get sizes of scratchpad
				std::int64_t getrf_scratchpad_size = oneapi::mkl::lapack::getrf_scratchpad_size<double>(queue_gpu, m, n, lda);
				double *getrf_scratchpad = cl::sycl::malloc_shared<double>(getrf_scratchpad_size, device_gpu, context_gpu);
				std::int64_t getrs_scratchpad_size = oneapi::mkl::lapack::getrs_scratchpad_size<double>(queue_gpu, oneapi::mkl::transpose::nontrans, n, nrhs, lda, ldb);
				double *getrs_scratchpad = cl::sycl::malloc_shared<double>(getrs_scratchpad_size, device_gpu, context_gpu);

				// Perform factorization
				auto getrf_done_event = oneapi::mkl::lapack::getrf(queue_host, m, n, A, lda, ipiv, getrf_scratchpad, getrf_scratchpad_size);

				// Print out factorization
				/*
				for(int j = 0; j < m; j++){
					for(int i = 0; i < n; i++){
						std::cout << A[j*lda+i] << " ";
					}
					std::cout << std::endl;
				}*/

				// Solve the system
				auto getrs_done_event = oneapi::mkl::lapack::getrs(queue_host, oneapi::mkl::transpose::nontrans, n, nrhs, A, lda, ipiv, b, ldb, getrs_scratchpad, getrs_scratchpad_size);

				// Wait until calculations are done
				getrs_done_event.wait_and_throw();

				//free scatch memory
				cl::sycl::free(getrf_scratchpad, context_gpu);
				cl::sycl::free(getrs_scratchpad, context_gpu);

		       } catch(oneapi::mkl::lapack::exception const &e) {
			       // Handle lapack related exceptions
			       std::cout << "Unexpected exception caught during synchronous call to LAPACK API:\nreason: " << e.what() << "\ninfo: " << e.info() << std::endl;
			       info = e.info();
		       } catch(cl::sycl::exception const& e) {
			       // Handle not lapack related exceptions
			       std::cout << "Unexpected exception caught during synchronous call to SYCL API:\n" << e.what() << std::endl;
			       info = -1;
		       }
		       std::cout << "getrs " << ((info == 0) ? "ran OK" : "FAILED") << std::endl;
		       
		       // PRINT RESULT
		      /*std::cout << "\n" << "SOLUTION TO SYSTEM: " << "\n" << std::endl;
		      for(std::int64_t j = 0; j < size; j++){
			      std::cout << b[j] << "\n" << std::endl;
			      }*/
}
