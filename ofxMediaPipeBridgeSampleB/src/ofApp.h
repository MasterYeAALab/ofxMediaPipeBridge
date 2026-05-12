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
	
	// 计时器
	float lastFaceTime, lastHandTime, lastPoseTime;
	
	// 解析与绘制辅助函数
	void parseLandmarks(const ofxOscMessage& m, vector<ofDefaultVec3>& targetList);
	void drawContour(const vector<ofDefaultVec3>& pts, const vector<int>& indices, ofColor c, bool close = false);
	void drawSkeleton(const vector<ofDefaultVec3>& pts, const vector<int>& connections, ofColor c);
};
