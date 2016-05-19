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
class SimpleLightingApp : public App
{
  public:

	  //SimpleLightingApp(){}
    // Called once when the App starts, after OpenGL has been initialized.
    void setup() override;

    // Called for every frame to be rendered, before draw().
    void update() override;

    // Called once for every frame to be rendered.
    void draw() override;

	void keyDown(KeyEvent event) override;

  private:
    // Rotation angle used for the animation.
    double angle = 0.0;

    // Tracking time between two draw() calls.
    double lastTime = getElapsedSeconds();

	rtr::MaterialRef phong;
	rtr::MaterialRef toon;
	rtr::MaterialRef circle;

	ci::gl::GlslProgRef phongProgram;
	ci::gl::GlslProgRef toonProgram;
	ci::gl::GlslProgRef circleProgram;

    // Model of the duck that is displayed.
    //rtr::ModelRef duck;
	rtr::ShapeRef mShape;

	ci::geom::Teapot teapot;
	ci::geom::Cube cube;
};

// Place all one-time setup code here.
void
SimpleLightingApp::setup()
{
    // Arange for the window to always be on top. This helps with live shader
    // coding
    getWindow()->setAlwaysOnTop();

    // Create a live-reloading shader program.
    //auto lambert = rtr::watcher.createWatchedProgram(
      //{ getAssetPath("lambert.vert"), getAssetPath("lambert.frag") });

	phongProgram = rtr::watcher.createWatchedProgram(
		{ getAssetPath("myphong.vert"), getAssetPath("myphong.frag") });

	/*toonProgram = rtr::watcher.createWatchedProgram(
		{ getAssetPath("celshading.vert"), getAssetPath("celshading.frag") });*/

	/*circleProgram = rtr::watcher.createWatchedProgram(
		{ getAssetPath("circle_shader.vert"), getAssetPath("circle_shader.frag") });*/

	phong = rtr::Material::create(phongProgram);
	toon = rtr::Material::create(toonProgram);
	circle = rtr::Material::create(circleProgram);

	phong->uniform("k_ambient", vec3(0.2, 0.2, 0.2));
	phong->uniform("k_diffuse", vec3(1, 1, 0));
	phong->uniform("k_specular", vec3(1, 1, 1));
	phong->uniform("shininess", (float)100);
	phong->uniform("ambientLightColor", vec3(0, 0, 0));
	phong->uniform("lightColor", vec3(1, 1, 1));
	phong->uniform("lightPositionEC", vec4(1, 3, 1, 1));

	//cel shading
	toon->uniform("k_ambient", vec3(0.2, 0.2, 0.2));
	toon->uniform("k_diffuse", vec3(1, 1, 0));
	toon->uniform("k_specular", vec3(1, 1, 1));
	toon->uniform("shininess", (float)100);
	toon->uniform("ambientLightColor", vec3(0, 0, 0));
	toon->uniform("lightColor", vec3(1, 1, 1));
	toon->uniform("lightPositionEC", vec4(1, 3, 1, 1));
	toon->uniform("numberOfShades", (float)4);

	circle->uniform("radius", (float)0.4);
	circle->uniform("density", (float)4);
	circle->uniform("background", vec3(0.4, 0.4, 0.4));

    // Load the duck model and use the lambert shader on it.
    //duck = rtr::loadObjFile(getAssetPath("duck/duck.obj"), true, phong);
	teapot = ci::geom::Teapot().subdivisions(80);
	mShape = rtr::Shape::create({ teapot }, toon);

    // The shader program can also be replaced after the fact.
    // duck = rtr::loadObjFile(getAssetPath("duck/duck.obj"));
    // for (auto& shape : duck->shapes)
    //     shape->replaceProgram(lambert);

    // Try this version of the duck to see the default shader for OBJ
    // models.
    // duck = rtr::loadObjFile(getAssetPath("duck/duck.obj"));
}

void
SimpleLightingApp::keyDown(KeyEvent event)
{
	//which key did the user press?
	auto c = event.getChar();
	switch (c) {
	case '1':
		mShape->replaceMaterial(phong);
	case '2':
		mShape->replaceMaterial(toon);
	case '3':
		mShape->replaceProgram(circleProgram);
		mShape->replaceMaterial(circle);
	default:
		break;
	}
}

// Place all non-OpenGL once-per-frame code here.
void
SimpleLightingApp::update()
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
SimpleLightingApp::draw()
{
    // Clear background to gray.
    gl::clear(Color(0.5, 0.5, 0.5));

    // Setup a perspective projection camera.
    CameraPersp camera(getWindowWidth(), getWindowHeight(), 35.0f, 0.1f, 10.0f);
    camera.lookAt(vec3(0, 0, 3), vec3(0, 0, 0));

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
    mShape->draw();

    // Restore the previous model-view-projection matrix.
    gl::popModelMatrix();
}

void
prepareSettings(SimpleLightingApp::Settings* settings)
{
    // Required for Retina display and such.
    settings->setHighDensityDisplayEnabled();
}

// Instantiate the cinder-based rendering app.
CINDER_APP(SimpleLightingApp, RendererGl, prepareSettings)
