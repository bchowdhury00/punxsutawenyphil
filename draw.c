
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"




void night_sky(){
    int fd = open("night",O_CREAT | O_RDWR, 0644);
    int n,z;
    char line[256];
    char * draw = "draw\n";
    char * end = "save\nsky.png\nquit\n";
    sprintf(line,"color\n%d %d %d\n",0,50,111);
    //creates blue sky
    write(fd,line,strlen(line));
    for(n = 0; n < 500; n++){
        sprintf(line,"line\n%d %d %d %d %d %d\n",0,n,0,XRES,n,0);
        write(fd,line,strlen(line));
    }
    write(fd,draw,strlen(draw));
    sprintf(line,"color\n%d %d %d\n",216,216,216);
    write(fd,line,strlen(line));
    //draws moon
    for (n = 0; n < 50; n++){
        sprintf(line,"circle\n%d %d %d %d\n",400,400,0,n);
        write(fd,line,strlen(line));
    }
    //makes the moon glow
    write(fd,draw,strlen(draw));
    for (n = 0; n < 15; n++){
        sprintf(line,"color\n%d %d %d\n",255 - n *14,255 - n * 13,255 - n);
        write(fd,line,strlen(line));
        sprintf(line,"circle\n%d %d %d %d\n",400,400,0,50 + n);
        write(fd,line,strlen(line));
        write(fd,draw,strlen(draw));
    }
    sprintf(line,"color\n%d %d %d\n",0,0,0);
    write(fd,line,strlen(line));
    z = rand() % 20;
    for (n = 0; n < z; n ++){
        int m, p;
        m = rand() % 15;
        for (p = 0; p < m; p++){
            sprintf(line,"circle\n%d %d %d %d\n",400,400,0,p);
            write(fd,line,strlen(line));
        }
        sprintf(line,"move\n%d %d %d\napply\n",rand() % 50,rand() % 50,0);
        write(fd,line,strlen(line));
        write(fd,draw,strlen(draw));
    }
    write(fd,end,strlen(end));
    close(fd);
}

/*======== void add_circle() ==========
  Inputs:   struct matrix * edges
            double cx
            double cy
            double r
            double step

  Adds the circle at (cx, cy) with radius r to edges
  ====================*/
void add_circle( struct matrix *edges,
                 double cx, double cy, double cz,
                 double r, double step ) {
                   double x,y,x1,y1,t;
                   t = 0.0;
                   while (t < 1){
                    x = r * cos(M_PI * 2 * t) + cx;
                    y = r * sin(M_PI * 2 * t) + cy;
                    t += step;
                    x1 = r * cos(M_PI * 2 * t) + cx;
                    y1 = r * sin(M_PI * 2 * t) + cy;
                    add_edge(edges,x,y,cz,x1,y1,cz);
                   }
                   return;
}

/*======== void add_curve() ==========
Inputs:   struct matrix *edges
         double x0
         double y0
         double x1
         double y1
         double x2
         double y2
         double x3
         double y3
         double step
         int type

Adds the curve bounded by the 4 points passsed as parameters
of type specified in type (see matrix.h for curve type constants)
to the matrix edges
====================*/
void add_curve( struct matrix *edges,
    double x0, double y0,
    double x1, double y1,
    double x2, double y2,
    double x3, double y3,
    double step, int type ) {
        struct matrix * coefsX = generate_curve_coefs(x0,x1,x2,x3,type);
        struct matrix * coefsY = generate_curve_coefs(y0,y1,y2,y3,type);
        double t,x,y,xNext,yNext;
        t = 0.0;
        while (t < 1){
            x = coefsX->m[3][0] + t * (coefsX->m[2][0] + t * (coefsX->m[1][0] + t * coefsX->m[0][0]));
            y = coefsY->m[3][0] + t * (coefsY->m[2][0] + t * (coefsY->m[1][0] + t * coefsY->m[0][0]));
            t += step;
            xNext = coefsX->m[3][0] + t * (coefsX->m[2][0] + t * (coefsX->m[1][0] + t * coefsX->m[0][0]));
            yNext = coefsY->m[3][0] + t * (coefsY->m[2][0] + t * (coefsY->m[1][0] + t * coefsY->m[0][0]));
            add_edge(edges,x,y,0,xNext,yNext,0);
        }
        return;
}


/*======== void add_point() ==========
Inputs:   struct matrix * points
         int x
         int y
         int z
Returns:
adds point (x, y, z) to points and increment points.lastcol
if points is full, should call grow on points
====================*/
void add_point( struct matrix * points, double x, double y, double z) {

  if ( points->lastcol == points->cols )
    grow_matrix( points, points->lastcol + 100 );

  points->m[0][ points->lastcol ] = x;
  points->m[1][ points->lastcol ] = y;
  points->m[2][ points->lastcol ] = z;
  points->m[3][ points->lastcol ] = 1;
  points->lastcol++;
} //end add_point

/*======== void add_edge() ==========
Inputs:   struct matrix * points
          int x0, int y0, int z0, int x1, int y1, int z1
Returns:
add the line connecting (x0, y0, z0) to (x1, y1, z1) to points
should use add_point
====================*/
void add_edge( struct matrix * points,
	       double x0, double y0, double z0,
	       double x1, double y1, double z1) {
  add_point( points, x0, y0, z0 );
  add_point( points, x1, y1, z1 );
}

/*======== void draw_lines() ==========
Inputs:   struct matrix * points
         screen s
         color c
Returns:
Go through points 2 at a time and call draw_line to add that line
to the screen
====================*/
void draw_lines( struct matrix * points, screen s, color c) {

 if ( points->lastcol < 2 ) {
   printf("Need at least 2 points to draw a line!\n");
   return;
 }

 int point;
 for (point=0; point < points->lastcol-1; point+=2)
   draw_line( points->m[0][point],
	      points->m[1][point],
	      points->m[0][point+1],
	      points->m[1][point+1],
	      s, c);
}// end draw_lines









void draw_line(int x0, int y0, int x1, int y1, screen s, color c) {

  int x, y, d, A, B;
  //swap points if going right -> left
  int xt, yt;
  if (x0 > x1) {
    xt = x0;
    yt = y0;
    x0 = x1;
    y0 = y1;
    x1 = xt;
    y1 = yt;
  }

  x = x0;
  y = y0;
  A = 2 * (y1 - y0);
  B = -2 * (x1 - x0);

  //octants 1 and 8
  if ( abs(x1 - x0) >= abs(y1 - y0) ) {

    //octant 1
    if ( A > 0 ) {

      d = A + B/2;
      while ( x < x1 ) {
	plot( s, c, x, y );
	if ( d > 0 ) {
	  y+= 1;
	  d+= B;
	}
	x++;
	d+= A;
      } //end octant 1 while
      plot( s, c, x1, y1 );
    } //end octant 1

    //octant 8
    else {
      d = A - B/2;

      while ( x < x1 ) {
	//printf("(%d, %d)\n", x, y);
	plot( s, c, x, y );
	if ( d < 0 ) {
	  y-= 1;
	  d-= B;
	}
	x++;
	d+= A;
      } //end octant 8 while
      plot( s, c, x1, y1 );
    } //end octant 8
  }//end octants 1 and 8

  //octants 2 and 7
  else {

    //octant 2
    if ( A > 0 ) {
      d = A/2 + B;

      while ( y < y1 ) {
	plot( s, c, x, y );
	if ( d < 0 ) {
	  x+= 1;
	  d+= A;
	}
	y++;
	d+= B;
      } //end octant 2 while
      plot( s, c, x1, y1 );
    } //end octant 2

    //octant 7
    else {
      d = A/2 - B;

      while ( y > y1 ) {
	plot( s, c, x, y );
	if ( d > 0 ) {
	  x+= 1;
	  d+= A;
	}
	y--;
	d-= B;
      } //end octant 7 while
      plot( s, c, x1, y1 );
    } //end octant 7
  }//end octants 2 and 7
} //end draw_line
