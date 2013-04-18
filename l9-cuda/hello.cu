 
#include <stdio.h>
 
//const int N = 16; 
const int blocksize = 16; 
 
__global__ 
void hello(int *a, int *b) 
{
	int z = blockDim.x * blockIdx.x + threadIdx.x;
	a[z] = b[z]* b[z] * b[z];
	//a[threadIdx.x] = 5;
}
 
int main(int argc, char** argv)
{
	int SIZE = atoi(argv[1]);
	int *a =(int*) malloc(SIZE * sizeof(int));
	int *b =(int*) malloc(SIZE * sizeof(int));

	for(int i=0; i<SIZE; i++) {
		a[i]=0;
		b[i]=rand()%100+1;
	}
//	int a[N] = {};
	//int b[N] = {16, 10, 6, 0, -11, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	
 
	int *ad;
	int *bd;
	const int csize = SIZE*sizeof(int);
	const int isize = SIZE*sizeof(int);
 
	printf("input ");
	for (int i=0;i<SIZE;i++)
		printf("%d ", b[i]);
	printf("\n");

	cudaMalloc( (void**)&ad, csize ); 
	cudaMalloc( (void**)&bd, isize ); 
	cudaMemcpy( ad, a, csize, cudaMemcpyHostToDevice ); 
	cudaMemcpy( bd, b, isize, cudaMemcpyHostToDevice ); 
	
	dim3 dimBlock( blocksize, 1 );
	dim3 dimGrid( 1, 1 );
	//hello<<<dimGrid, dimBlock>>>(ad, bd);
	int cores = 512;
	//int blocks = SIZE/cores + 16;
	int blocks = 16;
	int threadsPerBlock = 256;
	//int blocksPerGrid =(SIZE + threadsPerBlock – 1) / threadsPerBlock;

	int blocksPerGrid = ( SIZE + threadsPerBlock -1) / threadsPerBlock;
	printf("blocks = %d cores = %d\n", blocks, cores);
	hello<<<blocksPerGrid, threadsPerBlock>>>(ad, bd);
	cudaMemcpy( a, ad, csize, cudaMemcpyDeviceToHost ); 
	cudaFree( ad );
	cudaFree( bd );
	
	printf("ouput ");
	for (int i=0;i<SIZE;i++)
		printf("%d ", a[i]);
	printf("\n");
	//printf("%s\n", a);
	free(a);
	free(b);
	return EXIT_SUCCESS;
}
