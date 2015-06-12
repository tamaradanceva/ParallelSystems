#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "time.c"
# include "normal.c"

//Number of elements to be sorted
long int SIZE =100000;
//The range of the elements to be sorted
long int RANGE =2000;
//Number of buckets
int NUM_BUCKETS=100;
//Array holding the elements
int * array;
// Array containing the sorted array
int * resultArray;
// Array containing the samples;
int * sample;
// Array containing the calculated frequencies when distributed uniformly into buckets of equal size

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
    double myRand = rand()/(1.0 + RAND_MAX); 
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}

int main(){
	float a;
	float b;
	int seed;
	int seed_init = 123465789;

	a = 1000.0;
	b = 400.0;
	seed = seed_init;
	//int time1=timer();
	
	// Create an array with SIZE elements
	array= malloc(SIZE*sizeof(int));
	int i;
	srand(time(NULL));

	// Assign random values from 1 to/including RANGE to the elements in the array
	for(i=0;i<SIZE;i++){
		//array[i]=rand()%RANGE+1;
		//Normal distribution
		array[i]=i4_normal_ab( a, b, &seed );
		while(array[i]<1||array[i>]RANGE){
		array[i]=i4_normal_ab( a, b, &seed );
		}
		//printf("array[i]:%d", array[i]);
		//Uniform distribution
	//	array[i]=uniform_distribution(1,RANGE);

	}

	//int factor=RANGE/SIZE;
	// Size of the sample to be taken
	//int sizeSample= SIZE/(factor/(SIZE/100))-1;
	int sizeSample=SIZE/NUM_BUCKETS;
	
	
	//printf("size sample: %d\n",sizeSample);
	//allocate memory for sample
	sample=malloc(sizeSample*sizeof(int));
	//take every offset element
	int offset=SIZE/sizeSample;
	
	//printf("offset: %d\n",offset);
	//printf("NUM OF BUCKETS %d",NUM_BUCKETS);

	//initialize buckets
	lstBuckets= malloc((NUM_BUCKETS)*sizeof(Bucket));
	
	
	//creating the resulting array
	resultArray=malloc(SIZE*sizeof(int));
	// create splitters array
	
	int * splitterArray= malloc((NUM_BUCKETS-1)*sizeof(int));
	
	
	int splitterOffset= sizeSample/(NUM_BUCKETS-1);
	
	int tmpSplitterCounter=0;
	
	int time1=timer();
	
	// take sample 
	for(i=0;i<sizeSample;i++){
	sample[i]=array[i*offset];
	}
	
	//printf("BEFORE SAMPLE SORT");
	
	quicksort(sample,0,sizeSample-1);
	
	//printf("AFTER SAMPLE SORT");
	
	//choose num of num of buckets -1 values from the sample
	for(i=0;i<NUM_BUCKETS-1;i++){
	splitterArray[i]=sample[tmpSplitterCounter];
	tmpSplitterCounter+=splitterOffset;
	}
	
	//printf("AFTER SPLITTER CREATION");
	
	//for(i=0;i<sizeSample;i++){
		//printf("SampleAfter: %d\n",sample[i]);
	//}
	
	for(i=0;i<NUM_BUCKETS;i++){
		//printf("num buckets %d",NUM_BUCKETS);
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
	bucket.arrayBucket=malloc(((SIZE/NUM_BUCKETS)+(SIZE/NUM_BUCKETS)/3)*sizeof(int));
	lstBuckets[i]=bucket;
	}   
	
	/*
	for(i=0;i<NUM_BUCKETS;i++){
	Bucket b=lstBuckets[i];
	printf("BUCKET %d: \n",i);
	printf("start=%d\n",b.startNum);
	printf("end=%d\n",b.endNum);
	}*/
	
	int j;
	for(i=0;i<SIZE;i++){
		//iterating over the number of buckets
		for(j=0;j<NUM_BUCKETS;j++){
			Bucket b=lstBuckets[j];
			if(array[i]>=b.startNum&&array[i]<=b.endNum){
			//put element in the array
			lstBuckets[j].arrayBucket[b.count]=array[i];
			//increase count1
			lstBuckets[j].count++;
			}
		}
	}
	
	/*
	for(i=0;i<NUM_BUCKETS;i++){
		Bucket b=lstBuckets[i];
		for(j=0;j<b.count;j++){
			//printf("Bucket %d : array[%d]= %d \n",i,j,b.arrayBucket[j]);
		}
	}*/
	
	for(i=0;i<NUM_BUCKETS;i++){
		quicksort(lstBuckets[i].arrayBucket, 0,lstBuckets[i].count-1);
	}
	
	
	
	//creating the resulting array combining all the buckets
	int tmpCount=0;
	for(i=0;i<NUM_BUCKETS;i++){
		Bucket b=lstBuckets[i];
		for(j=0;j<b.count;j++){
			resultArray[tmpCount]=b.arrayBucket[j];
			tmpCount++;
			//printf("After Bucket %d : array[%d]= %d \n",i,j,b.arrayBucket[j]);
		}
	}

	
	time1 = timer() - time1;
	//printf("______________________");
	//for(i=0;i<SIZE;i++){
	//	printf("Array %d\n",array[i]);
	//}
	//printf("______________________");
	for(i=0;i<SIZE;i++){
		printf("ResultArray %d\n",resultArray[i]);
	}
	
	
	printf("TIME OF EXECUTION: ");
	print_time(time1);
	
	return 0;
}
