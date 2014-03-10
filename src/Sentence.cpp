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
    
	int i = 0;

	xoff = 0;

	float x = 0;
    
    
    // If there are no start/stop index do not calculate  scale factors
    if(startShine.size() > 0 ) {
        p = floor(10*(startShine[0] + stopShine[0]) / 2);
        r = ofGetWidth() - p;
    
        l = p / 10;
        m = r / 10;
    
        float incr = 1 / l;
    }
    
	myFont->beginBatch();
	while(x <= ofGetWidth()){
		
		
		// Calculate a y value according to noise, map to 
		float y = yStart + ofMap(ofNoise(xoff, yoff), 0, 1, 0,400); // Option #1: 2D Noise
    
		// Set the vertex
		//if(x > 0){
            line.addVertex(ofVec2f(x,y));
			if(i < strLen){

				ofVec2f tanVec = ofVec2f(xPrev,yPrev) - ofVec2f(x,y);

				float yDiff = tanVec.y;
				float xDiff = tanVec.x;

				float arcTan = ofRadToDeg(atan(yDiff / xDiff));
                
                ofPushMatrix();
				ofTranslate(x,y);
				ofRotateZ(arcTan);
               
                float alphaVal = ofMap(x,0,ofGetWidth(),10,255);
               // useScaleFac = false;
                
                float scaleFac2 = incr;
                
                //cout << incr << endl;
                /*
                if(!useScaleFac) {
                    ofScale(scaleFac2,scaleFac2,1);
                   
                }
                else {
                    ofScale(scaleFac2,scaleFac2,1);
                }
                
                */
                
                
                
                
                
                
                 //cout << incr << endl;
                
                //ofSetColor(255-(alphaVal));
                
                // If start/stop index is lower than 0 do nothing set colors and incr to zero
                if(startShine.size()>0) {
                    
                    if(x > startShine[0] * 10 && x < stopShine[0] * 10){
                        
                        // incr = 1;
                        
                        
                    }else{
                        
                        if(x <= p){
                            
                            incr += 1/l;
                            
                        }else{
                            incr -= 1/m;
                        }
                        
                    }
                    
				if((i >= startShine[0] && i< stopShine[0]) || (i >= startShine[1] && i< stopShine[1])|| (i >= startShine[2] && i< stopShine[2])|| (i >= startShine[3] && i< stopShine[3]) || (i >= startShine[4] && i< stopShine[4])
        ){
                    
					ofSetColor(0,col,0,255);
                    ofScale(scaleFac,scaleFac,1);
                    //ofScale(1,1,1);
				}else{
                    ofScale(scaleFac,scaleFac,1);
                    //ofScale(0.8,0.8,1);
					ofSetColor(0,100,0,255);
				}
                    
                }else{
                    ofScale(scaleFac,scaleFac,1);
					//ofScale(0.2,0.2,1);
                    ofSetColor(0,100,0,255);
				}
                
				myFont->drawBatch(letters[i],24, 0, 0);
                
                ofPopMatrix();
			//}
		
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
    ofSetColor(0,100,0,255);
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
    //col = ofMap(scaleFac,0.2,1,50,255);
}

void Sentence::setColor(int _color) {
    col = _color;
}

void Sentence::highLight(vector<ofVec2f> hl, int _id){

     stopShine.resize(stopShine.size()+1);
     startShine.resize(startShine.size()+1);
     startShine[startShine.size()-1] = hl[_id].x;
     stopShine[stopShine.size()-1] = hl[_id].y;
     
     //cout << hl[_id].y << " , " <<hl[_id].x << endl;

    //cout << _id << endl;
}

void Sentence::clearHighLight() {
    startShine.clear();
    stopShine.clear();
}