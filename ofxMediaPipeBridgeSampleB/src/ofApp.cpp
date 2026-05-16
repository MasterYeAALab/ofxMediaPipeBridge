#include "ofApp.h"

// --- 所有的特征索引定义 (MediaPipe 标准) ---
const vector<int> FACE_OVAL = {10,  338, 297, 332, 284, 251, 389, 356, 454,
							   323, 361, 288, 397, 365, 379, 378, 400, 377,
							   152, 148, 176, 149, 150, 136, 172, 58,  132,
							   93,  234, 127, 162, 21,  54,  103, 67,  109};
const vector<int> LIPS = {61,  146, 91,  181, 84,  17, 314, 405, 321, 375, 291,
						  308, 324, 318, 402, 317, 14, 87,  178, 88,  95,  185};
const vector<int> L_EYE = {263, 249, 390, 373, 374, 380, 381, 382,
						   362, 398, 384, 385, 386, 387, 388, 466};
const vector<int> R_EYE = {33,  7,   163, 144, 145, 153, 154, 155,
						   133, 173, 157, 158, 159, 160, 161, 246};

// 极简 6 点眉毛回路 (确保绝对不交叉)
// 建议在 ofApp.cpp 中改为这种更完整的回路顺序
const vector<int> L_BROW = {285, 295, 282, 283, 276, 300,
							293, 334, 296, 336, 285};
const vector<int> R_BROW = {55, 65, 52, 53, 46, 70, 63, 105, 66, 107, 55};

const vector<int> NOSE_BRIDGE = {168, 6, 197, 195, 5};
const vector<int> NOSE_BASE = {98, 97, 2, 326, 327};
const vector<int> L_IRIS = {473, 474, 475, 476, 477};
const vector<int> R_IRIS = {468, 469, 470, 471, 472};

const vector<int> HAND_CONN = {0,  1,  1,  2,  2,  3,  3,  4,  0,  5,  5,
							   6,  6,  7,  7,  8,  5,  9,  9,  10, 10, 11,
							   11, 12, 9,  13, 13, 14, 14, 15, 15, 16, 13,
							   17, 17, 18, 18, 19, 19, 20, 0,  17};
const vector<int> POSE_CONN = {
	11, 12, 12, 24, 24, 23, 23, 11,         // Torso
	12, 14, 14, 16, 16, 18, 16, 20, 16, 22, // Right Arm & Hand
	11, 13, 13, 15, 15, 17, 15, 19, 15, 21, // Left Arm & Hand
	24, 26, 26, 28, 28, 30, 28, 32, 30, 32, // Right Leg & Foot
	23, 25, 25, 27, 27, 29, 27, 31, 29, 31  // Left Leg & Foot
};

void ofApp::setup() {
  ofSetBackgroundColor(30, 30, 30);
  ofSetCircleResolution(60);
  ofSetFrameRate(60);

  ofEnableSmoothing();
  ofEnableAntiAliasing();

  // 监听端口 9000
  receiver.setup(9000);

  // 初始化 (Map 不需要手动置零)
}

void ofApp::update() {
  float now = ofGetElapsedTimef();

  // 1. 自动清空逻辑 (清除超过 0.5 秒未更新的对象)
  auto clearStale = [&](map<int, vector<ofDefaultVec3>>& ptsMap, map<int, float>& timers) {
	for (auto it = timers.begin(); it != timers.end(); ) {
	  if (now - it->second > 0.5) {
		ptsMap.erase(it->first);
		it = timers.erase(it);
	  } else {
		++it;
	  }
	}
  };
  clearStale(allFacePoints, faceTimers);
  clearStale(allHandPoints, handTimers);
  clearStale(allPosePoints, poseTimers);

  // 2. OSC 接收
  while (receiver.hasWaitingMessages()) {
	ofxOscMessage m;
	receiver.getNextMessage(m);
	string addr = m.getAddress();
	int oid = m.getArgAsInt(1); // 获取对象 ID (Face ID, Hand ID 等)

	if (addr == "/ofxmp/faces") {
	  parseLandmarks(m, allFacePoints[oid]);
	  faceTimers[oid] = now;
	} else if (addr == "/ofxmp/hands") {
	  parseLandmarks(m, allHandPoints[oid]);
	  handTimers[oid] = now;
	} else if (addr == "/ofxmp/poses") {
	  parseLandmarks(m, allPosePoints[oid]);
	  poseTimers[oid] = now;
	}
  }
}

void ofApp::draw() {
  ofNoFill();
  ofSetLineWidth(2.0);

  // --- 绘制脸部 ---
  for (auto const& [id, facePoints] : allFacePoints) {
	if (facePoints.size() >= 478) {
	  drawContour(facePoints, FACE_OVAL, ofColor::white, true);
	  drawContour(facePoints, LIPS, ofColor::cyan, true);
	  drawContour(facePoints, L_EYE, ofColor::white, true);
	  drawContour(facePoints, R_EYE, ofColor::white, true);
	  drawContour(facePoints, L_BROW, ofColor::blue, false);
	  drawContour(facePoints, R_BROW, ofColor::green, false);
	  drawContour(facePoints, NOSE_BRIDGE, ofColor::white, false);
	  drawContour(facePoints, NOSE_BASE, ofColor::white, false);

	  ofFill();
	  ofSetColor(ofColor::blue);
	  ofDrawCircle(facePoints[L_IRIS[0]].x * ofGetWidth(), facePoints[L_IRIS[0]].y * ofGetHeight(), 4);
	  ofSetColor(ofColor::green);
	  ofDrawCircle(facePoints[R_IRIS[0]].x * ofGetWidth(), facePoints[R_IRIS[0]].y * ofGetHeight(), 4);
	  ofNoFill();
	}
  }

  // --- 绘制手部 (遍历所有检测到的手) ---
  for (auto const& [id, handPoints] : allHandPoints) {
	if (handPoints.size() >= 21) {
	  drawSkeleton(handPoints, HAND_CONN, ofColor(200));
	}
  }

  // --- 绘制身体 ---
  for (auto const& [id, posePoints] : allPosePoints) {
	if (posePoints.size() >= 33) {
	  drawSkeleton(posePoints, POSE_CONN, ofColor(150));
	}
  }

  // 状态
  ofSetColor(255);
  ofDrawBitmapString("Faces: " + ofToString(allFacePoints.size()), 20, 20);
  ofDrawBitmapString("Hands: " + ofToString(allHandPoints.size()), 20, 40);
  ofDrawBitmapString("Poses: " + ofToString(allPosePoints.size()), 20, 60);
}

// --- 辅助函数定义 ---

void ofApp::drawContour(const vector<ofDefaultVec3> &pts,
						const vector<int> &indices, ofColor c, bool close) {
  ofSetColor(c);
  ofBeginShape();
  for (int idx : indices) {
	if (idx < pts.size()) {
	  ofVertex(pts[idx].x * ofGetWidth(), pts[idx].y * ofGetHeight());
	}
  }
  ofEndShape(close);
}

void ofApp::drawSkeleton(const vector<ofDefaultVec3> &pts,
						 const vector<int> &connections, ofColor c) {
  ofSetColor(c);
  for (int i = 0; i < connections.size(); i += 2) {
	int a = connections[i];
	int b = connections[i + 1];
	if (a < pts.size() && b < pts.size()) {
	  ofDrawLine(pts[a].x * ofGetWidth(), pts[a].y * ofGetHeight(),
				 pts[b].x * ofGetWidth(), pts[b].y * ofGetHeight());
	}
  }
}

void ofApp::parseLandmarks(const ofxOscMessage &m,
						   vector<ofDefaultVec3> &targetList) {
  targetList.clear();
  int numArgs = m.getNumArgs();
  if (numArgs < 3)
	return;
  for (int i = 2; i <= numArgs - 3; i += 3) {
	targetList.push_back(ofDefaultVec3(
		m.getArgAsFloat(i), m.getArgAsFloat(i + 1), m.getArgAsFloat(i + 2)));
  }
}
