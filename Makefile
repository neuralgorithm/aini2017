CC = gcc
MPICC = mpicc
CFLAGS = -std=c99 -Wall -O3
OMPFLAGS = -fopenmp
MISC_OBJS = mt19937ar-cok.o timer.o

all: lif lif2 lif2net randomnet omp ompsyn mpi hybrid

lif: lif.o
	$(CC) $(CFLAGS) -o $@ $^ -lm
lif.o: lif.c
	$(CC) $(CFLAGS) -o $@ -c $<
lif2: lif2.o
	$(CC) $(CFLAGS) -o $@ $^ -lm
lif2.o: lif2.c
	$(CC) $(CFLAGS) -o $@ -c $<
lif2net: lif2net.o
	$(CC) $(CFLAGS) -o $@ $^ -lm
lif2net.o: lif2net.c
	$(CC) $(CFLAGS) -o $@ -c $<
randomnet: randomnet.o $(MISC_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm
randomnet.o: randomnet.c
	$(CC) $(CFLAGS) -o $@ -c $<
omp: omp.o $(MISC_OBJS)
	$(CC) $(OMPFLAGS) $(CFLAGS) -o $@ $^ -lm
omp.o: omp.c
	$(CC) $(OMPFLAGS) $(CFLAGS) -o $@ -c $<
ompsyn: ompsyn.o $(MISC_OBJS)
	$(CC) $(OMPFLAGS) $(CFLAGS) -o $@ $^ -lm
ompsyn.o: ompsyn.c
	$(CC) $(OMPFLAGS) $(CFLAGS) -o $@ -c $<
mpi: mpi.o $(MISC_OBJS)
	$(MPICC) $(CFLAGS) -o $@ $^ -lm
mpi.o: mpi.c
	$(MPICC) $(CFLAGS) -o $@ -c $<
hybrid: hybrid.o $(MISC_OBJS)
	$(MPICC) $(OMPFLAGS) $(CFLAGS) -o $@ $^ -lm
hybrid.o: hybrid.c
	$(MPICC) $(OMPFLAGS) $(CFLAGS) -o $@ -c $<

mt19937ar-cok.o: mt19937ar-cok.c
	$(CC) $(CFLAGS) -o $@ -c $<
timer.o: timer.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f lif lif2 lif2net randomnet omp ompsyn mpi hybrid *~ *.o
distclean: clean
	rm -f *.dat

mpirun:
	mpirun -hostfile hostfile -np 16 ./mpi
