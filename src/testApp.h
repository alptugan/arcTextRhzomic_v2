#pragma once

#include "ofMain.h"
#include "ofxSQLiteHeaders.h"
#include "ofxFontStash.h"
#include "Sentence.h"
#include "ofxOpenCv.h"

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
		
    void drawLines(int min,int max, float _xoff, float _yoff);
    
    
    ofxFontStash* font;
    int SentenceNum;
    vector<Sentence*> sentences;
    
    ofFbo fbo;
    ofPolyline poli;
    
    float speed = 0.01f+ofRandomf()/100.f;
    float xPrev,yPrev;
    float yoff;
    int lineAcounter;
    int lineAlpha = 255;
    
    
    // Interaction
    ofVideoGrabber 		vidGrabber;
    ofxCvColorImage			colorImg;
    ofxCvGrayscaleImage 	grayDiff;
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    bool				bLearnBakground;
    int 				threshold;
    bool mirror;
    
    int weights[5];
    int actionThreshold[5];
    
private:
    ofxSQLite* sqlite;
    vector<string> texts;
};
