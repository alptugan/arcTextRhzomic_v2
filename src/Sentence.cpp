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
	xPrev = 0;
	yPrev = 0;
	zPrev = 0;


	myString = str;
    myFont = _font;

	speed = 0.01f+ofRandomf()/100.f;

	yStart = 100 + ofRandom(ofGetHeight()-200);
    
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
    
    for (int i = 0; i < strLen; i++) {
    /*
        
        Letter *l;
        
        char myArray[1];
        strcpy(myArray,utf8_substr2(myString,i,1).c_str());
        
        l->letter_ = &myArray;*/
        
        letters.push_back(utf8_substr2(myString,i,1));
        
        //cout <<  << endl;
    }
    
    
}


Sentence::~Sentence()
{
}

void Sentence::update(){
   
}


void Sentence::draw(){
    
	ofSetColor(0,225,0);

	int i = 0;

	float xoff = 0; 
	float zoff = 0; 

	string currentChar; 
	float x = 0;
	float z = 0;
    
    
    
	//for (float x = 0; x <= ofGetWidth(); x += 10) {
	while(x <= ofGetWidth()){
		
		
		// Calculate a y value according to noise, map to 
		float y = ofMap(ofNoise(xoff, yoff), 0, 1, 0,yStart+100); // Option #1: 2D Noise
		float z = 10*ofNoise(zoff);
    
		// Set the vertex
		if(x > 0){
			//ofLine(xPrev,yPrev,x,y);
            
			if(i < strLen){

				

				ofVec2f tanVec = ofVec2f(xPrev,yPrev) - ofVec2f(x,y);

				float yDiff = tanVec.y;
				float xDiff = tanVec.x;

				float arcTan = ofRadToDeg(atan(yDiff / xDiff));
                
                ofPushMatrix();
				ofTranslate(x,y);
				ofRotateZ(arcTan);
				//float scaleVal = ofMap(x, 0, ofGetWidth(), 0, 0.9);
                float p = ofGetMouseX() / ofGetWidth();
                float scaleVal = ofMap(x/ofGetMouseX(), 0, ofGetWidth(), 0, 0.9);
                //cout<< p << endl;
                float alphaVal = ofMap(x,0,ofGetWidth(),10,255);
                
                ofScale(1-scaleVal,1-scaleVal,1);
                
                ofSetColor(255-(alphaVal));
				if(i > startShine && i< stopShine){
					ofSetColor(225,0,0);
				}else{
					ofSetColor(0,200,0);
				}
                
				myFont->drawBatch(letters[i],24, 0, 0);
                ofPopMatrix();
			}
		
		i++;

		}
	

		xPrev = x;
		yPrev = y;
		zPrev = z;
		// Increment x dimension for noise
		//xoff += ofMap(mouseX, 0, ofGetWidth(), 0,0.1);
		xoff += 0.008;
		//zoff += 0.008;

		x += 10;	


	}
    
    yoff += speed;
    
	

} 

void Sentence::setup(float speed){
   
	
}

void Sentence::highLight(float _startShine, float _stopShine){
   
	startShine = _startShine;
	stopShine = _stopShine;
}

