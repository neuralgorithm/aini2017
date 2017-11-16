#include<stdio.h>

#define TAU 20.0
#define V_LEAK -65.0
#define V_INIT (V_LEAK)
#define V_RESET (V_LEAK)
#define THETA -55.0
#define R_M 1.0
#define DT 1.0
#define T 1000.0
#define NT 1000 // ( T / DT )
#define I_EXT 12.0

void loop ( void )
{
  double v = V_INIT;
  for ( int nt = 0; nt < NT; nt++ ) {
    printf ( "%f %f\n", DT * nt, v );
    double i_ext = ( DT * nt < 100.0 || 900 < DT * nt) ? 0 : I_EXT;
    double dv = ( DT / TAU ) * ( - ( v - V_LEAK ) + R_M * i_ext );
    v  += dv;
    if ( v > THETA ) {
      printf ( "%f 0\n", DT * nt ); // print spike with membrane potential = 0 mV
      v = V_RESET;
    }
  }
}

int main ( void )
{
  loop ( );

  return 0;
}
