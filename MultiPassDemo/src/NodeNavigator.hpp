//
//  NodeNavigator.hpp
//  MovingThingsAround
//
//  Created by Hartmut Schirmacher on 16.05.16.
//
//

#ifndef NodeNavigator_hpp
#define NodeNavigator_hpp

#include "RTR/RTR.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"


class NodeNavigator {
public:
    
    NodeNavigator(rtr::NodeRef node = nullptr,
                  rtr::NodeRef scene = nullptr)
				  : node_(node), scene_(scene) {
		ci::app::console() << "node is: " << (node_ != nullptr)<<std::endl;
	}

    glm::mat4 toWorld() const;
    
protected:
    rtr::NodeRef node_, scene_;
    
};

/* change absolute position of a node using left/right/up/down/shift-up/shift-down, 
   constant speed, no acceleration */

class AbsolutePositionNavigator : public NodeNavigator {
    
public:
    
    // constructor
    AbsolutePositionNavigator(rtr::NodeRef node = nullptr,
                              rtr::NodeRef scene = nullptr)
							  : NodeNavigator(node, scene) {
		ci::app::console() << "node is: " << (node_ != nullptr) << std::endl;
	}
    
    // process (shift) cursor keys
    void keyDown(cinder::app::KeyEvent event);

	void keyUp(cinder::app::KeyEvent event);

	glm::vec2 getSpeed(){
		return glm::vec2(speedU, speedV);
	}
    
    // speed: how much is the position changed with one key press
    float speedU = 0.0;
	float speedV = 0.0;
	float maxSpeed = 0.1;
	float acceleration = 0.001;
};

/* left mouse button to rotate; shift-drag to pan, wheel to zoom */
class TrackballNavigator : public NodeNavigator {
    
public:
    
    // constructor
    TrackballNavigator(rtr::NodeRef node = nullptr,
                       rtr::NodeRef scene = nullptr)
    : NodeNavigator(node,scene) {}
    
    // process mouse events and query modifier keys
	void mouseMove(cinder::app::MouseEvent event);
    void mouseDown(cinder::app::MouseEvent event);
    void mouseDrag(cinder::app::MouseEvent event);
    void mouseWheel(cinder::app::MouseEvent event);
    
    // sensitivities
    float rotationSensitivity = 0.1; // degrees per pixel mouse movement
    float panSensitivity = 0.01;      // translation per pixel mouse movement
    float zoomSensitivity = 0.2;     // translation per pixel mouse movement
    
private:
    
    // attributes to implement dragging
    glm::vec2 lastPosition_;
    
    // routines for actual navigation.
    void rotate(glm::vec2 degrees);
    void pan(glm::vec3 translation); // zoom implemented by pan in Z direction
    
};

#endif /* NodeNavigator_hpp */
