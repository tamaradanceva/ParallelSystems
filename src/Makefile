MPICC= mpicc 
CC= gcc


PRGS=BucketSampleSortMPI

all: $(PRGS)

time.o: time.c 
	$(CC) -O3 -c time.c -o time.o 

	
%: %.c time.o 
	$(MPICC) -o $@ $@.c time.o -lm

	
clean: 
	rm -f *.o 
	rm -f $(PRGS) 
	
distclean: clean
	rm -f *~ 
