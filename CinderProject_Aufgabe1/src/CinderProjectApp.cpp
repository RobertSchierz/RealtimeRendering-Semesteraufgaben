//
//  2016-03-07 Created by Henrik Tramberend
//  2016-04-06 Modified by Hartmut Schirmacher
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "RTR/RTR.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// app template for the Real Time Rendering course, inherits from Cinder App
class CinderProjectApp : public App
{
  public:
    
    // called once when the App starts, after OpenGL has been initialized
    void setup() override;
    
    // called for every frame to be rendered, before draw()
    void update() override;
    
    // called once for every frame to be rendered
    void draw() override;

	void keyDown() override;

  private:
    
    // rotation
    double angle = 0.0;
    
    // tracking time between two draw() calls
    double lastTime = getElapsedSeconds();

    // models
    rtr::ModelRef graka;

    // used to track when files have changed
    rtr::WatchThis watcher;
};


// place all one-time setup code here
void
CinderProjectApp::setup()
{
    // Create a live-reloading shader program.
    auto lambert = watcher.createWatchedProgram(
      { getAssetPath("lambert.vert"), getAssetPath("lambert.frag") });

    // Load the duck model and use the lambert shader on it.
	graka = rtr::loadObjFile(getAssetPath("graka/Geforce8500.obj"), true, lambert);

    // Try this version of the duck to see the default shader for OBJ
    // models.
    // duck = rtr::loadObjFile(getAssetPath("duck/duck.obj"));

    // update duck whenever a related asset changes
	watcher.watchForUpdates({ graka });
}

// place all non-OpenGL once-per-frame code here
void
CinderProjectApp::update()
{
    // calculate elapsed time since last frame
    double now = getElapsedSeconds();
    double elapsed = now - lastTime;
    lastTime = now;

    // animate the rotation angle
    angle += (M_PI / 10) * elapsed;

    // check now whether any files changed
    watcher.checkForChanges();
}

// place all once-per-frame drawing code here
void
CinderProjectApp::draw()
{
    // clear background
    gl::clear(Color(0.5, 0.5, 0.5));

    // setup a perspective projection camera
    CameraPersp camera(getWindowWidth(), getWindowHeight(), 35.0f, 0.1f, 10.0f);
    camera.lookAt(vec3(0, 0, 4), vec3(0, 0, 0));
    
    // push the view-projection matrix to the bottom of the matrix stack
    gl::setMatrices(camera);

    // enable depth buffering
    gl::enableDepthWrite();
    gl::enableDepthRead();

    // save current model-view-projection matrix by pushing a new matrix on top
    gl::pushModelMatrix();

    // apply the rotation around the diagonal unit axis
    gl::rotate(angle, vec3(1, 1, 1));

    // draw the duck model
	graka->draw();

    // restore the previous model-view-projection matrix
    gl::popModelMatrix();
}

void
CinderProjectApp::keyDown()
{
	
}

void
prepareSettings(CinderProjectApp::Settings* settings)
{
    // required for Retina display and such
    settings->setHighDensityDisplayEnabled();
}

// instantiate the cinder-based rendering app
CINDER_APP(CinderProjectApp, RendererGl, prepareSettings)
