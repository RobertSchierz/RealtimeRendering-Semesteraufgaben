#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "RTR/RTR.h"

#include "NodeNavigator.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace rtr;
using namespace glm;
using namespace cinder;
using namespace cinder::app;


glm::mat4
NodeNavigator::toWorld() const {

	// find node in entire scene
	assert(node_);
	vector<Transformed> transNodes =
		scene_ ? scene_->find(node_) : node_->find(node_);

	// check how many instances have been returned
	if (transNodes.size() != 1)
		throw Exception("Node to be navigated must be in the scene exactly once, found " + to_string(transNodes.size()) + " times.");

	//ci::app::console() << "toWorld transform is: " << transNodes[0].transform << std::endl;

	// return just the transform of the first result
	return transNodes[0].transform;
	//return scene_->find(node_)[0].transform;
}

void
AbsolutePositionNavigator::keyDown(KeyEvent event)
{
	// translation vector in eye coords
	vec4 trans(0, 0, 0, 0);

	// depending on key press, change position
	switch (event.getCode()) {
	case KeyEvent::KEY_DOWN:
		if (event.isShiftDown()){
			isMovingBackward = true;
			
			//trans[2] = speed;  // shift: Z
			//ci::app::console() << "speed is: " << speed << std::endl;
			//if (speed < maxSpeed) speed += acceleration;
			//else if (speed > maxSpeed) speed = maxSpeed;
		}
		else
			trans[1] = -0.01; // Y
		break;
	case KeyEvent::KEY_UP:
		if (event.isShiftDown())
			//trans[2] = -speed; // shift: Z
			isMovingForward = true;
		else
			trans[1] = 0.01; // Y
		break;
	case KeyEvent::KEY_LEFT:
		//trans[0] = -speed; // X
		isMovingLeft = true;
		break;
	case KeyEvent::KEY_RIGHT:
		//trans[0] = speed; // X
		isMovingRight = true;
		break;
	default:
		return;
	}

	// translate in model coords
	mat4 toModel = inverse(toWorld());
	node_->transform *= glm::translate(vec3(toModel*trans));

}

void
AbsolutePositionNavigator::keyUp(KeyEvent event)
{
	// translation vector in eye coords
	vec4 trans(0, 0, 0, 0);

	// depending on key press, change position
	switch (event.getCode()) {
	case KeyEvent::KEY_UP:
		if (event.isShiftDown()){
			isMovingForward = false;
		}
		break;
	case KeyEvent::KEY_DOWN:
		if (event.isShiftDown()){
			isMovingBackward = false;
		}
		break;
	case KeyEvent::KEY_LEFT:
		isMovingLeft = false;
		break;
	case KeyEvent::KEY_RIGHT:
		isMovingRight = false;
		break;
	default:
		return;
	}

	// translate in model coords
	//mat4 toModel = inverse(toWorld());
	//node_->transform *= glm::translate(vec3(toModel*trans));
}

void
TrackballNavigator::mouseDrag(MouseEvent event){

	// how much did the mouse move?
	vec2 now = vec2(event.getPos());
	vec2 delta = now - lastPosition_;
	lastPosition_ = now;

	// not enough movement -> do nothing
	if (length(delta) < 2)
		return;

	// apply to panning or rotation
	if (event.isShiftDown())
		pan(vec3(delta * vec2(1, -1) * this->panSensitivity, 0));
	else{
		rotate(delta * this->rotationSensitivity);
	}
}




void
TrackballNavigator::mouseDown(MouseEvent event){
	if (event.isLeft()){
		lastPosition_ = event.getPos();
	}
}

void
TrackballNavigator::mouseWheel(MouseEvent event){
	// zoom according to wheel movement
	float inc = event.getWheelIncrement();
	if (inc != 0.0)
		pan(vec3(0, 0, -inc * this->zoomSensitivity));
}


void
TrackballNavigator::rotate(vec2 degrees)
{
	// transformation from camera coords to model coords

	mat4 toModel = toWorld();
	mat4 inverseToModel = inverse(toModel);

	// axes transformed into model space
	vec3 xAxis = vec3(inverseToModel[0]);
	vec3 yAxis = vec3(inverseToModel[1]);

	// rotate around transformed X and Y axes
	float RadiansX = toRadians(degrees[1]);
	float RadiansY = toRadians(degrees[0]);

	node_->transform *= glm::rotate(RadiansX, xAxis);
	node_->transform *= glm::rotate(RadiansY, yAxis);

}

void
TrackballNavigator::pan(vec3 translation)
{
	// translation transformed into model space
	mat4 toModel = inverse(toWorld());
	vec4 translationMC = toModel*vec4(translation, 0);
	node_->transform *= glm::translate(vec3(translationMC));

}