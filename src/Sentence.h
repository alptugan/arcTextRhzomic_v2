#pragma once

#include "stdio.h"
#include "ofMain.h"
#include "ofxFontStash.h"
#include <set>
#include "ofUTF8.h"
#include "ofxTween.h"

struct Letter{
    char * letter_;
    int posx,posy;
};

class Sentence
{
public:
	Sentence(string str, ofxFontStash *font, int sPosX,int sPosY);
	~Sentence();

	void update();  // update method, used to refresh your objects properties
    void draw();    // draw method, this where you'll do the object's drawing 
 
    

	string myString;
    int strLen;
	ofxFontStash* myFont;

	void setSpeed(float);
    void setFactorNoiseX(float _x);
    void setScale(float _scale);
    void setColor(int color);
   
    float scaleFac = 1;
	float speed;

	float yStart;
    float yoff;
    float xoff = 0;
    float xFactor;
	float xPrev;
	float yPrev;
    float p ;//left of the mouse
    float r ;//right of the mouse
    float l; // no of left segments
    float m; // no of right segments
    float incr;
    int indy = 0;
    
    int col ;
    int colDown = 50;
    int col2;
    
	vector<int> startShine,stopShine;
    
    bool useScaleFac = true;

	void highLight(vector<ofVec2f> hl,int i);
    void clearHighLight();
    
    ofPolyline line;
    
    vector<string> letters;
    
    

 private: // place private functions or variables declarations here

};

