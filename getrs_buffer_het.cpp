// MKL/SYCL includes
#include "oneapi/mkl/lapack.hpp"

// includes
#include <iostream>
#include <vector>
#include <time.h>

void GETRS(cl::sycl::queue &queue_gpu, cl::sycl::queue &queue_host, std::vector<double> A, std::vector<double> b, std::int64_t size, std::int64_t A_size, std::int64_t b_size);

int main(int argc, char* argv[])
{
	std::int64_t size = atoi(argv[1]);
	std::int64_t i, j;

	cl::sycl::queue queue_gpu{cl::sycl::gpu_selector{}};
	cl::sycl::queue queue_host{cl::sycl::host_selector{}};
//	cl::sycl::device device_gpu = queue_gpu.get_device();
	cl::sycl::device device_host = queue_host.get_device();
//	cl::sycl::context context_gpu = queue_gpu.get_context();
	cl::sycl::context context_host = queue_host.get_context();

	std::cout << "Device: "
		  << device_host.get_info<cl::sycl::info::device::name>()
		  << std::endl;

	// ALLOCATE AND INITIALIZE COEFFICIENT MATRIX AND RHS
	std::int64_t coeff_matrix_size = size * size;
	std::int64_t RHS_size = size * size;
	std::vector<double> coeff_matrix(coeff_matrix_size);
	std::vector<double> RHS(RHS_size);

	// Check null vectors
	if (coeff_matrix.empty() || RHS.empty()){
		std::cerr << "Could not alloate memory to array A and b! Not enough memory!\n" << std::endl;
		exit(1);
	}

	// Initialize matrix A to zero
	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++){
		coeff_matrix[i * size + j] = 0.0;}
	}

	// Make A a tridiagonal matrix
	for(i = 0; i < size; i++){
		coeff_matrix[i * size + i] = 2.0;
		if(i > 0){
			coeff_matrix[i * size + i - 1] = -1.0;
		}
	}
	for(i = 0; i < size-1; i++){
		if(i < size -1 ){
			coeff_matrix[i * size + i + 1] = -1.0;
		}
	}

	// Result array overwritten by the solution X
	for(i = 0; i < size; i++){
		RHS[i] = 1.0;
	}

	// solve the tridiagonal matrix
	GETRS(queue_gpu, queue_host, coeff_matrix, RHS, size, coeff_matrix_size, RHS_size);

	return 0;
}

void GETRS(cl::sycl::queue &queue_gpu, cl::sycl::queue &queue_host, std::vector<double> A, std::vector<double> b, std::int64_t size, std::int64_t A_size, std::int64_t b_size)
{
		// oneMKL functions parameters
		std::int64_t m = size, n = size, nrhs = size, lda = size, ldb = size;
		std::int64_t info = 0;

		// timing variables
		clock_t start, end;

			// Asynchronous error handler
			auto error_handler = [&] (cl::sycl::exception_list exceptions) {
				for (auto const& e : exceptions) {
					try{
						std::rethrow_exception(e);
					} catch(oneapi::mkl::lapack::exception const& e) {
						// lapack related
						info = e.info();
				std::cout << "Unexpected exception caught during synchronous call to LAPACK operation:\n" << e.what() << "\ninfo: " << e.info() << std::endl;
					} catch(cl::sycl::exception const& e) {
						// not lapack related
						std::cout << "Unexpected exception caught during asynchronous operation:\n" << e.what() << std::endl;
						info = -1;
					}
				}
			};

			// Allocate and initialize arrays
			std::int64_t ipiv_size = n;
			std::vector<int64_t> ipiv(ipiv_size);

			if(ipiv.empty()){
					std::cerr << "Could not allocate memory to array ipiv and b! Not enough memory!\n" << std::endl;
					exit(1);
			}
 
		       try{
			       // Get sizes of scratchpad
				std::int64_t getrf_scratchpad_size = oneapi::mkl::lapack::getrf_scratchpad_size<double>(queue_gpu, m, n, lda);
				std::int64_t getrs_scratchpad_size = oneapi::mkl::lapack::getrs_scratchpad_size<double>(queue_host, oneapi::mkl::transpose::nontrans, n, nrhs, lda, ldb);

				// Define buffers for the data
				cl::sycl::buffer<double> A_buffer{A.data(), A_size};
				cl::sycl::buffer<int64_t> ipiv_buffer{ipiv.data(), ipiv_size};
				cl::sycl::buffer<double> b_buffer{b.data(), b_size};
				cl::sycl::buffer<double> getrf_scratchpad{getrf_scratchpad_size};
				cl::sycl::buffer<double> getrs_scratchpad{getrs_scratchpad_size};

				// Perform factorization
				start = clock();

				oneapi::mkl::lapack::getrf(queue_gpu, m, n, A_buffer, lda, ipiv_buffer, getrf_scratchpad, getrf_scratchpad_size);

				// Print out factorization
				/*
				for(int j = 0; j < m; j++){
					for(int i = 0; i < n; i++){
						std::cout << A[j*lda+i] << " ";
					}
					std::cout << std::endl;
				}*/
				// Solve the system
				oneapi::mkl::lapack::getrs(queue_host, oneapi::mkl::transpose::nontrans, n, nrhs, A_buffer, lda, ipiv_buffer, b_buffer, ldb, getrs_scratchpad, getrs_scratchpad_size);

				end = clock();

				std::cout << "\n" << "Time taken by getrs BUFFER heterogeneous implementation: " << double(end-start)/double(CLOCKS_PER_SEC) << " seconds. " << "\n" << std::endl;

				// Wait until calculations are done
				queue_host.wait_and_throw();

		       } catch(oneapi::mkl::lapack::exception const &e) {
			       // lapack
			       std::cout << "Unexpected exception caught during synchronous call to LAPACK API:\nreason: " << e.what() << "\ninfo: " << e.info() << std::endl;
			       info = e.info();
		       } catch(cl::sycl::exception const& e) {
			       // not lapack
			       std::cout << "Unexpected exception caught during synchronous call to SYCL API:\n" << e.what() << std::endl;
			       info = -1;
		       }
		       std::cout << "getrs " << ((info == 0) ? "ran OK" : "FAILED") << std::endl;
		       
		       // PRINT RESULT
/*		       std::cout << "\n" << "SOLUTION TO SYSTEM: " << "\n" << std::endl;
		       for(std::int64_t j = 0; j < size; j++){
			       std::cout << b[j] << "\n" << std::endl;
		       } */

}
