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

	void keyDown(KeyEvent event) override {
		cameraNav_.keyDown(event);
	}

	void keyUp(KeyEvent event) override {
		cameraNav_.keyUp(event);
	}

	void mouseMove(MouseEvent event) override{
		cameraMouseNav_.mouseMove(event);
	}

	void mouseDrag(MouseEvent event) override{
		cameraMouseNav_.mouseDrag(event);
	}

	void mouseDown(MouseEvent event) override{
		cameraMouseNav_.mouseDown(event);
	}

	void mouseWheel(MouseEvent event) override{
		cameraMouseNav_.mouseWheel(event);
	}

  private:
	  NodeRef camera_, root_, scene_, model_;
	  AbsolutePositionNavigator cameraNav_;
	  TrackballNavigator cameraMouseNav_;

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

	model_ = Node::create({ duck });

	scene_ = Node::create({}, glm::rotate(toRadians(-90.0f), vec3(0, 1, 0)), { model_ });

	camera_ = Node::create({}, translate(vec3(0, 0.2, 4)));
	root_ = Node::create({}, mat4(), { scene_, camera_ });

	cameraNav_ = AbsolutePositionNavigator(camera_, root_);

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
	// Setup a perspective projection camera.
	CameraPersp camera(getWindowWidth(), getWindowHeight(), 35.0f, 0.1f, 20.0f);
	gl::setMatrices(camera);
	//mat4 toView = inverse(root_->find(camera_)[0].transform);
	mat4 toView = inverse(cameraNav_.toWorld());
	//ci::app::console() << "toView is: " << toView << std::endl;

	gl::setViewMatrix(toView);

    // Enable depth buffering.
    gl::enableDepthWrite();
    gl::enableDepthRead();

    // Save current model-view-projection matrix by pushing a new matrix on top.
    gl::pushModelMatrix();

    // Apply the rotation around the diagonal unit axis.
    //gl::rotate(angle, vec3(1, 1, 1));

    // Draw the duck model.
    scene_->draw();

    // Restore the previous model-view-projection matrix.
    gl::popModelMatrix();
}

void
prepareSettings(MultiPassDemoApp::Settings* settings)
{
    // Required for Retina display and such.
    settings->setHighDensityDisplayEnabled();
}

// Instantiate the cinder-based rendering app.
CINDER_APP(MultiPassDemoApp, RendererGl, prepareSettings)
