#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "normal.c"

//NORMAL 4 BUCKETS
//10 0.245372
//100 0.221312
//1000 0.272721
//10000 0.247927
//20000 0.311978
//30000  0.295615
//40000 0.308402
//50000 0.267974
//60000 0.348055
//70000 0.359974
//80000 0.418049
//90000 0.421595
//100000 0.429186
//110000  0.550594
//150000 0.609117
//200000 0.920193
//250000 1.263733
//300000 1.596027
//350000 2.151141
//400000 2.589006
//450000 3.210868
//500000 3.934816
//600000 5.238321
//700000 7.320009

//UNIFORMM 4 BUCKETS
//10 0.248607
//100  0.236084
//1000 0.259914
//10000 0.251962
//20000 0.278182
//30000  0.268879
//40000 0.259212
//50000 0.283983
//60000 0.245250
//70000 0.331961
//800000.263934
//90000 0.365239
//100000 0.325433
//110000   0.368437
//150000 0.384665
//200000 0.408849 
//250000 0.402261
//300000 0.504242
//350000 0.468067
//400000 0.608201 
//450000 0.575140
//500000 0.619874
//600000 0.674000
//700000 0.871993



//Number of elements to be sorted
#define SIZE 700000
//The range of the elements to be sorted
#define RANGE 2000
//Number of buckets
#define NUM_BUCKETS 4
//Array holding the elements
int * array;
// Array containing the sorted array
int * resultArray;
// Array containing the samples;
int * sample;
// Array containing the calculated frequencies when distributed uniformly into buckets of equal size
int B[NUM_BUCKETS][NUM_BUCKETS];
// Structure for the bucket
typedef struct
{
int startNum;
int endNum;
int count;
//arrayBucket with size  endNum-startNum
int * arrayBucket;
} Bucket;

Bucket * lstBuckets;

 void quicksort(int * x,int first,int last){
    int pivot,j,temp,i;

     if(first<last){
         pivot=first;
         i=first;
         j=last;

         while(i<j){
             while(x[i]<=x[pivot]&&i<last)
                 i++;
             while(x[j]>x[pivot])
                 j--;
             if(i<j){
                 temp=x[i];
                  x[i]=x[j];
                  x[j]=temp;
             }
         }

         temp=x[pivot];
         x[pivot]=x[j];
         x[j]=temp;
         quicksort(x,first,j-1);
         quicksort(x,j+1,last);

    }
}

int uniform_distribution(int rangeLow, int rangeHigh) {
    double myRand = rand()/(1.0 + RANGE); 
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}

int main (argc, argv)
     int argc;
     char *argv[];
{
	int rank, size;
	int i;
	int time;
	
	float a;
	float b;
	int seed;
	int seed_init = 123465789;
	int * mySample, *myArray;
	a = 1000.0;
	b = 400.0;
	seed = seed_init;

	MPI_Init (&argc, &argv);	/* starts MPI */
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */
	
	if (rank == 0){
		time = timer();
	// Create an array with SIZE elements
	array= malloc(SIZE*sizeof(int));
	
	resultArray= malloc(SIZE*sizeof(int));
	
	int i;
	
	// Assign random values from 1 to/including RANGE to the elements in the array
	for(i=0;i<SIZE;i++){
		//array[i]=rand()%RANGE+1;
		//Normal distribution
		array[i]=i4_normal_ab( a, b, &seed );
		while(array[i]<1||array[i>]RANGE){
		array[i]=i4_normal_ab( a, b, &seed );
		}
		//printf("array[i]:%d\n", array[i]);
		//Uniform distribution
		//array[i]=uniform_distribution(1,RANGE);
	}
	
	}
	int sizeSample=SIZE/NUM_BUCKETS;
	if(rank==0)
	printf("SIZE SAMPLE:%d\n",sizeSample);
	
	//initialize buckets
	lstBuckets= malloc((NUM_BUCKETS)*sizeof(Bucket));
	
	int * splitterArray= malloc((NUM_BUCKETS-1)*sizeof(int));
	
	
	int splitterOffset= sizeSample/(NUM_BUCKETS-1);
	
	int tmpSplitterCounter=0;
	
	
	int sizesMatrix [NUM_BUCKETS][NUM_BUCKETS];
	
	//array that holds the size of final buckets formed after all processes end with sorting the elements into buckets
	int * bucketSizes= malloc(NUM_BUCKETS*sizeof(int));
	int * masterBucketSizes= malloc(NUM_BUCKETS*sizeof(int));
	
	int * myArrayToSort;
	
	if(rank==0){
	//allocate memory for sample
	sample=malloc(sizeSample*sizeof(int));	
	}
	else {
	//printf("size sapleeee %d num b %d res %d \n",sizeSample,NUM_BUCKETS,(sizeSample/NUM_BUCKETS));
	mySample=malloc((sizeSample/NUM_BUCKETS)*sizeof(int));
	if(rank==1)
	printf("SIZE MYSAMPLE:%d\n",(sizeSample/NUM_BUCKETS));
	myArray=malloc((SIZE/NUM_BUCKETS)*sizeof(int));
	}
	
	 int * displs = (int *)malloc((size)*sizeof(int)); 
     int * scounts = (int *)malloc((size)*sizeof(int));
	
	int tmpDispls=0;
	//init displs and scounts for scattering the array
	for(i=0;i<(size);i++){
		displs[i]=tmpDispls;
		if(i==0){
		tmpDispls+=(SIZE-((NUM_BUCKETS-1)*(SIZE/NUM_BUCKETS)));
		scounts[i]=tmpDispls;
		}
		else {
		tmpDispls+=SIZE/NUM_BUCKETS;
		scounts[i]=SIZE/NUM_BUCKETS;
		}
		if(rank==1){
		printf("scounts[%d]=%d\n",i,scounts[i]);
		printf("displs[%d]=%d\n",i,displs[i]);
		}
	}
	
	//scatter arrray to processes 
	
	if(rank==0){
		//ignore root, do not send to itself
	MPI_Scatterv(array, scounts,displs,MPI_INT,MPI_IN_PLACE, SIZE/NUM_BUCKETS,MPI_INT,0, MPI_COMM_WORLD);
	}
	else {
	MPI_Scatterv(array, scounts,displs,MPI_INT,myArray, SIZE/NUM_BUCKETS,MPI_INT,0, MPI_COMM_WORLD);
	
	for(i=0;i<SIZE/NUM_BUCKETS;i++){
		//printf("rank %d: array[%d]=%d\n",rank,i,myArray[i]);
		}
	}
	
	//each process take samples
	
	if(rank==0){
	int masterMySample=sizeSample-((NUM_BUCKETS-1)*(sizeSample/NUM_BUCKETS));
	
	int offset=scounts[0]/masterMySample;
	for(i=0;i<masterMySample;i++){
		sample[i]=array[i*offset];
		//printf("MASTER SAMPLE[%d]: %d \n",i,sample[i]);
	}
	}
	else {
		int offset=(SIZE/NUM_BUCKETS)/(sizeSample/NUM_BUCKETS);
		for(i=0;i<sizeSample/NUM_BUCKETS;i++){
		mySample[i]=myArray[i*offset];
		//printf("RANK %d sample[%d]: %d",rank,i,mySample[i]);
		}
	}
	
	// gather sample in root
	
	 int * displs1 = (int *)malloc((size)*sizeof(int)); 
     int * scounts1 = (int *)malloc((size)*sizeof(int));
     int tmp1=0;
     for(i=0;i<size;i++){
		 displs1[i]=tmp1;
		if(i==0){
			tmp1+=sizeSample-((NUM_BUCKETS-1)*(sizeSample/NUM_BUCKETS));
			scounts1[i]=tmp1;
			
		}
		else {
			tmp1+=sizeSample/NUM_BUCKETS;
			scounts1[i]=sizeSample/NUM_BUCKETS;
		}
	
	}
	
	
	if(rank==0){
        MPI_Gatherv(MPI_IN_PLACE, sizeSample/NUM_BUCKETS, MPI_INT,sample, scounts1, displs1,MPI_INT,0,MPI_COMM_WORLD);
	}
	else {
		MPI_Gatherv(mySample,sizeSample/NUM_BUCKETS, MPI_INT,sample, scounts1, displs1,MPI_INT,0,MPI_COMM_WORLD);
	}
	/*
	 //Check if sample is okay YAY
	if(rank==0){
			for(i=0;i<sizeSample;i++){
				printf("SAMPLEEEE [%d]=%d\n",i,sample[i]);
			}
		}
	*/
	
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank==0){
		quicksort(sample,0,sizeSample-1);
		// create splitters 
		//choose num of num of buckets -1 values from the sample
		for(i=0;i<NUM_BUCKETS-1;i++){
		splitterArray[i]=sample[tmpSplitterCounter];
		tmpSplitterCounter+=splitterOffset;
		}
		
	}
	
	MPI_Bcast(splitterArray,NUM_BUCKETS-1, MPI_INT, 0, MPI_COMM_WORLD);
	
	for(i=0;i<NUM_BUCKETS-1;i++){
		//printf("RANK  %d SPLITTER_ARRAY [%d]=%d\n", rank, i, splitterArray[i]);
		
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	for(i=0;i<NUM_BUCKETS;i++){
	Bucket bucket; 
	if(i==0){
	bucket.startNum=1;
	bucket.endNum= splitterArray[i];
	}
	else if(i==NUM_BUCKETS-1){
	bucket.startNum= splitterArray[i-1]+1;
	bucket.endNum=RANGE;
	}
	else {
	bucket.startNum= splitterArray[i-1]+1;
	bucket.endNum= splitterArray[i];
	}
	bucket.count=0;
	
	bucket.arrayBucket=malloc(((SIZE/NUM_BUCKETS)+((SIZE/NUM_BUCKETS)/2))*sizeof(int));
	lstBuckets[i]=bucket;
	}  
	
	 
	if(rank==0)
	for(i=0;i<NUM_BUCKETS;i++){
	Bucket b=lstBuckets[i];
	//printf("BUCKET %d: \n",i);
	//printf("start=%d\n",b.startNum);
	//printf("end=%d\n",b.endNum);
	}
	
	
	if(rank==0){
		int j;
		
		int s=(SIZE-((NUM_BUCKETS-1)*(SIZE/NUM_BUCKETS)));
		//printf("Size of master: %d",s);
		
		for(i=0;i<s;i++){
		//iterating over the number of buckets
		for(j=0;j<NUM_BUCKETS;j++){
			
			Bucket b=lstBuckets[j];
			if(array[i]>=b.startNum&&array[i]<=b.endNum){
			//printf("array[%d]=%d WENT INTO BUCKET %d\n",i,array[i],j);
			//put element in the array
			lstBuckets[j].arrayBucket[b.count]=array[i];
			//increase count1
			lstBuckets[j].count++;
			break;
			}
		}
	}
	
	for(i=0;i<NUM_BUCKETS;i++){
		Bucket b=lstBuckets[i];
		bucketSizes[i]=b.count;
		//for(j=0;j<b.count;j++){
			
		//	printf("Buckettttt %d : array[%d]= %d \n",i,j,b.arrayBucket[j]);
		//}
	}
	for(i=0;i<NUM_BUCKETS;i++){
	masterBucketSizes[i]=0;
	}
		
	}
	else {
	int j;
	for(i=0;i<SIZE/NUM_BUCKETS;i++){
		//iterating over the number of buckets
		for(j=0;j<NUM_BUCKETS;j++){
			Bucket b=lstBuckets[j];
			if(myArray[i]>=b.startNum&&myArray[i]<=b.endNum){
			//printf("myarray[%d]=%d WENT INTO BUCKET %d\n",i,myArray[i],j);
			//put element in the array
			lstBuckets[j].arrayBucket[b.count]=myArray[i];
			//increase count1
			lstBuckets[j].count++;
			break;
			}
		}
	}
	
	for(i=0;i<NUM_BUCKETS;i++){
		Bucket b=lstBuckets[i];
		for(j=0;j<b.count;j++){
		//	printf("RANK %d Bucket %d : array[%d]= %d \n",rank,i,j,b.arrayBucket[j]);
		}
	}
	
	for(i=0;i<NUM_BUCKETS;i++){
		Bucket b=lstBuckets[i];
		//printf("RANK %d bucket %d size %d ",rank,i,lstBuckets[i].count);
		bucketSizes[i]=b.count;
	}
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	//get number of elements in each bucket , mpi reduce
	
	int * finalTest= malloc((NUM_BUCKETS*NUM_BUCKETS)*sizeof(int));
	MPI_Gather(bucketSizes, NUM_BUCKETS, MPI_INT,finalTest, NUM_BUCKETS, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	//if(rank==0){
	//	int sum=0;
	//for(i=0;i<NUM_BUCKETS*NUM_BUCKETS;i++){
		//printf("TESTTT [%d]=%d\n",i,finalTest[i]);
		//sum+=finalTest[i];
	//}
	//printf("SUM =%d\n",sum);
	//}
	
	
	MPI_Reduce( bucketSizes, masterBucketSizes, NUM_BUCKETS, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	
	MPI_Barrier(MPI_COMM_WORLD);
	//if(rank==0)
	//for(i=0;i<NUM_BUCKETS;i++){
		
		//printf("FINAL NUM IN BUCKETS [%d]=%d\n",i,masterBucketSizes[i]);
		
		//}
	if(rank==0){
		for(i=0;i<NUM_BUCKETS;i++){
		Bucket b=lstBuckets[i];
		bucketSizes[i]=b.count;
		}
		
		//for(i=0;i<NUM_BUCKETS;i++){
		
		//printf("FINAL NUM IN BUCKETS [%d]=%d\n",i,masterBucketSizes[i]);
		
		//}
	}
	
	MPI_Bcast(masterBucketSizes,NUM_BUCKETS, MPI_INT, 0, MPI_COMM_WORLD);
	
	myArrayToSort= malloc(masterBucketSizes[rank]*sizeof(int));
	
	for(i=0;i<NUM_BUCKETS;i++){
	//printf("B rank");
	B[rank][i]=bucketSizes[i];	
	//printf("MATRICA[%d][%d]=%d \n",rank,i,B[rank][i]);
		
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Status status;
	
	if(rank==0){
		int j;
	for(i=0;i<NUM_BUCKETS;i++){
			for(j=0;j<NUM_BUCKETS;j++){
		//		printf(" A   RANK %d, m[%d][%d]=%d ",rank,i,j,B[i][j]);
				}
				//	printf("\n");	
			}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank==1){
		int j;
	for(i=0;i<NUM_BUCKETS;i++){
			for(j=0;j<NUM_BUCKETS;j++){
		//		printf(" A    RANK %d, m[%d][%d]=%d ",rank,i,j,B[i][j]);
				}
					printf("\n");	
			}
	}
	
	int * sizeArrayM= malloc((NUM_BUCKETS*NUM_BUCKETS)*sizeof(int));
	
    MPI_Gather(bucketSizes, NUM_BUCKETS, MPI_INT,sizeArrayM, NUM_BUCKETS, MPI_INT,0, MPI_COMM_WORLD);

	MPI_Bcast(sizeArrayM,NUM_BUCKETS*NUM_BUCKETS, MPI_INT, 0, MPI_COMM_WORLD);
	
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	if(rank==1)
	for(i=0;i<NUM_BUCKETS*NUM_BUCKETS;i++){
	//	printf("SIZES [%d]=%d \n", i, sizeArrayM[i]);
		
	}
	MPI_Barrier(MPI_COMM_WORLD);
	// TEST MPI SEND 
	/*
	int * test= malloc(1*sizeof(int));
	
	if(rank==0){
		MPI_Send(&bucketSizes[2], 1, MPI_INT, 1,0, MPI_COMM_WORLD);
	}
	if(rank==1){
		 MPI_Recv(&test[0], 1, MPI_INT, 0,0,MPI_COMM_WORLD,&status);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank==1)
	printf("TEST %d", test[0]);
	*/
	
    // If we are the root process, send our data to everyone
    
    
    /*
     int * displs2 = (int *)malloc((size)*sizeof(int)); 
     int * scounts2 = (int *)malloc((size)*sizeof(int));
     int j;
     int offsetIndex=0;
     int tmp2=rank;
     for(i=0;i<size;i++){
	displs2[i]=offsetIndex;
	scounts2[i]=sizeArrayM[(tmp2/NUM_BUCKETS)+(rank%NUM_BUCKETS)];
	tmp2+=NUM_BUCKETS;
	offsetIndex+=scounts[i];
	}
	
	MPI_Gatherv(lstBuckets[rank].arrayBucket,, MPI_INT,myArrayToSort, scounts, displs,MPI_INT,rank,MPI_COMM_WORLD);
     
    */
      MPI_Request * reqs1;
      MPI_Request * reqs2;
      reqs1= malloc((NUM_BUCKETS*(NUM_BUCKETS-1))*sizeof(MPI_Request));
      reqs2= malloc((NUM_BUCKETS*(NUM_BUCKETS-1))*sizeof(MPI_Request));
      for(i=0;i<(NUM_BUCKETS*(NUM_BUCKETS-1));i++){
	  MPI_Request req1;
	  reqs1[i]=req1;
	  MPI_Request req2;
	  reqs2[i]=req2;
	  }
    
    
    int j;
    for (i=0;i<size;i++) {
		int offset=0;
		
		int indCount=(rank*NUM_BUCKETS)+(i%NUM_BUCKETS);
		int rindCount=(i*NUM_BUCKETS)+(rank%NUM_BUCKETS);
		for(j=0;j<i;j++){
		
		offset+=sizeArrayM[(j*NUM_BUCKETS)+(rank%NUM_BUCKETS)];
		}
	
	//printf("RANK %d, III: %d, OFFSET: %d, indCount: %d, rind %d, count %d\n",rank,i,offset, indCount, rindCount,lstBuckets[i].count);
      if (i !=rank) {
		  
        MPI_Isend(lstBuckets[i].arrayBucket,lstBuckets[i].count, MPI_INT, i, indCount, MPI_COMM_WORLD, &reqs1[indCount]);
        
        
		
      }
      else {
		
		 int j;
		 int offset=0;
		 for(j=0;j<rank;j++){
		
		offset+=sizeArrayM[(j*NUM_BUCKETS)+(rank%NUM_BUCKETS)];
		}
		 for(j=0;j<lstBuckets[rank].count;j++){
		 myArrayToSort[offset+j]=lstBuckets[rank].arrayBucket[j];
		 }
		
	  }
    }
    
    for (i=0;i<size;i++) {
		int offset=0;
		
		
		int rindCount=(i*NUM_BUCKETS)+(rank%NUM_BUCKETS);
		for(j=0;j<i;j++){
		
		offset+=sizeArrayM[(j*NUM_BUCKETS)+(rank%NUM_BUCKETS)];
		}
		if (i !=rank) {
		//	printf("RECEIVE RANK: %d, IIII: %d, offset: %d , rindCount: %d\n",rank,i,offset,rindCount);
   MPI_Recv(&myArrayToSort[offset], sizeArrayM[rindCount],MPI_INT, i, rindCount,MPI_COMM_WORLD,&status);
     }
      
    }
     MPI_Barrier(MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);
	if(rank==1){
	for(i=0;i<masterBucketSizes[rank];i++){
	//printf("MY ARRAY TO SORT [%d]=%d\n",i,myArrayToSort[i]);
	}
	}
    
		quicksort(myArrayToSort,0,masterBucketSizes[rank]-1);
		
		
		    MPI_Barrier(MPI_COMM_WORLD);
		if(rank>0){
			MPI_Isend(myArrayToSort,masterBucketSizes[rank], MPI_INT, 0, rank, MPI_COMM_WORLD, &reqs1[rank]);
		}
		
		if(rank==0){
			int j;
			for(i=0;i<masterBucketSizes[0];i++){
			resultArray[i]=myArrayToSort[i];
			}
			
			for(i=1;i<NUM_BUCKETS;i++){
				int tmp5=0;
				for(j=0;j<i;j++){
					tmp5+=masterBucketSizes[j];
				}
			MPI_Recv(&resultArray[tmp5], masterBucketSizes[i],MPI_INT, i, i,MPI_COMM_WORLD,&status);
			}
		}
	
		
		
		
		
		
		
	
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	if (rank == 0)
	{
		time = timer() - time;
        // check
	/*
	if(rank==0){
		quicksort(array,0,SIZE-1);
	for(i=0;i<SIZE;i++){
			printf(" ARRAY [%d]=%d\n",i,array[i]);
			printf("FINAL ARRAY [%d]=%d\n",i,resultArray[i]);
			}
		}
	*/
	
		printf("Total runtime: %f seconds\n", time/1000000.0);
		
	}
	
	MPI_Finalize();
	return 0;
}
