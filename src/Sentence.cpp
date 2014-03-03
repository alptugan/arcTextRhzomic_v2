#include "Sentence.h"

string utf8_substr2(const string &str,int start, int length=INT_MAX)
{
    int i,ix,j,realstart,reallength;
    if (length==0) return "";
    if (start<0 || length <0)
    {
        //find j=utf8_strlen(str);
        for(j=0,i=0,ix=str.length(); i<ix; i+=1, j++)
        {
            unsigned char c= str[i];
            if      (c>=0   && c<=127) i+=0;
            else if (c>=192 && c<=223) i+=1;
            else if (c>=224 && c<=239) i+=2;
            else if (c>=240 && c<=247) i+=3;
            else if (c>=248 && c<=255) return "";//invalid utf8
        }
        if (length !=INT_MAX && j+length-start<=0) return "";
        if (start  < 0 ) start+=j;
        if (length < 0 ) length=j+length-start;
    }
    
    j=0,realstart=0,reallength=0;
    for(i=0,ix=str.length(); i<ix; i+=1, j++)
    {
        if (j==start) { realstart=i; }
        if (j>=start && (length==INT_MAX || j<=start+length)) { reallength=i-realstart; }
        unsigned char c= str[i];
        if      (c>=0   && c<=127) i+=0;
        else if (c>=192 && c<=223) i+=1;
        else if (c>=224 && c<=239) i+=2;
        else if (c>=240 && c<=247) i+=3;
        else if (c>=248 && c<=255) return "";//invalid utf8
    }
    if (j==start) { realstart=i; }
    if (j>=start && (length==INT_MAX || j<=start+length)) { reallength=i-realstart; }
    
    return str.substr(realstart,reallength);
}



Sentence::Sentence(string str, ofxFontStash *_font, int sPosX,int sPosY)
{
	yoff = 0.0;


	myString = str;
    myFont = _font;

	

	yStart = sPosY;
    
    //currentChar = utf8_substr2(myString,i,1);
    
    strLen = myString.length();
    
    //
    /*
    for(vector<Letter*>::iterator it = myString.begin(); it != myString.end(); ++it) {
        //Letter l;
        //l.letter_ = utf8_substr2(myString,i,1);
        letters.push_back(*it);
        cout << *it << endl;
    }*/
    //myString =  ofUTF8::repair(myString);
    //char myArray[myString.size()];
    //strcpy(myArray, utf8_substr2(myString,0,strLen).c_str());
    
    letters.resize(strLen);
    
    for (int i = 0; i < strLen; i++) {
    /*
        
        Letter *l;
        
        char myArray[1];
        strcpy(myArray,utf8_substr2(myString,i,1).c_str());
        
        l->letter_ = &myArray;*/
        
        letters[i] = utf8_substr2(myString,i,1);
        
        //cout <<  << endl;
    }
}


Sentence::~Sentence()
{
}

void Sentence::update(){
   
}


void Sentence::draw(){
    
	//ofSetColor(0,120,0);

	int i = 0;

	xoff = 0;

	string currentChar; 
	float x = 0;
    p = ofGetMouseX();
    l = p / 10;
    //cout << l << endl;
	myFont->beginBatch();
	while(x <= ofGetWidth()){
		
		
		// Calculate a y value according to noise, map to 
		float y = yStart + ofMap(ofNoise(xoff, yoff), 0, 1, 0,400); // Option #1: 2D Noise
    
		// Set the vertex
		if(x > 0){
            line.addVertex(ofVec2f(x,y));
			if(i < strLen){

				ofVec2f tanVec = ofVec2f(xPrev,yPrev) - ofVec2f(x,y);

				float yDiff = tanVec.y;
				float xDiff = tanVec.x;

				float arcTan = ofRadToDeg(atan(yDiff / xDiff));
                
                ofPushMatrix();
				ofTranslate(x,y);
				ofRotateZ(arcTan);
                //cout << line[i].getRotatedRad(x, y, 1.0f).z << endl;
                //cout << arcTan << endl;
				//float scaleVal = ofMap(x, 0, ofGetWidth(), 0, 0.9);
               
                float scaleVal = ofMap(x, 0, ofGetWidth(), 0, 0.9);
                
                float alphaVal = ofMap(x,0,ofGetWidth(),10,255);
                
                ofScale(scaleFac,scaleFac,1);
                
                
                ofSetColor(255-(alphaVal));
				if(i > startShine && i< stopShine){
					ofSetColor(225,0,0);
				}else{
					ofSetColor(0,120,0);
				}
                
				myFont->drawBatch(letters[i],24, 0, 0);
                
                ofPopMatrix();
			}
		
		i++;

		}
	

		xPrev = x;
		yPrev = y;
		// Increment x dimension for noise
		//xoff += ofMap(p, 0, ofGetWidth(), 0,0.1);
        xoff += xFactor;

		x += 10;
        
        
	}
    myFont->endBatch();
    
    //yoff += ofMap(ofGetMouseY(), 0, ofGetHeight(), 0.001,0.05);
    yoff += speed;
    
	line.draw();
    
    line.clear();

} 

void Sentence::setSpeed(float _speed){
   
	speed = _speed;
}

void Sentence::setFactorNoiseX(float _x){
    
	xFactor = _x;
}

void Sentence::setScale(float _scale) {
    scaleFac = _scale;
}

void Sentence::highLight(float _startShine, float _stopShine){
   
	startShine = _startShine;
	stopShine = _stopShine;
}

