#include "ofApp.h"

void ofApp::setup() {
	ofSetBackgroundColor(20, 20, 20);
	ofSetFrameRate(60);
	
	// 监听端口 9000
	receiver.setup(9000);
	
	lastFaceTime = lastHandTime = lastPoseTime = 0;
}

void ofApp::update() {
	float now = ofGetElapsedTimef();
	
	// 1. 如果超过 0.5 秒没收到数据，清空点阵（防止画面残留）
	if (now - lastFaceTime > 0.5) facePoints.clear();
	if (now - lastHandTime > 0.5) handPoints.clear();
	if (now - lastPoseTime > 0.5) posePoints.clear();

	// 2. 处理 OSC 消息
	while (receiver.hasWaitingMessages()) {
		ofxOscMessage m;
		receiver.getNextMessage(m);
		
		string addr = m.getAddress();
		
		if (addr == "/ofxmp/faces") {
			parseLandmarks(m, facePoints);
			lastFaceTime = ofGetElapsedTimef();
		}
		else if (addr == "/ofxmp/hands") {
			parseLandmarks(m, handPoints);
			lastHandTime = ofGetElapsedTimef();
		}
		else if (addr == "/ofxmp/poses") {
			parseLandmarks(m, posePoints);
			lastPoseTime = ofGetElapsedTimef();
		}
	}
}

void ofApp::draw() {
	// 绘制脸部 (青色)
	ofSetColor(0, 255, 255);
	for (auto& p : facePoints) ofDrawCircle(p, 2);
	
	// 绘制手部 (紫色)
	ofSetColor(255, 0, 255);
	for (auto& p : handPoints) ofDrawCircle(p, 6);
	
	// 绘制姿态 (黄色)
	ofSetColor(255, 255, 0);
	for (auto& p : posePoints) ofDrawCircle(p, 8);
	
	// 状态显示
	ofSetColor(255);
	ofDrawBitmapString("Face: " + ofToString(facePoints.size()), 20, 20);
	ofDrawBitmapString("Hand: " + ofToString(handPoints.size()), 20, 40);
	ofDrawBitmapString("Pose: " + ofToString(posePoints.size()), 20, 60);
}

void ofApp::parseLandmarks(const ofxOscMessage& m, vector<ofDefaultVec3>& targetList) {
	targetList.clear();
	
	int numArgs = m.getNumArgs();
	// 协议：[0]=Long FrameID, [1]=Int ObjectID, [2...]=Float X,Y,Z
	if (numArgs < 3) return;
	
	// 解析坐标（每 3 个一组）
	for (int i = 2; i <= numArgs - 3; i += 3) {
		try {
			float x = m.getArgAsFloat(i);
			float y = m.getArgAsFloat(i + 1);
			// 映射归一化坐标到屏幕尺寸
			targetList.push_back(ofDefaultVec3(x * ofGetWidth(), y * ofGetHeight(), 0));
		} catch (...) {
			break;
		}
	}
}
