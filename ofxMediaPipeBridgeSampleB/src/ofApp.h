#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void extracted();
	
	void draw();

	ofxOscReceiver receiver;
	
	// 存储特征点 (支持多对象)
	map<int, vector<ofDefaultVec3>> allFacePoints;
	map<int, vector<ofDefaultVec3>> allHandPoints;
	map<int, vector<ofDefaultVec3>> allPosePoints;
	
	// 计时器 (支持多对象)
	map<int, float> faceTimers, handTimers, poseTimers;
	
	// 解析与绘制辅助函数
	void parseLandmarks(const ofxOscMessage& m, vector<ofDefaultVec3>& targetList);
	void drawContour(const vector<ofDefaultVec3>& pts, const vector<int>& indices, ofColor c, bool close = false);
	void drawSkeleton(const vector<ofDefaultVec3>& pts, const vector<int>& connections, ofColor c);
};
