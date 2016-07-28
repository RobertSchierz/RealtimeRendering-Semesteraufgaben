//
//  Copyright 2016 Henrik Tramberend, Hartmut Schirmacher
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"

#include "RTR/RTR.h"
#include "NodeNavigator.hpp"
#include "cinder/ImageIo.h"

#include <windows.h>
#include <winuser.h>

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

	void resize() override {
		myFbo = makeFBO_();
		//secondFbo = makeFBO_();
	}

    // Called once for every frame to be rendered.
    void draw() override;

	void drawScene_();

	void drawPostProcess_(gl::FboRef fbo_);

	void drawSecondPostProcess_(gl::FboRef fbo_);

	void keyDown(KeyEvent event) override;
	void keyUp(KeyEvent event) override;

	void mouseMove(MouseEvent event) override;

	void mouseDrag(MouseEvent event) override;

	void mouseDown(MouseEvent event) override;

	void mouseUp(MouseEvent event) override;

	void mouseWheel(MouseEvent event) override;

  private:

	  gl::FboRef makeFBO_();

	  void doMovement();

	  bool firstMouse = true;

	  vec2 lastPos;
	  float lastX;
	  float lastY;

	  float yaw = 0;
	  float pitch = 0;

	  NodeRef camera_, root_, scene_, model_;
	  /*AbsolutePositionNavigator cameraNav_;
	  TrackballNavigator cameraMouseNav_;*/
	  float fov = 45.0f;
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
	double deltaTime;

    // Model of the duck that is displayed.
    rtr::ModelRef duck;
	rtr::ModelRef scene;
	rtr::ModelRef blaster;

	gl::FboRef secondFbo;
	gl::FboRef myFbo;

	gl::GlslProgRef motionBlur;
	rtr::MaterialRef blurMaterial;
};

// Place all one-time setup code here.
void
MultiPassDemoApp::setup()
{
    // Arange for the window to always be on top. This helps with live shader
    // coding
    //getWindow()->setAlwaysOnTop();
	//setFullScreen(true);

	hideCursor();

	lastPos = getWindowCenter();	

    // Create a live-reloading shader program.
    auto lambert = rtr::watcher.createWatchedProgram(
      { getAssetPath("lambert.vert"), getAssetPath("lambert.frag") });

	motionBlur = rtr::watcher.createWatchedProgram(
	{ getAssetPath("blur_vert.vert"), getAssetPath("blur_frag.frag") });
	blurMaterial = rtr::Material::create(motionBlur);

	myFbo = makeFBO_();
	secondFbo = makeFBO_();


    // Load the duck model and use the lambert shader on it.
    duck = rtr::loadObjFile(getAssetPath("duck/duck.obj"), true, lambert);
	scene = rtr::loadObjFile(getAssetPath("Scene/Scene.obj"), true, lambert);
	//blaster = rtr::loadObjFile(getAssetPath("Pistol/Pistol.obj"), true, lambert);

	model_ = Node::create({ scene }, glm::scale(vec3(4,4,4) ));

	//scene_ = Node::create({}, glm::rotate(toRadians(-90.0f), vec3(0, 1, 0)), { model_ });

	//camera_ = Node::create({}, translate(vec3(0, 0.2, 4)));
	//root_ = Node::create({}, mat4(), { scene_, camera_ });

	//cameraNav_ = AbsolutePositionNavigator(camera_, root_);

}

gl::FboRef MultiPassDemoApp::makeFBO_(){
	auto size = getWindowSize();
	auto format = gl::Fbo::Format().colorTexture().depthTexture();
	auto fbo = gl::Fbo::create(size.x, size.y, format);

	auto colorTex = fbo->getColorTexture();
	auto depthTex = fbo->getDepthTexture();
	blurMaterial->texture("depthTex", depthTex);
	blurMaterial->uniform("depthValue", 0.995f);
	blurMaterial->uniform("isAiming", false);
	
	blurMaterial->texture("tex", colorTex);
	//blurMaterial->uniform("resolution", size.x);

	return fbo;
}

// Place all non-OpenGL once-per-frame code here.
void
MultiPassDemoApp::update()
{
    // Calculate elapsed time since last frame.
    double now = getElapsedSeconds();
    deltaTime = now - lastTime;
    lastTime = now;

    // Animate the rotation angle.
    angle += (M_PI / 10) * deltaTime;

    // Check now whether any files changed.
    rtr::watcher.checkForChanges();
}

// Place all once-per-frame drawing code here.
void
MultiPassDemoApp::draw()
{
	myFbo->bindFramebuffer();
	drawScene_();
	myFbo->unbindFramebuffer();
	secondFbo->bindFramebuffer();
	drawPostProcess_(myFbo);
	secondFbo->unbindFramebuffer();
	drawSecondPostProcess_(secondFbo);
}

void MultiPassDemoApp::drawScene_(){
	// Clear background to gray.
	gl::clear(Color(0.5, 0.5, 0.5));

	// Setup a perspective projection camera.
	CameraPersp camera(getWindowWidth(), getWindowHeight(), fov, 0.01f, 100.0f);
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
	//duck->draw();
	//blaster->draw();
	model_->draw();

	// Restore the previous model-view-projection matrix.
	gl::popModelMatrix();
}

void MultiPassDemoApp::drawPostProcess_(gl::FboRef fbo_){
	//set matrices
	gl::pushMatrices();
	gl::clear(Color(0, 0, 0));
	gl::setMatricesWindow(getWindowSize());

	blurMaterial->uniform("sampleOffset", vec2(1.0f / fbo_->getWidth(), 0.0f));

	//draw full-screen textures rectangle
	blurMaterial->bind();
	//gl::drawSolidRect(fbo_->getBounds());

	//restore matrices
	gl::popMatrices();
}

void MultiPassDemoApp::drawSecondPostProcess_(gl::FboRef fbo_){
	//set matrices
	gl::pushMatrices();
	gl::clear(Color(0, 0, 0));
	gl::setMatricesWindow(getWindowSize());

	blurMaterial->uniform("sampleOffset", vec2(0.0f, 1.0f / fbo_->getHeight()));

	//draw full-screen textures rectangle
	blurMaterial->bind();
	gl::drawSolidRect(fbo_->getBounds());

	//restore matrices
	gl::popMatrices();
}

void 
MultiPassDemoApp::doMovement(){
	float cameraSpeed = 3 * deltaTime;
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

	if (key == KeyEvent::KEY_ESCAPE){
		exit(0);
	}else{
		keys[key] = true;
	}
}
void
MultiPassDemoApp::keyUp(KeyEvent event){
	int key = event.getCode();
	keys[key] = false;
}

void
MultiPassDemoApp::mouseDown(MouseEvent event){
	if (event.isRightDown()){
		blurMaterial->uniform("isAiming", true);
		fov = 35.0f;
	}
}

void MultiPassDemoApp::mouseUp(MouseEvent event){
	if (!event.isRightDown()){
		blurMaterial->uniform("isAiming", false);
		fov = 45.0f;
	}
}

void MultiPassDemoApp::mouseWheel(MouseEvent event){
	
	float scrolled = event.getWheelIncrement();
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= scrolled;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

void
MultiPassDemoApp::mouseDrag(MouseEvent event){

}

void
MultiPassDemoApp::mouseMove(MouseEvent event){
	//TEST
	float xpos = event.getX();
	float ypos = event.getY();

	//Reset Cursor Position to middle
	if (xpos < 50 || xpos > getWindowWidth() - 50 || ypos < 50 || ypos > getWindowHeight() - 50) {
		POINT pt;
		lastX = pt.x = getWindowWidth() / 2;
		lastY = pt.y = getWindowHeight() / 2;

		HWND hWnd = getRenderer()->getHwnd();
		::ClientToScreen(hWnd, &pt);
		::SetCursorPos(pt.x, pt.y);
	}

	//console() << xpos << " " << ypos << endl;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
		//console() << "INIT " << xpos << " " << ypos << endl;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
	lastX = xpos;
	lastY = ypos;

	//console() << xoffset << " " << yoffset << endl;

	GLfloat sensitivity = 0.05;	// Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	//console() << yaw << " " << pitch << endl;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void
prepareSettings(MultiPassDemoApp::Settings* settings)
{
    // Required for Retina display and such.
    settings->setHighDensityDisplayEnabled();
}

// Instantiate the cinder-based rendering app.
CINDER_APP(MultiPassDemoApp, RendererGl, prepareSettings)
