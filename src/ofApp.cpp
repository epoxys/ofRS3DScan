#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	guiDisplayStatus = true;

	mRSSDK = RSDevice::createUniquePtr();
	
	if (mRSSDK->initScan()) {
		
		ofLog(OF_LOG_NOTICE, "Camera initialized");
		camInit = true;

		mTexRgb.allocate(1280, 720, GL_RGBA);

	}
	else {

		ofLog(OF_LOG_ERROR, "Failed to initialize camera.");
		camInit = false;

	}
	
	/*
	if (!mRSSDK->init())
	{
		ofLogError("Unable to create ofxRSSDK object");
		exit();
	}
	else
	{
		mRSSDK->initDepth(DepthRes::F200_VGA, 30, true);
		mRSSDK->initRgb(RGBRes::HD1080, 30);
		
		mTexDepth.allocate(mRSSDK->getDepthWidth(), mRSSDK->getDepthHeight(), GL_RGBA);
		mRSSDK->start();
	}
	*/

}

//--------------------------------------------------------------
void ofApp::update(){

	mRSSDK->scanUpdate();
	
	

	mTexRgb.loadData(mRSSDK->getRgbFrame(), GL_BGRA);
	mTexDepth.loadData(mRSSDK->getDepth8uFrame());
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofClear(ofColor::black);

	if (guiDisplayStatus) displayScanStatus();
	ofSetColor(255);
	ofDrawBitmapString("OF FPS:      " + ofToString(ofGetFrameRate()), 200, 10, 0);
	ofDrawBitmapString("SCAN ACTIVE: " + ofToString(mRSSDK->isScanActive()), 200, 20, 0);
	
	/*
	ofSetColor(255);

	//display scan states (key g)
	if (guiDisplayStatus) displayScanStatus();
	
	ofClear(ofColor::black);
	mTexRgb.draw(0, 0);
	ofDrawBitmapString("Raw Color", ofPoint(10, 20));
	ofPushMatrix();
	ofTranslate(640, 0);
	mTexDepth.draw(0, 0, 640, 480);
	ofDrawBitmapString("Depth Pixels", ofPoint(10, 20));
	ofPopMatrix();
	*/
	ofSetColor(255);
	mTexRgb.draw(0,100,640,360);
	mTexDepth.draw(650, 100, 480, 360);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch (key) {
		case 'g':
			guiDisplayStatus ? guiDisplayStatus = false : guiDisplayStatus = true;
		break;
		case 's':
			mRSSDK->isScanActive() ? mRSSDK->scanCancel() : mRSSDK->scanStart();
		break;
		case 'd':
			if (mRSSDK->isScanActive()) mRSSDK->saveScan();
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
void ofApp::displayScanStatus() {
	if (mRSSDK->FaceScan_StatusDetected() == 0) {
		ofSetColor(0, 150, 0);
		ofDrawBitmapString("Face Detected", 10, 10, 0);
	}
	else {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("No Face Detected", 10, 10, 0);
	}

	if (mRSSDK->FaceScan_StatusXAxis() == 0) {
		ofSetColor(0, 150, 0);
		ofDrawBitmapString("Horizontally Centered", 10, 20, 0);
	}
	else if (mRSSDK->FaceScan_StatusXAxis() == 1) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Move Left", 10, 20, 0);
	}
	else if (mRSSDK->FaceScan_StatusXAxis() == 2) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Move Right", 10, 20, 0);
	}

	if (mRSSDK->FaceScan_StatusYAxis() == 0) {
		ofSetColor(0, 150, 0);
		ofDrawBitmapString("Vertically Centered", 10, 30, 0);
	}
	else if (mRSSDK->FaceScan_StatusYAxis() == 1) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Move Down", 10, 30, 0);
	}
	else if (mRSSDK->FaceScan_StatusYAxis() == 2) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Move Up", 10, 30, 0);
	}

	if (mRSSDK->FaceScan_StatusZAxis() == 0) {
		ofSetColor(0, 150, 0);
		ofDrawBitmapString("Depth in range", 10, 40, 0);
	}
	else if (mRSSDK->FaceScan_StatusZAxis() == 1) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Move away", 10, 40, 0);
	}
	else if (mRSSDK->FaceScan_StatusZAxis() == 2) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Move closer", 10, 40, 0);
	}

	if (mRSSDK->FaceScan_StatusYaw() == 0) {
		ofSetColor(0, 150, 0);
		ofDrawBitmapString("Yaw in range", 10, 50, 0);
	}
	else if (mRSSDK->FaceScan_StatusYaw() == 1) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Turn right", 10, 50, 0);
	}
	else if (mRSSDK->FaceScan_StatusYaw() == 2) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Turn left", 10, 50, 0);
	}

	if (mRSSDK->FaceScan_StatusPitch() == 0) {
		ofSetColor(0, 150, 0);
		ofDrawBitmapString("Pitch in Range", 10, 60, 0);
	}
	else if (mRSSDK->FaceScan_StatusPitch() == 1) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Turn down", 10, 60, 0);
	}
	else if (mRSSDK->FaceScan_StatusPitch() == 2) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Turn up", 10, 60, 0);
	}


	if (mRSSDK->Scan_StatusRange() == 0) {
		ofSetColor(0, 150, 0);
		ofDrawBitmapString("In Range", 10, 70, 0);
	}
	else if (mRSSDK->Scan_StatusRange() == 1) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Too Close", 10, 70, 0);
	}
	else if (mRSSDK->Scan_StatusRange() == 2) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Too far", 10, 70, 0);
	}

	if (mRSSDK->Scan_StatusTracking() == 0) {
		ofSetColor(0, 150, 0);
		ofDrawBitmapString("Tracking object", 10, 80, 0);
	}
	else if (mRSSDK->Scan_StatusTracking() == 1) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Tracking lost", 10, 80, 0);
	}

	if (mRSSDK->Scan_StatusStructure() == 0) {
		ofSetColor(0, 150, 0);
		ofDrawBitmapString("Sufficient Structure", 10, 90, 0);
	}
	else if (mRSSDK->Scan_StatusStructure() == 1) {
		ofSetColor(150, 0, 0);
		ofDrawBitmapString("Insufficient Structure", 10, 90, 0);
	}
}
