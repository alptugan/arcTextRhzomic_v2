#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	ofBackground(0);
    
    
    // Interaction
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(320,240);
    colorImg.allocate(320,240);
    
    threshold = 80;
    
    actionThreshold[0] = 500;
    actionThreshold[1] = 500;
    actionThreshold[2] = 500;
    actionThreshold[3] = 500;
    actionThreshold[4] = 500;
    
    // Read Sentences from database
    sqlite = new ofxSQLite("rizomi.db");
    ofxSQLiteSelect sel = sqlite->select("id, word").from("words");
	sel.execute().begin();
    
	while(sel.hasNext()) {
		int id = sel.getInt();
		std::string name = sel.getString();
		//cout << id << ", " << name << endl;
		sel.next();
        //if(id < 1)
        name = ofUTF8::toUpper(name);
        texts.push_back(name);
	}
    
    
    // load fonts
    font = new ofxFontStash();
    font->setup("mono.ttf"); //load verdana font, set lineHeight to be 130%
    
    
    SentenceNum = texts.size();
    SentenceNum = 30;
    cout << "Sentence size : "<< SentenceNum << endl;
    for (int j = 0; j < SentenceNum; j++) {
        Sentence *sentence = new Sentence(texts[j],font, 0, 35 + 24*j);
        sentences.push_back(sentence);
    }
    
	fbo.allocate(1024, 768);
    
    fbo.begin();
    ofBackground(0);
    fbo.end();
    
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    fbo.begin();
    ofClear(0);
    
    for (int j = 0; j < 100; j++) {
        /*if(lineAcounter % 10 == 0) {
            lineAlpha = lineAlpha - 1;
            
            if(lineAlpha < 10)
                lineAlpha=255;
        }*/
        
        
        
        drawLines(0,200,j,j*10);
    }
    fbo.end();
    
    lineAcounter++;
    
    
    // Interaction
    bool bNewFrame = false;
    
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();
    
    if (bNewFrame){
        
        
        colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
        
        grayImage = colorImg;
		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}
        
		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);
        
        if(mirror){
            grayDiff.mirror(false, true);
        }
        
        ofPixelsRef pixels = grayDiff.getPixelsRef();
        
        for(int i = 0; i<5 ; i++){
            weights[i] = 0;
        }
        
        for(int y = 0; y < grayDiff.getHeight(); y++){
            for(int x = 0; x < grayDiff.getWidth(); x++){
                
                if(pixels.getColor(x, y) != ofColor(0,0,0,255)){
                    
                    if(x < grayDiff.getWidth() * 0.2){
                        
                        weights[0]++;
                        
                    }
                    
                    if(x > grayDiff.getWidth() * 0.2 && x < grayDiff.getWidth() * 0.4){
                        
                        weights[1]++;
                        
                    }
                    
                    if(x > grayDiff.getWidth() * 0.4 && x < grayDiff.getWidth() * 0.6){
                        
                        weights[2]++;
                        
                    }
                    
                    if(x > grayDiff.getWidth() * 0.6 && x < grayDiff.getWidth() * 0.8){
                        
                        weights[3]++;
                        
                    }
                    
                    if(x > grayDiff.getWidth() * 0.8 && x < grayDiff.getWidth()){
                        
                        weights[4]++;
                        
                    }
                    
                    
                }
                
            }
        }
        
        grayBg = grayImage;
        
        
	}
}

//--------------------------------------------------------------
void testApp::draw(){
    fbo.draw(0, 0);
    grayDiff.draw(20,20);
    ofSetColor(255);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, 14);
    
    
    
    font->beginBatch();
    for (int j = 0; j < SentenceNum; j++) {
        
        //font->drawBatch(texts[j], 24, 0, 35+20 * j);
        
        
        
        sentences[j]->draw();
        
    }
    
    
    font->endBatch();
    
    for(int i = 0; i<5 ; i++){
        if(weights[i] > actionThreshold[i]){
            
            ofDrawBitmapString("Region "+ofToString(i), 500, 330+i*20);
            
        }
    }
    
}

void testApp::drawLines(int min,int max, float _xoff, float _yoff) {
    float xoff = _xoff;
    int i = 0;
    
    for (float x = 0; x < ofGetWidth(); x += 10) {
        
		
        // Calculate a y value according to noise, map to
        float y = _yoff+ofMap(ofNoise(xoff+_xoff, yoff), 0, 1, min,max);
        
        if(x >0){
            ofSetLineWidth(1);
            ofFill();
            //ofSetHexColor(10);
            //int alphaVal = ofMap(x,0,ofGetWidth(),0,255);

            
            ofSetColor(0,100,0);
            
            ofLine(xPrev,yPrev,x,y);
            //ofLine(xPrev,(yPrev-20),x,(y-20));
            /* ofSetColor(225);
             */
            
            i++;
            
        }
        
        
        xPrev = x;
        yPrev = y;
        // Increment x dimension for noise
        //xoff += ofMap(mouseX, 0, ofGetWidth(), 0,0.01);
        xoff += 0.01;
	}
	// increment y dimension for noise
	//yoff += ofMap(mouseY, 0, ofGetHeight(), 0,0.01);
	yoff += 0.00005;

    
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
