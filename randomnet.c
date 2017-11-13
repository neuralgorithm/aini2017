#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
static int *w_exc, *w_inh, spike [ N ];

static FILE *file_spike;

void initialize ( void )
{
  // PRNG
  init_genrand ( 23L );

  // Output file
  file_spike = fopen ( "spike.dat", "w" );

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
    // From excitatory neurons to other neurons
    for ( int j = 0; j < N_EXC; j++ ) {
      w_exc [ j + N * i ] = ( genrand_real2() < P_EXC ) ? 1 : 0;
    }
    // From inhibitory neurons to other neurons
    for ( int j = N_EXC; j < N_EXC + N_INH; j++ ) {
      w_inh [ j + N * i ] = ( genrand_real2() < P_INH ) ? 1 : 0;
    }
  }

}

void finalize ( void )
{
  fclose ( file_spike );
  free ( w_exc );
  free ( w_inh );
}

void calculateSynapse ( const int i )
{
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

void updateMembranePotential ( const int i )
{
  v [ i ] += DT * ( ge [ i ] + gi [ i ] - ( v [ i ] - V_LEAK ) ) / TAU_MBP;
  if ( v [ i ] > V_THRESHOLD ) {
    spike [ i ] = 1;
    v [ i ] = V_RESET;
  } else {
    spike [ i ] = 0;
  }
}

void outputSpike ( const int nt )
{
  for ( int i = 0; i < N; i++ ) {
    if ( spike [ i ] ) { fprintf ( file_spike, "%f %d\n", DT * nt, i ); }
  }
}

void loop ( void )
{
  timer_start ();
  for ( int nt = 0; nt < NT; nt++ ) {
    for ( int i = 0; i < N; i++ ) {
      calculateSynapse ( i );
      updateMembranePotential ( i );
    }
    outputSpike ( nt );
  }
  double elapsedTime = timer_elapsed ();
  printf ( "Elapsed time = %f sec.\n", elapsedTime);
}

int main ( void )
{
  initialize ();
  loop ();
  finalize ();

  return 0;
}
