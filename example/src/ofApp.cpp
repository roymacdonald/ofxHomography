#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	originalRect.set(0,0,ofGetWidth(), ofGetHeight());
	originalRect.scaleFromCenter(0.7);
	
	originalCorners = ofxHomography::rectangleToCorners(originalRect);
	
	distortedCorners.resize(4);
	for(int i =0; i < 4; i ++){
	  distortedCorners[i].x = originalCorners[i].x;
	  distortedCorners[i].y = originalCorners[i].y;
	}
	
}

//--------------------------------------------------------------
void ofApp::updateHomography(){
	homography = ofxHomography::findHomography(originalCorners, distortedCorners);
	
	distPoly.clear();
	for(auto& p: poly){
		distPoly.addVertex(ofxHomography::toDestinationCoordinates(p, homography));
	}
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	// Define a point to be drawn in the warped space
	glm::vec3 point(originalRect.x + originalRect.width *0.8, originalRect.y + originalRect.height *0.8,0);

	glm::vec3 warpedMouse = ofxHomography::toSourceCoordinates({ofGetMouseX(), ofGetMouseY(),0}, homography);
	
	
	ofPushMatrix();
	glMultMatrixf(glm::value_ptr(homography));
	
	ofSetColor(ofColor::yellow);
	ofDrawRectangle(originalRect);
	
	ofSetColor(ofColor::black);
	ofDrawCircle(originalRect.getCenter(), 50);
	
	// Draw a point in the warped space
	ofSetColor(ofColor::red);
	ofDrawCircle(point, 10);
	


	ofSetColor(ofColor::magenta);
	ofDrawCircle(warpedMouse, 10);

	ofPopMatrix();


	for(int i = 0; i < 4; i++){
		distortedCorners[i].draw();
	}

	ofPushStyle();
	ofSetColor(0, 120);
	ofSetLineWidth(4);
	poly.draw();
	distPoly.draw();
	ofPopStyle();
	
	// Draw the screen coordinates of that point
	ofSetColor(ofColor::black);
	glm::vec3 pointInScreen = ofxHomography::toDestinationCoordinates(point, homography);
	ofDrawBitmapString("Local coordinates " + ofToString(point) + "\nScreen coordinates " + ofToString(pointInScreen) , pointInScreen);
	
	
	
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	bool bPoly = true;
	for(int i = 0; i < 4; i++){
		if(distortedCorners[i].mouseDragged({x,y})){
			bPoly = false;
		}
	}
	if(bPoly){
		poly.lineTo(x,y);
	}else{
		updateHomography();
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	bool bPoly = true;
	for(int i = 0; i < 4; i++){
		if(distortedCorners[i].mousePressed({x,y})){
			bPoly = false;
		}
	}
	if(bPoly){
		poly.clear();
		poly.addVertex(x,y);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bool bUpdateHomography = false;
	for(int i = 0; i < 4; i++){
		if(distortedCorners[i].mouseReleased()){
			bUpdateHomography = true;
		}
	}
	if(bUpdateHomography){
		updateHomography();
	}
}
