#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SimpleLightingApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void SimpleLightingApp::setup()
{
}

void SimpleLightingApp::mouseDown( MouseEvent event )
{
}

void SimpleLightingApp::update()
{
}

void SimpleLightingApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( SimpleLightingApp, RendererGl )
