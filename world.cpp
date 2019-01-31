// world.cpp


#include "world.h"
#include "ship.h"
#include "main.h"
#include "gpuProgram.h"
#include "strokefont.h"

#include <sstream>


void World::start()

{
  // Create a ship at the centre of the world

  ship = new Ship( 0.5 * (worldMin+worldMax) );

  // Create some large asteroids

  // Pick a random position at least 20% away from the origin (which
  // is where the ship will be).

  score = 0;	//Clear score on game reset
  asteroids.clear();
  shells.clear();

  for (int i=0; i<NUM_INITIAL_ASTEROIDS; i++) {

    vec3 pos;
    do {
      pos = vec3( randIn01(), randIn01(), 0 );
    } while ((pos - vec3( 0.5, 0.5, 0 )).length() < 0.20);

    asteroids.push_back( new Asteroid( pos % (worldMax - worldMin) + worldMin ) );
  }

  // Increase asteroid speed in later rounds

  for (unsigned int  i=0; i<asteroids.size(); i++) {
    asteroids[i]->velocity = ((1+round) * ASTEROID_SPEED_ROUND_FACTOR) * asteroids[i]->velocity;
    asteroids[i]->angularVelocity = ((1+round) * ASTEROID_SPEED_ROUND_FACTOR) * asteroids[i]->angularVelocity;
  }

  state = RUNNING;
}


void World::updateState( float elapsedTime )

{
  if (state == PAUSED)
    return;

  if (asteroids.size() == 0) {
    round++;
    start();
    return;
  }

  // See if any keys are pressed for thrust

  if (state == RUNNING) {

    if (rightKey == DOWN)
      ship->rotateCW( elapsedTime );

    if (leftKey == DOWN)
      ship->rotateCCW( elapsedTime );

    if (upKey == DOWN)
      ship->addThrust( elapsedTime );
  }

  // Update the ship

  ship->updatePose( elapsedTime );

  // Update the asteroids (check for asteroid collisions)

  for (unsigned int i=0; i<asteroids.size(); i++) {
    asteroids[i]->updatePose( elapsedTime );
    if (state == RUNNING && ship->intersects( *asteroids[i] ))
      gameOver();
  }

  // Update the shells (check for asteroid collisions)

  for (unsigned int i=0; i<shells.size(); i++) {
    shells[i]->elapsedTime += elapsedTime;

    if (shells[i]->elapsedTime > SHELL_MAX_TIME) { // remove an old shell

      shells.erase( shells.begin() + i );
      i--;
	   
    } else { // move a not-too-old shell

      vec3 prevPosition = shells[i]->centrePosition();
      shells[i]->updatePose( elapsedTime );

      Segment path( prevPosition, shells[i]->centrePosition() );
	  
	  //YOUR CODE HERE
	  
	  bool hit = false;	//Did the shell hit?
	  int toDamage;		//Which asteroid to damage (index)
	  for (int j = 0; j < asteroids.size(); j++){
		  Asteroid asteroid = *asteroids[j];
		  //Only hit one asteroid
		  if (!hit && asteroid.intersects(path)){
			  hit = true;
			  toDamage = j;
		  }
	  }
	  if(hit){
		  //Get our asteroid to damage and remove shell on hit
		  Asteroid asteroid = *asteroids[toDamage];
		  vec3 shellVelocity = shells[i]->velocity;
		  shells.erase(shells.begin() + i);
		  //Add score and erase if small enough
		  if (asteroid.scaleFactor * ASTEROID_SCALE_FACTOR_REDUCTION < MIN_ASTEROID_SCALE_FACTOR) {
			  score += asteroid.scoreValue;
			  asteroids.erase(asteroids.begin() + toDamage);
		  }
		  else {
			  score += asteroid.scoreValue;
			  
			  vec3 pos = asteroid.centrePosition();			//Location to spawn new asteroids
			  vec3 normalized = shellVelocity.normalize();	//Normalize in order to properly determine directions
			  Asteroid* subA = new Asteroid(pos);
			  Asteroid* subB = new Asteroid(pos);

			  //Send each new asteroid in opposite directions, normal to normalized vector, starting with base speed
			  subA->velocity = (ASTEROID_SPEED* vec3(-1 * normalized[1], normalized[0], 0));
			  subB->velocity = (ASTEROID_SPEED* vec3(normalized[1], -1* normalized[0], 0));

			  //Half scale and score
			  subA->scaleFactor = asteroid.scaleFactor * 0.5;
			  subB->scaleFactor = asteroid.scaleFactor * 0.5;
			  
			  subB->scoreValue= asteroid.scoreValue * 0.5;
			  subB->scoreValue = asteroid.scoreValue * 0.5;

			  //Destroy our old asteroid and queue up the new ones
			  asteroids.erase(asteroids.begin() + toDamage);
			  asteroids.push_back(subA);
			  asteroids.push_back(subB);

		  }
	  }
    }
  }
}


void World::draw()

{
  // Transform [worldMin,worldMax] to [(-1,-1),(+1,+1)].

  mat4 worldToViewTransform;

  worldToViewTransform
    = translate( -1, -1, 0 )
    * scale( 2.0/(worldMax.x-worldMin.x), 2.0/(worldMax.y-worldMin.y), 1 )
    * translate( -worldMin.x, -worldMin.y, 0 );

  // Draw all world elements, passing in the worldToViewTransform so
  // that they can append their own transforms before passing the
  // complete transform to the vertex shader.

  ship->draw( worldToViewTransform );

  for (unsigned int i=0; i<shells.size(); i++)
    shells[i]->draw( worldToViewTransform );

  for (unsigned int i=0; i<asteroids.size(); i++)
    asteroids[i]->draw( worldToViewTransform );

  // Draw the title

  drawStrokeString( "ASTEROIDS", 0, 0.85, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP"), CENTRE );

  // Draw messages according to game state

  if (state == BEFORE_GAME) 

    drawStrokeString( "PRESS 's' TO START, 'p' TO PAUSE DURING GAME", 0, -.9, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP"), CENTRE );

  else {

    // draw score

    stringstream ss;
    ss.setf( ios::fixed, ios::floatfield );
    ss.precision(1);
    ss << "SCORE " << score;
    drawStrokeString( ss.str(), -0.95, 0.75, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP"), LEFT );

    if (state == AFTER_GAME) {

      // Draw "game over" message

      drawStrokeString( "GAME OVER", 0, 0, 0.12, glGetUniformLocation( myGPUProgram->id(), "MVP"), CENTRE );
      drawStrokeString( "PRESS 's' TO START, 'p' TO PAUSE DURING GAME", 0, -0.9, 0.06, glGetUniformLocation( myGPUProgram->id(), "MVP"), CENTRE );
    }
  }
}


void World::gameOver()

{
  state = AFTER_GAME;
}


void World::togglePause()

{
  if (state == RUNNING)
    state = PAUSED;
  else if (state == PAUSED)
    state = RUNNING;
}
