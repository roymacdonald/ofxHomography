#pragma once

#include "ofMain.h"
#include "ofxHomography.h"

#define DRAG_RADIUS 10

class draggablePoint: public glm::vec3{
public:
	draggablePoint(){}
	draggablePoint(int x, int y):glm::vec3(x,y,0){}
	draggablePoint(float x, float y):glm::vec3(x,y,0){}
	//--------------------------------------------------------------
	bool mouseDragged(const glm::vec2& mouse ){
		if (bDragging) {
			set(mouse - clicOffset);
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------
	bool mousePressed( const glm::vec2& mouse ){
		if (isOver(mouse)) {
			bDragging = true;
			clicOffset = mouse - xy();
			return true;
		}
		return false;
	}
	//--------------------------------------------------------------
	bool mouseReleased(){
		bool bTemp = bDragging;
		bDragging = false;
		return bTemp;
	}
	//--------------------------------------------------------------
	bool isOver(const glm::vec2& v){
		return (glm::distance(v, xy())<= DRAG_RADIUS);
	}
	//--------------------------------------------------------------
	void set(const glm::vec2& p){
		x = p.x;
		y = p.y;
	}
	//--------------------------------------------------------------
	void draw(){
		if (bDragging) {
			ofPushStyle();
			ofNoFill();
			ofSetColor(ofColor::red);
			ofDrawCircle(x, y , DRAG_RADIUS);
			ofPopStyle();
		}
		ofPushStyle();
		ofFill();
		ofSetColor(ofColor::black);
		ofDrawCircle(x, y , 6);
		
		ofSetColor(ofColor::white);
		ofDrawCircle(x, y , 4);
		ofPopStyle();
		
	}
	//--------------------------------------------------------------
	bool isDragging(){return bDragging;}
protected:
	glm::vec2 clicOffset;
private:
	bool bDragging = false;
};

//--------------------------------------------------------------
//--------------------------------------------------------------

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void updateHomography();
	void draw();
	
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	
	ofRectangle originalRect;
	std::vector<glm::vec3> originalCorners;
	std::vector<draggablePoint> distortedCorners;
	glm::mat4 homography;

	ofPolyline poly, distPoly;
	
	
	
};
