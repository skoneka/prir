// This is the REAL "hello world" for CUDA!
// It takes the string "Hello ", prints it, then passes it to CUDA with an array
// of offsets. Then the offsets are added in parallel to produce the string "World!"
// By Ingemar Ragnemalm 2010
 
#include <stdio.h>
 
const int N = 16; 
const int blocksize = 16; 
 
__global__ 
void hello(int *a, int *b) 
{
	a[threadIdx.x] = b[threadIdx.x]*b[threadIdx.x]*b[threadIdx.x];
	//a[threadIdx.x] = 5;
}
 
int main()
{
	int a[N] = {};
	int b[N] = {16, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 
	int *ad;
	int *bd;
	const int csize = N*sizeof(int);
	const int isize = N*sizeof(int);
 
	printf("input ");
	for (int i=0;i<N;i++)
		printf("%d ", b[i]);
	printf("\n");

	cudaMalloc( (void**)&ad, csize ); 
	cudaMalloc( (void**)&bd, isize ); 
	cudaMemcpy( ad, a, csize, cudaMemcpyHostToDevice ); 
	cudaMemcpy( bd, b, isize, cudaMemcpyHostToDevice ); 
	
	dim3 dimBlock( blocksize, 1 );
	dim3 dimGrid( 1, 1 );
	//hello<<<dimGrid, dimBlock>>>(ad, bd);
	hello<<<1, 16>>>(ad, bd);
	cudaMemcpy( a, ad, csize, cudaMemcpyDeviceToHost ); 
	cudaFree( ad );
	cudaFree( bd );
	
	printf("ouput ");
	for (int i=0;i<N;i++)
		printf("%d ", a[i]);
	printf("\n");
	//printf("%s\n", a);
	return EXIT_SUCCESS;
}
