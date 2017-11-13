#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

#define N 4000
#define N_EXC 3200
#define N_INH ( ( N ) - ( N_EXC ) )

#define T 1000.
#define DT 1.
#define NT 1000  // ( T / DT )

#define TAU_MBP 20.
#define TAU_GE 5.
#define TAU_GI 10.

#define V_LEAK -49.
#define V_INIT -60.
#define V_THRESHOLD -50.
#define V_RESET -60.

#define G_EXC 1.62
#define G_INH -9.
#define P_EXC 0.02
#define P_INH 0.02

extern void init_genrand ( unsigned long );
extern double genrand_real2 ( void );
extern void timer_start ( void );
extern double timer_elapsed ( void );

static double v [ N ], ge [ N ], gi [ N ];
static int *w_exc, *w_inh, spike [ N ], *spike_local;

static FILE *file_spike;

void initialize ( const int mpi_size, const int mpi_rank )
{
  // PRNG
  init_genrand ( 23L );

  // Output file
  if ( mpi_rank == 0 ) {
    file_spike = fopen ( "spike.dat", "w" );
  }
  
  // Cell parameters
  for ( int i = 0; i < N; i++ ) {
    v [ i ] = V_INIT + 10. * genrand_real2 ();
    ge [ i ] = 0.;
    gi [ i ] = 0.;
    spike [ i ] = 0;
  }

  // Synaptic weights
  w_exc = (int *) malloc ( N * N * sizeof ( int ) );
  w_inh = (int *) malloc ( N * N * sizeof ( int ) );
  for ( int i = 0; i < N; i++ ) {
    // Excitatory neurons to other neurons
    for ( int j = 0; j < N_EXC; j++ ) {
      w_exc [ j + N * i ] = ( genrand_real2() < P_EXC ) ? 1 : 0;
    }
    // Inhibitory neurons to other neurons
    for ( int j = N_EXC; j < N_EXC + N_INH; j++ ) {
      w_inh [ j + N * i ] = ( genrand_real2() < P_INH ) ? 1 : 0;
    }
  }

  const int n_each = N / mpi_size;
  spike_local = (int *) malloc ( n_each * sizeof ( int ) );
  
}

void finalize ( const int mpi_rank )
{
  if ( mpi_rank == 0 ) {
    fclose ( file_spike );
  }
  
  free ( spike_local );
  free ( w_exc );
  free ( w_inh );
}

void calculateSynapse ( const int n, const int offset )
{
  int i = n + offset;
  {
    double r = 0.;
    for ( int j = 0; j < N_EXC; j++ ){
      r += w_exc [ j + N * i ] * spike [ j ];
    }
    ge [ i ] += DT * ( G_EXC * r - ge [ i ] ) / TAU_GE;
  }
  {
    double r = 0.;
    for ( int j = N_EXC; j < N_EXC + N_INH; j++ ){
      r += w_inh [ j + N * i ] * spike [ j ];
    }
    gi [ i ] += DT * ( G_INH * r - gi [ i ] ) / TAU_GI;
  }
}

void updateMembranePotential ( const int n, const int offset )
{
  int i = n + offset; 
  v [ i ] += DT * ( ge [ i ] + gi [ i ] - ( v [ i ] - V_LEAK ) ) / TAU_MBP;
  if ( v [ i ] > V_THRESHOLD ) {
    spike_local [ n ] = 1;
    //spike [ i ] = 1;
    v [ i ] = V_RESET;
  } else {
    spike_local [ n ] = 0;
    //spike [ i ] = 0;
  }
}

void outputSpike ( const int nt )
{
  for ( int i = 0; i < N; i++ ) {
    if ( spike [ i ] ) { fprintf ( file_spike, "%f %d\n", DT * nt, i ); }
  }
}

void loop ( const int mpi_size, const int mpi_rank )
{
  const int n_each = N / mpi_size;

  timer_start ();
  for ( int nt = 0; nt < NT; nt++ ) {
#pragma omp parallel for num_threads ( 4 )
    for ( int n = 0; n < n_each; n++ ) {
      calculateSynapse ( n, n_each * mpi_rank );
      updateMembranePotential ( n, n_each * mpi_rank );
    }
    MPI_Allgather ( spike_local, n_each, MPI_INT, spike, n_each, MPI_INT, MPI_COMM_WORLD );
    //if ( mpi_rank == 0 ) { outputSpike ( nt ); }
  }
  double elapsedTime = timer_elapsed ();
  if ( mpi_rank == 0 ) { printf ( "Elapsed time = %f sec.\n", elapsedTime); }
}

int main ( int argc, char *argv[] )
{
  MPI_Init ( &argc, &argv );

  int mpi_size, mpi_rank;
  MPI_Comm_size ( MPI_COMM_WORLD, &mpi_size );
  MPI_Comm_rank ( MPI_COMM_WORLD, &mpi_rank );
  
  initialize ( mpi_size, mpi_rank );
  loop ( mpi_size, mpi_rank );
  finalize ( mpi_rank );

  MPI_Finalize ();
  
  return 0;
}
