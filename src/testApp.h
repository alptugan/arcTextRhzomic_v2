#pragma once

#include "ofMain.h"
#include "ofxSQLiteHeaders.h"
#include "ofxFontStash.h"
#include "Sentence.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "MSATimer.h"
#include "ofxTween.h"
/*
typedef {
    int i1;
    int i2;
}Indexes;
*/
class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
     void resetToDefaultValues();
    void setIdleMode();
    bool isResetCalled = false;
    bool isNoMovement = true;
		
    void drawLines(int min,int max, float _xoff, float _yoff,int linew);
    
    
    ofxFontStash* font;
    int SentenceNum;
    vector<Sentence*> sentences;
    vector<float> speedVals;
    
    float speed = 0.01f+ofRandomf()/100.f;
    float xPrev,yPrev;
    float yoff;
    
    
    // Interaction
    ofVideoGrabber 		vidGrabber;
    ofxCvColorImage			colorImg;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    bool				bLearnBakground;

    bool mirror;
    
    int weights[5];
    int actionThreshold[5];
    
    
    // GUI
    ofxPanel gui;
    ofxIntSlider thres1;
    ofxIntSlider thres2;
    ofxIntSlider thres3;
    ofxIntSlider thres4;
    ofxIntSlider thres5;
    ofxIntSlider threshold;
    ofxIntSlider timeLimit;
    ofxLabel fps;
    string framerate;
    vector<int> thres;
    ofxToggle camSwitch;
    bool bHide = false;
    
    // Background lines
    ofPolyline line;
    
    
    // TweenValues
    ofxTween multitween;
    ofxTween tweenexpo;
    ofxTween tweenexpoSpeed;
    ofxEasingExpo 	easingexpo;
    ofxEasingExpo 	easingexpoSpeed;
    float scaleValEnd = 1;
    float scaleValStart = 0.2;
    unsigned duration = 2000;
    unsigned delay = 0;
    unsigned delay2 = 500;
    
    // Hightlighted Indexes
    //vector<Indexes>
    
private:
    ofxSQLite* sqlite;
    vector<string> texts;
    msa::Timer timer;
    double stepCounter;
    
    float startTime;
};
