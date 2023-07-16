// Name: HERNANDEZ, Pierre Vincent C.		CEPARCO - S11
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/* @brief x86-64 SIMD assembly function for 1-D stencil operation on 1 vector.
* 
* Linked to an assembly file consisting the code implementation of 1-D stencil
* in SIMD assembly.
* 
* @param Y The address of the output vector
* @param X The address of the input vector
* @param n number of elements of the vector
* @return Void
*/
extern void stencil_1D_x86_64_SIMD(int n, long long int* Y, long long int* X);


/* @brief x86-64 assembly function for 1-D stencil operation on 1 vector.
* 
* Linked to an assembly file consisting the code implementation of 1-D stencil
* in assembly.
* 
* @param Y The address of the output vector
* @param X The address of the input vector
* @param n number of elements of the vector
* @return Void
*/
extern void stencil_1D_x86_64(int n, long long int* Y, long long int* X);


/* @brief C function for 1-D stencil operation on 1 vector.
* 
* 1-D stencil operation is defined as: 
* Y[i] = X[i-3] + X[i-2] + X[i-1] + X[i] + X[i+1] + X[i+2] + X[i+3]  (1-D Stencil equation)
* 
* The function considers the halo elements on the output vector Y.
* This means that the 1-D stencil equation will not be performed
* on the first and last 3 elements (halo elements) but it still in respect
* with the indexing of the equation above. Thus, the start of the indexing
* for output vector Y is i = 3.
* 
* The implementation of the equation is altered such that the starting index
* for the X vector is at 0 when index of Y is at 3 -- X is at X[i-3] or X[0].
* This gives an offset for X indexing of i-3, which will result to:
* Y[3]  = X[0] + X[1] + X[2] + X[3] + X[4] + X[5] + X[6]
* Thus, new stencil equation can be:
* Y[iy] = X[ix] + X[ix + 1] + X[ix + 2] + X[ix + 3] + X[ix + 4] + X[ix + 5] + X[ix + 6], 
* where ix = iy - 3.
* 
* @param Y The address of the output vector
* @param X The address of the input vector
* @param n number of elements of the vector
* @return Void
*/
void stencil_1D_C(long long int* Y, long long int* X, int n) {
	int max = n - 3; // don't include last 3 elements (halo elements)
	// Halo values (i.e. 0) at the first and last 3 elements of Y 
	// are not included in Y vector variable
	for (int ix = 0, iy = 3; iy < max; ix++, iy++)
		Y[iy] = X[ix] + X[ix + 1] + X[ix + 2] + X[ix + 3] + X[ix + 4] + X[ix + 5] + X[ix + 6];
}


/* @brief Function for checking errors (i.e. output results not equal to expected output)
* 
* The halo elements are still in the Y vector but the only elements that was checked
* was the non-halo elements -- first and last 3 elements are not checked
* 
* @param Y The address of the output vector
* @param X The address of the input vector
* @param n number of elements of the vector
* @return number of incorrect elements in Y vector
*/
unsigned int count_err(long long int* Y, long long int* X, int n) {
	unsigned int err_count = 0;
	int last_ind = n - 3;

	// count errors (i.e. output results not equal to expected output)
	for (int i = 3; i < last_ind; i++) {
		if (Y[i] != (X[i - 3] + X[i - 2] + X[i - 1] + X[i] + X[i + 1] + X[i + 2] + X[i + 3])) {
			err_count++;
			printf("Error at index [%d]: output = % lld; expected = % lld\n", i, Y[i], 
				X[i - 3] + X[i - 2] + X[i - 1] + X[i] + X[i + 1] + X[i + 2] + X[i + 3]);
		}
	}

	return err_count;
}


int main(void) {
	const int n = 1<<24; // 1<<20; 1<<24; 1<<30
	const long long int N_BYTES = n * sizeof(long long int); // bytes to be allocated for each vector
	const int RUN_COUNT = 30; // number of runs

	// Allocate memory for vectors
	long long int *X, *Y;
	X = (long long int*)malloc(N_BYTES);
	Y = (long long int*)malloc(N_BYTES);

	// Check if successfully allocated memory for vector X
	if (X == NULL) {
		printf("\n[ERROR]: Not enough memory to allocate for vector X.\n");
		return 1; // return with 1 error;
	}

	// Check if successfully allocated memory for vector X
	if (Y == NULL) {
		printf("\n[ERROR]: Not enough memory to allocate for vector Y.\n");
		return 1; // return with  1 error;
	}

	// Initialize X and Y vectors
	for (int i = 0; i < n; i++) {
		X[i] = (long long int)(i + 1);
		Y[i] = 0L;
	}

	// Display current specs
	printf("\n=== 1-D Stencil ===\n");
	printf("Number of elements (n):  %d\n", n);
	printf("Byte size per vector:  %lld\n", N_BYTES);
	printf("Number of runs:  %d\n\n", RUN_COUNT);

	// timer variables
	clock_t start, end;
	double time_taken = 0;

	// === Implement 1-D stencil using C ===
	printf("-- C version --\n");
	for (int i = 0; i < RUN_COUNT; i++) {
		start = clock();
		stencil_1D_C(Y, X, n);
		end = clock();
		// Total execution time
		time_taken += ((double)(end - start)) * 1e6 / CLOCKS_PER_SEC; // in microseconds
	}

	// Calculate and display average execution time
	time_taken = time_taken / (double)RUN_COUNT;
	printf("Average execution time in microseconds (C program):  %lf uS\n", time_taken);

	// Debugging purposes: Print first 6 non-halo elements
	printf("Output (first 10 non-halo elements)");
	for (int i = 0; i < 10; i++) {
		printf("%lld ", Y[i + 3]);
	}
	printf("\n");

	// Count and display number of errors (i.e. output results not equal to expected output)
	unsigned int err_count = count_err(Y, X, n);
	printf("Error Count (C program):  %u\n\n", err_count);

	// Reset time and counter values
	time_taken = 0;
	err_count = 0;

	// Reset Y vector
	for (int i = 0; i < n - 1; i++) {
		Y[i] = 0L;
	}



	// === Implement 1-D stencil using x86-64 assembly ===
	printf("-- x86-64 version --\n");
	for (int i = 0; i < RUN_COUNT; i++) {
		start = clock();
		stencil_1D_x86_64(n, Y, X);
		end = clock();
		// Total execution time
		time_taken += ((double)(end - start)) * 1e6 / CLOCKS_PER_SEC; // in microseconds
	}

	// Calculate and display average execution time
	time_taken = time_taken / (double)RUN_COUNT;
	printf("Average execution time in microseconds (x86-64 program):  %lf uS\n", time_taken);

	// Debugging purposes: Print first 6 non-halo elements
	printf("Output (first 10 non-halo elements)");
	for (int i = 0; i < 10; i++) {
		printf("%lld ", Y[i + 3]);
	}
	printf("\n");

	// Count and display number of errors (i.e. output results not equal to expected output)
	err_count = count_err(Y, X, n);
	printf("Error Count (x86-64 program):  %u\n\n", err_count);

	// Reset time and counter values
	time_taken = 0;
	err_count = 0;

	// Reset Y vector
	for (int i = 0; i < n - 1; i++) {
		Y[i] = 0L;
	}



	// === Implement 1-D stencil using x86-64 SIMD assembly ===
	printf("-- x86-64 SIMD version --\n");
	for (int i = 0; i < RUN_COUNT; i++) {
		start = clock();
		stencil_1D_x86_64_SIMD(n, Y, X);
		end = clock();
		// Total execution time
		time_taken += ((double)(end - start)) * 1e6 / CLOCKS_PER_SEC; // in microseconds
	}

	// Calculate and display average execution time
	time_taken = time_taken / (double)RUN_COUNT;
	printf("Average execution time in microseconds (x86-64 SIMD program):  %lf uS\n", time_taken);

	// Debugging purposes: Print first 6 non-halo elements
	printf("Output (first 10 non-halo elements)");
	for (int i = 0; i < 10; i++) {
		printf("%lld ", Y[i + 3]);
	}
	printf("\n");

	// Count and display number of errors (i.e. output results not equal to expected output)
	err_count = count_err(Y, X, n);
	printf("Error Count (x86-64 SIMD program):  %u\n\n", err_count);



	// Deallocate X and Y vectors from memory
	free(X);
	free(Y);

	return 0;
}