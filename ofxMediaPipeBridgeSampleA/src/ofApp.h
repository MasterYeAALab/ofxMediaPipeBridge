#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	ofxOscReceiver receiver;
	
	// 存储特征点
	vector<ofDefaultVec3> facePoints;
	vector<ofDefaultVec3> handPoints;
	vector<ofDefaultVec3> posePoints;
	
	// 计时器（用于处理出镜消失）
	float lastFaceTime, lastHandTime, lastPoseTime;
	
	// 解析函数
	void parseLandmarks(const ofxOscMessage& m, vector<ofDefaultVec3>& targetList);
};
