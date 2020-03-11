#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"

int main(int argc, char **argv) {
  screen s;
  struct matrix * edges;
  struct matrix * transform;
  srand(time(NULL));
  edges = new_matrix(4, 4);
  transform = new_matrix(4, 4);
  /*if ( argc == 2 )
    parse_file( argv[1], transform, edges, s );
  else
    parse_file( "stdin", transform, edges, s );*/
  night_sky();
  struct matrix * edges1;
  struct matrix * transform1;
  edges1 = new_matrix(4, 4);
  transform1 = new_matrix(4, 4);
  parse_file("script",transform,edges,s);
  parse_file("night",transform1,edges1,s);
  free_matrix( edges );
  free_matrix( transform );
  free_matrix( edges1 );
  free_matrix( transform1 );
}
