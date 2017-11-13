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
#define TAU_SYN 5.0
#define W 10.0 //-10.0

void loop ( void )
{
  double v [ 2 ] = { V_INIT, V_INIT - 10. };
  double i_ext [ 2 ] = { I_EXT, I_EXT };
  double g [ 2 ] = { 0., 0. };
  int spike [ 2 ] = { 0, 0 };
  for ( int nt = 0; nt < NT; nt++ ) {
    printf ( "%f %f %f\n", DT * nt, v [ 0 ], v [ 1 ]);
    double dv [ 2 ];
    dv [ 0 ] = ( DT / TAU ) * ( - ( v [ 0 ] - V_LEAK ) + g [ 0 ] + i_ext [ 0 ] );
    dv [ 1 ] = ( DT / TAU ) * ( - ( v [ 1 ] - V_LEAK ) + g [ 1 ] + i_ext [ 1 ] );
    double dg [ 2 ];
    dg [ 0 ] = ( DT / TAU_SYN ) * ( - g [ 0 ] + W * spike [ 1 ] );
    dg [ 1 ] = ( DT / TAU_SYN ) * ( - g [ 1 ] + W * spike [ 0 ] );
    v [ 0 ] += dv [ 0 ];
    v [ 1 ] += dv [ 1 ];
    g [ 0 ] += dg [ 0 ];
    g [ 1 ] += dg [ 1 ];
    spike [ 0 ] = ( v [ 0 ] > THETA );
    if ( v [ 0 ] > THETA ) {
      printf ( "%f 0 %f\n", DT * nt, v [ 1 ] ); // print spike with membrane potential = 0 mV
      v [ 0 ] = V_RESET;
    }
    spike [ 1 ] = ( v [ 1 ] > THETA );
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
