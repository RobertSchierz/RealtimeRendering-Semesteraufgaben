//
//  Copyright 2016 Henrik Tramberend, Hartmut Schirmacher
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "RTR/RTR.h"
#include "NodeNavigator.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace rtr;

// App template for the Real Time Rendering course, inherits from Cinder App.
class MultiPassDemoApp : public App
{
  public:
    // Called once when the App starts, after OpenGL has been initialized.
    void setup() override;

    // Called for every frame to be rendered, before draw().
    void update() override;

    // Called once for every frame to be rendered.
    void draw() override;

	void keyDown(KeyEvent event) override;
	void keyUp(KeyEvent event) override;

	void mouseMove(MouseEvent event) override;

	

	/*void mouseDrag(MouseEvent event) override{
		cameraMouseNav_.mouseDrag(event);
	}

	void mouseDown(MouseEvent event) override{
		cameraMouseNav_.mouseDown(event);
	}

	void mouseWheel(MouseEvent event) override{
		cameraMouseNav_.mouseWheel(event);
	}*/

  private:
	  void doMovement();

	  /*NodeRef camera_, root_, scene_, model_;
	  AbsolutePositionNavigator cameraNav_;
	  TrackballNavigator cameraMouseNav_;*/

	  vec3 cameraPos = vec3(0, 0, 3);
	  vec3 cameraFront = vec3(0, 0, -1);
	  vec3 cameraUp = vec3(0, 1, 0);

	  bool keys[1024];

	  bool w = false;
	  bool s = false;
	  bool a = false;
	  bool d = false;

    // Rotation angle used for the animation.
    double angle = 0.0;

    // Tracking time between two draw() calls.
    double lastTime = getElapsedSeconds();

    // Model of the duck that is displayed.
    rtr::ModelRef duck;
};

// Place all one-time setup code here.
void
MultiPassDemoApp::setup()
{
    // Arange for the window to always be on top. This helps with live shader
    // coding
    //getWindow()->setAlwaysOnTop();

    // Create a live-reloading shader program.
    auto lambert = rtr::watcher.createWatchedProgram(
      { getAssetPath("lambert.vert"), getAssetPath("lambert.frag") });

    // Load the duck model and use the lambert shader on it.
    duck = rtr::loadObjFile(getAssetPath("duck/duck.obj"), true, lambert);

	//model_ = Node::create({ duck });

	//scene_ = Node::create({}, glm::rotate(toRadians(-90.0f), vec3(0, 1, 0)), { model_ });

	//camera_ = Node::create({}, translate(vec3(0, 0.2, 4)));
	//root_ = Node::create({}, mat4(), { scene_, camera_ });

	//cameraNav_ = AbsolutePositionNavigator(camera_, root_);

    // The shader program can also be replaced after the fact.
    // duck = rtr::loadObjFile(getAssetPath("duck/duck.obj"));
    // for (auto& shape : duck->shapes)
    //     shape->replaceProgram(lambert);

    // Try this version of the duck to see the default shader for OBJ
    // models.
    // duck = rtr::loadObjFile(getAssetPath("duck/duck.obj"));
}

// Place all non-OpenGL once-per-frame code here.
void
MultiPassDemoApp::update()
{
    // Calculate elapsed time since last frame.
    double now = getElapsedSeconds();
    double elapsed = now - lastTime;
    lastTime = now;

    // Animate the rotation angle.
    angle += (M_PI / 10) * elapsed;

    // Check now whether any files changed.
    rtr::watcher.checkForChanges();
}

// Place all once-per-frame drawing code here.
void
MultiPassDemoApp::draw()
{
    // Clear background to gray.
    gl::clear(Color(0.5, 0.5, 0.5));

	// Setup a perspective projection camera.
	CameraPersp camera(getWindowWidth(), getWindowHeight(), 35.0f, 0.1f, 10.0f);
	camera.lookAt(cameraPos, cameraPos + cameraFront);

	// Push the view-projection matrix to the bottom of the matrix stack.
	gl::setMatrices(camera);

    // Enable depth buffering.
    gl::enableDepthWrite();
    gl::enableDepthRead();

    // Save current model-view-projection matrix by pushing a new matrix on top.
    gl::pushModelMatrix();

    // Apply the rotation around the diagonal unit axis.
    //gl::rotate(angle, vec3(1, 1, 1));

	doMovement();

    // Draw the duck model.
    //scene_->draw();
	duck->draw();

    // Restore the previous model-view-projection matrix.
    gl::popModelMatrix();
}

void 
MultiPassDemoApp::doMovement(){
	float cameraSpeed = 0.05;
	if (keys[KeyEvent::KEY_w])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[KeyEvent::KEY_a])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[KeyEvent::KEY_s])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[KeyEvent::KEY_d])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void
MultiPassDemoApp::keyDown(KeyEvent event){
	int key = event.getCode();
	keys[key] = true;
	/*switch (key) {
	case KeyEvent::KEY_w:
		w = true;
		keys[key] = true;
		break;
	case KeyEvent::KEY_a:
		a = true;
		break;
	case KeyEvent::KEY_s:
		s = true;
		break;
	case KeyEvent::KEY_d:*/


	//}
}
void
MultiPassDemoApp::keyUp(KeyEvent event){
	int key = event.getCode();
	keys[key] = false;
}
void
MultiPassDemoApp::mouseMove(MouseEvent event){

}

void
prepareSettings(MultiPassDemoApp::Settings* settings)
{
    // Required for Retina display and such.
    settings->setHighDensityDisplayEnabled();
}

// Instantiate the cinder-based rendering app.
CINDER_APP(MultiPassDemoApp, RendererGl, prepareSettings)
