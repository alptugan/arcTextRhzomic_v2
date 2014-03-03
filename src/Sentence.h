#pragma once

#include "stdio.h"
#include "ofMain.h"
#include "ofxFontStash.h"
#include <set>
#include "ofUTF8.h"

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

   
    float scaleFac = 1;
	float speed;

	float yStart;
    float yoff;
    float xoff = 0;
    float xFactor;
	float xPrev;
	float yPrev;
    float p ;
    float l;

	float startShine,stopShine;

	void highLight(float,float);
    
    ofPolyline line;
    
    vector<string> letters;

 private: // place private functions or variables declarations here
};

