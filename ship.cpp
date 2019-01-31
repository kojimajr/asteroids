// ship.cpp


#include "ship.h"


void Ship::rotateCW( float deltaT )

{
  orientation = quaternion( - SHIP_ROTATION_SPEED * deltaT, vec3(0,0,1) ) * orientation;
}


void Ship::rotateCCW( float deltaT )

{
  orientation = quaternion( + SHIP_ROTATION_SPEED * deltaT, vec3(0,0,1) ) * orientation;
}


void Ship::addThrust( float deltaT )

{
  // YOUR CODE HERE

	//Acceleration and time gives us velocity to add in =Y
	//Rotate using homogeneous coordinates then add to exising velocity

	float changeX = 0;
	float changeY = deltaT * SHIP_THRUST_ACCEL ;

	vec4 rotated =  orientation.toMatrix() * vec4(changeX, changeY, 0, 1);

	this->velocity = vec3(velocity[0] + rotated[0], velocity[1] + rotated[1], 0);
}


Shell * Ship::fireShell()

{
	// YOUR CODE HERE

	//Use base shell speed to send shell out front of ship (at same angle as ship)
	//Rotate shell velocity (initially in +Y direction purely) using homogeneous coordinates

	float shellX = 0;
	float shellY = SHELL_SPEED;
	
	vec4 rotated = orientation.toMatrix() * vec4(shellX, shellY, 0, 1);
	vec3 shellVelocity = vec3(rotated[0], rotated[1], 0);
	
	return new Shell(this->position, shellVelocity, this->orientation);
}


// Ship model consisting of line segments
//
// These are in a ARBITRARY coordinate system and get remapped to the
// world coordinate system (with the centre of the ship at (0,0) and
// width SHIP_WIDTH) when the VAO is set up.


float Ship::verts[] = {

   3,0,  0,9,
   0,9, -3,0, 
  -3,0,  0,1,
   0,1,  3,0,

  9999
};

