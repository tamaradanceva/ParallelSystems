#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "time.c"
# include "normal.c"

/*RESULT EXECUTION TIME: NORMAL DISTRIBUTION a=70, b=10
 * SIZE=10; 0.000022;
 * SIZE=100; 0.000053;
 * SIZE=1000; 0.000376;
 * SIZE=10000; 0.011826;
 * SIZE=20000; 0.043048;
 * SIZE=30000; 0.115734;
 * SIZE=40000; 0.115432;
 * SIZE=50000; 0.174121;
 * SIZE=60000; 0.249102;
 * SIZE=70000; 0.298845;
 * SIZE=80000; 0.427522;
 * SIZE=90000; 0.483830;
 * SIZE=100000; 0.598888;
 * SIZE=110000; 0.705203;
 * SIZE=120000; 0.895879;
 * SIZE=130000; 1.036586;
 * SIZE=140000; 1.137492;
 * SIZE=150000; 1.322866;
 * SIZE=160000; 1.473868;
 * SIZE=170000; 1.713851;
 * SIZE=180000; 1.926754;
 * SIZE=190000; 2.063295;
 * SIZE=200000; 2.320106;
 * SIZE=250000; 3.591560;
 * SIZE=300000; 5.155354;
 * SIZE=350000; 6.924571;
 * SIZE=400000; 9.002567;
 * SIZE=450000; 11.482232;
 * SIZE=500000; 14.069999;
 * SIZE=550000; 16.996690;
 * SIZE=600000; 20.179871;
 * 
 * RESULT EXECUTION TIME WITH UNIFORM DISTRIBUTION
 * SIZE=10; 0.000003;
 * SIZE=100; 0.000013;
 * SIZE=1000; 0.000149;
 * SIZE=10000; 0.003187;
 * SIZE=20000; 0.011451;
 * SIZE=30000; 0.043213;
 * SIZE=40000; 0.060761;
 * SIZE=50000; 0.073121;
 * SIZE=60000; 0.078017;
 * SIZE=70000; 0.110439;
 * SIZE=80000; 0.131061;
 * SIZE=90000; 0.167746;
 * SIZE=100000; 0.255511;
 * SIZE=110000; 0.240379;
 * SIZE=120000; 0.292963;
 * SIZE=130000; 0.332249;
 * SIZE=140000; 0.387530;
 * SIZE=150000; 0.453347;
 * SIZE=160000; 0.497361;
 * SIZE=170000; 0.546070;
 * SIZE=180000; 0.584870;
 * SIZE=190000; 0.692192;
 * SIZE=200000; 0.747082;
 * SIZE=250000; 1.136697;
 * SIZE=300000; 1.603325;
 * SIZE=350000; 2.176600;
 * SIZE=400000; 2.798710;
 * SIZE=450000; 3.554562;
 * SIZE=500000; 4.387099;
 * SIZE=550000; 5.279040;
 * SIZE=600000; 6.299424;
 * 
 * */
//Number of elements to be sorted
#define SIZE 1000000

//The range of the elements to be sorted
#define RANGE 115

//Array holding the elements
int * array;

int uniform_distribution(int rangeLow, int rangeHigh) {
    double myRand = rand()/(1.0 + RAND_MAX); 
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}

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
int main(){
	
array= malloc(SIZE*sizeof(int));
	int i;
	srand(time(NULL));
	
	float a;
  float b;
  int seed;
  int seed_init = 123456789;

  a = 70.0;
  b = 10.0;
  seed = seed_init;
	int time1=timer();
	// Assign random values from 1 to/including RANGE to the elements in the array
	for(i=0;i<SIZE;i++){
		//array[i]=i4_normal_ab( a, b, &seed );
		array[i]=uniform_distribution(1,RANGE);
	}
	
	quicksort(array,0,SIZE-1);
	
		time1 = timer() - time1;
	
	
	for(i=0;i<SIZE;i++){
		printf("%d\n",array[i]);
	}
	printf("TIME OF EXECUTION: ");
	print_time(time1);

return 0;
}
