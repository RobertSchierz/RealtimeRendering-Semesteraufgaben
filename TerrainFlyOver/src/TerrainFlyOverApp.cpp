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
class TerrainFlyOverApp : public App
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

  private:

	NodeRef camera_, root_, scene_, model_;

	AbsolutePositionNavigator cameraNav_;

    // Rotation angle used for the animation.
    double angle = 0.0;

	float speed = 0.01;
	float maxSpeed = 0.1;
	float acceleration = 0.001;

    // Tracking time between two draw() calls.
    double lastTime = getElapsedSeconds();

    // Model of the duck that is displayed.

	rtr::MaterialRef bumpmap;

	ci::gl::GlslProgRef bumpmapProgram;

	ci::geom::Plane plane;

	gl::Texture2dRef mNormalMap;
	gl::Texture2dRef mHeightMap;

	//gl::TextureBaseRef mNormalMapture;
	//ci::gl::GlslProgRef	mGlsl;

	std::vector<rtr::ShapeRef> mShapes;
	int shapeIndex;

	//bool mIsMovingForward = false;
	float zMov = 1.5;
};

// Place all one-time setup code here.
void
TerrainFlyOverApp::setup()
{
	shapeIndex = 0;
	// Arange for the window to always be on top. This helps with live shader
	// coding
	//getWindow()->setAlwaysOnTop();

	auto normalMapImg = loadImage(loadAsset("Rock_01_local.jpg"));
	mNormalMap = gl::Texture2d::create(normalMapImg);
	mNormalMap->setWrap(GL_REPEAT, GL_REPEAT);

	auto heightMapImg = loadImage(loadAsset("Rock_01_h.jpg"));
	mHeightMap = gl::Texture2d::create(heightMapImg);
	mHeightMap->setWrap(GL_REPEAT, GL_REPEAT);

	// Create a live-reloading shader program.
	bumpmapProgram = rtr::watcher.createWatchedProgram(
	{ getAssetPath("bumpmaping.vert"), getAssetPath("bumpmaping.frag") });

	bumpmap = rtr::Material::create(bumpmapProgram);

	bumpmap->uniform("k_ambient", vec3(0.2, 0.2, 0.2));
	bumpmap->uniform("k_diffuse", vec3(0.0, 0.4, 0.07));
	bumpmap->uniform("k_specular", vec3(1, 1, 1));
	bumpmap->uniform("shininess", (float)200);
	bumpmap->uniform("ambientLightColor", vec3(0, 0, 0));
	bumpmap->uniform("lightColor", vec3(1, 1, 1));
	bumpmap->uniform("lightPositionEC", vec4(1, 3, 1, 1));
	bumpmap->texture("normalMap", mNormalMap);
	bumpmap->texture("heightMap", mHeightMap);
	bumpmap->uniform("movSpeed", (float)0);

	plane = ci::geom::Plane().subdivisions(vec2(1000, 1000)).size(vec2(8, 8));
	ShapeRef planeShape = rtr::Shape::create({ plane }, bumpmap);
	ModelRef planemodel = rtr::Model::create({planeShape});
	model_ = Node::create({ planemodel });
	std::vector<rtr::NodeRef> mNodes;

	mNodes.push_back(Node::create({}, glm::translate(vec3(0, 0, 0)), { model_ }));
	/*mNodes.push_back(Node::create({}, glm::translate(vec3(0, 0, 4)), { model_ }));
	mNodes.push_back(Node::create({}, glm::translate(vec3(0, 0, -4)), { model_ }));
	mNodes.push_back(Node::create({}, glm::translate(vec3(4, 0, 0)), { model_ }));
	mNodes.push_back(Node::create({}, glm::translate(vec3(4, 0, 4)), { model_ }));
	mNodes.push_back(Node::create({}, glm::translate(vec3(4, 0, -4)), { model_ }));
	mNodes.push_back(Node::create({}, glm::translate(vec3(-4, 0, 0)), { model_ }));
	mNodes.push_back(Node::create({}, glm::translate(vec3(-4, 0, -4)), { model_ }));
	mNodes.push_back(Node::create({}, glm::translate(vec3(-4, 0, 4)), { model_ }));*/

	


	//model_ = Node::create({mNodes});
	scene_ = Node::create({}, glm::rotate(toRadians(-90.0f), vec3(0, 1, 0)), { mNodes });

	camera_ = Node::create({}, translate(vec3(0, 0.2, 0)));
	root_ = Node::create({}, mat4(), { scene_, camera_ });

	cameraNav_ = AbsolutePositionNavigator(camera_, root_);
}

// Place all non-OpenGL once-per-frame code here.
void
TerrainFlyOverApp::update()
{
    // Calculate elapsed time since last frame.
	double now = getElapsedSeconds();
    double elapsed = now - lastTime;
    lastTime = now;

	// Animate the rotation angle.
    angle += (M_PI / 10) * elapsed;
	

	speed += acceleration;
	bumpmap->uniform("movSpeed", speed);

	//if (cameraNav_.getForwardMovement()){
		/*if (speed < maxSpeed)
			cameraNav_.setSpeed(speed += acceleration);
		else if (speed > maxSpeed)
			cameraNav_.setSpeed(maxSpeed);*/
	//}
	//else{
	//	speed = 0.0;
	//}
	

    // Check now whether any files changed.
    rtr::watcher.checkForChanges();
}

// Place all once-per-frame drawing code here.
void
TerrainFlyOverApp::draw()
{
    // Clear background to gray.
    gl::clear(Color(0.5, 0.5, 0.5));

    // Setup a perspective projection camera.
    CameraPersp camera(getWindowWidth(), getWindowHeight(), 35.0f, 0.1f, 20.0f);
	gl::setMatrices(camera);
	//mat4 toView = inverse(root_->find(camera_)[0].transform);
	mat4 toView = inverse(cameraNav_.toWorld());
	//ci::app::console() << "toView is: " << toView << std::endl;

	//float z = toView[3][2];
	//ci::app::console() << "Value of z is: " << z << std::endl;
	//if (z > 4.0)
	//{
	//	
	//	toView[3][2] = -z;
	//}

	gl::setViewMatrix(toView);

	/*else
	{
		bumpmap->uniform("movSpeed", (float)0);
	}*/


    // Push the view-projection matrix to the bottom of the matrix stack.
    //gl::setMatrices(camera);

    // Enable depth buffering.
    gl::enableDepthWrite();
    gl::enableDepthRead();

    // Save current model-view-projection matrix by pushing a new matrix on top.
    gl::pushModelMatrix();

	scene_->draw();

    // Restore the previous model-view-projection matrix.
    gl::popModelMatrix();
}

void
prepareSettings(TerrainFlyOverApp::Settings* settings)
{
    // Required for Retina display and such.
    settings->setHighDensityDisplayEnabled();
}

// Instantiate the cinder-based rendering app.
CINDER_APP(TerrainFlyOverApp, RendererGl, prepareSettings)
