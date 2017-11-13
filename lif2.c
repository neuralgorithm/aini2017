#include<stdio.h>

#define TAU 20.0
#define V_LEAK -65.0
#define V_INIT (V_LEAK)
#define V_RESET (V_LEAK)
#define THETA -55.0
#define DT 1.0
#define T 1000.0
#define NT 1000 // ( T / DT )
#define I_EXT 12.0

void loop ( void )
{
  double v [ 2 ] = { V_INIT, V_INIT - 10.0 };
  double i_ext = I_EXT;
  for ( int nt = 0; nt < NT; nt++ ) {
    printf ( "%f %f %f\n", DT * nt, v [ 0 ], v [ 1 ]);
    double dv [ 2 ];
    dv [ 0 ] = ( DT / TAU ) * ( - ( v [ 0 ] - V_LEAK ) + i_ext );
    dv [ 1 ] = ( DT / TAU ) * ( - ( v [ 1 ] - V_LEAK ) + i_ext );
    v [ 0 ] += dv [ 0 ];
    v [ 1 ] += dv [ 1 ];
    if ( v [ 0 ] > THETA ) {
      printf ( "%f 0 %f\n", DT * nt, v [ 1 ] ); // print spike with membrane potential = 0 mV
      v [ 0 ] = V_RESET;
    }
    if ( v [ 1 ] > THETA ) {
      printf ( "%f %f 0\n", DT * nt, v [ 0 ] ); // print spike with membrane potential = 0 mV
      v [ 1 ] = V_RESET;
    }
  }
}

int main ( void )
{
  loop ( );

  return 0;
}
