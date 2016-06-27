//
//  Copyright 2016 Henrik Tramberend, Hartmut Schirmacher
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "RTR/RTR.h"

using namespace ci;
using namespace ci::app;
using namespace std;

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

  private:
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
    getWindow()->setAlwaysOnTop();

    // Create a live-reloading shader program.
    auto lambert = rtr::watcher.createWatchedProgram(
      { getAssetPath("lambert.vert"), getAssetPath("lambert.frag") });

    // Load the duck model and use the lambert shader on it.
    duck = rtr::loadObjFile(getAssetPath("duck/duck.obj"), true, lambert);

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
    camera.lookAt(vec3(0, 0, 4), vec3(0, 0, 0));

    // Push the view-projection matrix to the bottom of the matrix stack.
    gl::setMatrices(camera);

    // Enable depth buffering.
    gl::enableDepthWrite();
    gl::enableDepthRead();

    // Save current model-view-projection matrix by pushing a new matrix on top.
    gl::pushModelMatrix();

    // Apply the rotation around the diagonal unit axis.
    gl::rotate(angle, vec3(1, 1, 1));

    // Draw the duck model.
    duck->draw();

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
