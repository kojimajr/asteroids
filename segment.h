// segment.h


class Segment {
 public:

  vec3 tail, head;

  Segment( vec3 t, vec3 h )
    { tail = t; head = h; }

  bool intersects( Segment const& s2 ) {
	  
	  //YOUR CODE HERE
	  //Since we are working exclusively in z=1, we can treat this as 2D
	  
	  //First we alias all coordinates for both segments
	  float x1 = tail.x;
	  float x2 = head.x;
	  float x3 = s2.tail.x;
	  float x4 = s2.head.x;
	  float y1 = tail.y;
	  float y2 = head.y;
	  float y3 = s2.tail.y;
	  float y4 = s2.head.y;

	  //Since both lines can be plotted as u + t(v-u) we can solve a system of two equations
	  // u1 + t1(v1-u1)
	  // u2 + t2(v2-u2)
	  // which results in
	  /*
	  *
	  -----------------	----		---------
	  |x4 - x3	x1 - x2||t1|		|x1 - x3|
	  |y4 - y3	y1 - y2||t2|	=	|y1 - y3|
	  -----------------	----		---------
	  *
	  */
	  //And with some matrix inversion (on paper, no 2x2 matrix support included), we eventually get...
	  
	  float t1 = ((y3-y4)*(x1-x3) + (x4-x3)*(y1-y3)) / ((x4-x3)*(y1-y2) - (x1-x2)*(y4-y3));
	  float t2 = ((y1-y2)*(x1-x3) + (x2-x1)*(y1-y3)) / ((x4-x3)*(y1-y2) - (x1-x2)*(y4-y3));
		
	  //If both terms are between 0 and 1 
	  //meaning they are on the segment,
	  //not on the imaginary line continued forwards and back
	  //that means they intersect!

	  if (t1 <= 1.00 && t1 >= 0.00 && t2 <= 1.00 && t2 >= 0.00) {
		  return true;
	  }
	 
	  return false;
  }
};


