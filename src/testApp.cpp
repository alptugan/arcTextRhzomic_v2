#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofEnableAlphaBlending();
	ofSetVerticalSync(true);
    
    
	ofSetFrameRate(30);
    ofBackground(0);
    
    
    
    ofHideCursor();
    
    
    ofSetFullscreen(true);
    
    // Interaction
    vidGrabber.setVerbose(true);
    vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(320,240);
    
    colorImg.allocate(320,240);
    
    actionThreshold[0] = 10;
    actionThreshold[1] = 10;
    actionThreshold[2] = 10;
    actionThreshold[3] = 10;
    actionThreshold[4] = 10;
    
    
    // GUI
    
    thres.push_back(thres1);
    thres.push_back(thres2);
    thres.push_back(thres3);
    thres.push_back(thres4);
    thres.push_back(thres5);
    
    
    gui.setup();
    gui.add(fps.setup("FrameRate",framerate));
    gui.add(camSwitch.setup("Show Video",false));
    gui.add(mirror.setup("Mirror Video",true));
    gui.add(thres1.setup("Threshold 1st Region" ,actionThreshold[0],0,15000));
    gui.add(thres2.setup("Threshold 2st Region" ,actionThreshold[1],0,15000));
    gui.add(thres3.setup("Threshold 3st Region" ,actionThreshold[2],0,15000));
    gui.add(thres4.setup("Threshold 4st Region" ,actionThreshold[3],0,15000));
    gui.add(thres5.setup("Threshold 5st Region" ,actionThreshold[4],0,15000));
    gui.add(threshold.setup("Threshold Gray Image" ,80,0,200));
    gui.add(timeLimit.setup("Set Delay Time" ,8,0,40));
    
    
    gui.setPosition(ofGetWidth() - 210, ofGetWindowHeight() - 300);
    
    // Load recent values at startup
    gui.loadFromFile("settings.xml");
    
    // Read Sentences from database
    sqlite = new ofxSQLite("rizomi.db");
    /*
    sqlite->simpleQuery(""\
                        "CREATE TABLE IF NOT EXISTS highlight (" \
                        " id INTEGER" \
                        " ,start INTEGER" \
                        " ,stop INTEGER" \
                        ");"
                        );
     sqlite->insert("highlight")
     .use("id", 0)
     .use("start",0)
     .use("stop",3).execute();

     */
    
    
    ofxSQLiteSelect sel = sqlite->select("id, word").from("words");
	sel.execute().begin();
    
	while(sel.hasNext()) {
		int id = sel.getInt();
		std::string name = sel.getString();
		sel.next();
        name = ofUTF8::toUpper(name);
        texts.resize(texts.size()+1);
        texts[texts.size()-1] = name;
	}
    
    
    // load font
    font = new ofxFontStash();
    font->setup("mono.ttf"); //load verdana font, set lineHeight to be 130%
    
    // Create Sentence Objects
    SentenceNum = texts.size();
    SentenceNum = 30;
    cout << "Sentence size : "<< SentenceNum << endl;
    

    // 2D Vector Array
    // to hold highlighted letter indexes
    highlights.resize(SentenceNum);
    vector<ofVec2f> pp;
    
    // Loop through database to get indexes
    for (int j = 0; j < SentenceNum; j++) {
        
        sel = sqlite->select("id, start, stop").from("highlight").where("id", j);
        sel.execute().begin();
        
        while(sel.hasNext()) {
            int id = sel.getInt();
            int start = sel.getInt();
            int stop = sel.getInt();
            pp.resize(pp.size()+1);
            pp[pp.size()-1]=ofVec2f(start,stop);
            //cout << id << ", " << pp[pp.size()] << endl;
            sel.next();
        }

     
        highlights[j] = pp;
        pp.clear();
    }
    

    // Get sentences and generate sentences
    sentences.resize(SentenceNum);
    speedVals.resize(SentenceNum);
    
    for (int j = 0; j < SentenceNum; j++) {
        Sentence *sentence = new Sentence(texts[j],font, 0, 12*j);
        sentences[j] = sentence;
        speedVals[j] = ofRandom(0.0008, 0.005);
        sentences[j]->setSpeed(speedVals[j]);
        sentences[j]->setFactorNoiseX(ofRandom(0.003, 0.02));
    }
    
    
   // Background Lines
    bgLineNum = 80;
    for (int i=0;  i< bgLineNum; i++) {
        bgLineAlpha.resize(bgLineAlpha.size()+1);
        bgLineAlpha[bgLineAlpha.size()-1] = ofMap(i, 0, bgLineNum, 0, 100);
    }
    
    // Hide Cursor
    CGDisplayHideCursor(NULL);
}

//--------------------------------------------------------------
void testApp::update(){
    
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
    
    
    if(timer.getSeconds() > timeLimit) {
        
        timer.reset();
        isResetCalled=false;
        isNoMovement = true;
    }

    if(!multitween.isCompleted() || !tweenexpo.isCompleted()) {
        for (int j = 0; j < SentenceNum; j++) {
            sentences[j]->setScale(tweenexpo.update());
            sentences[j]->speed = multitween.update();
            sentences[j]->speed = multitween.getTarget((j+1));
            sentences[j]->setColor(tweenexpoColor.update());
        }
        //setIdleMode();
        //cout << "bitti"<<endl;
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    //Ugly hack move background line upper
    ofPushMatrix();
    ofTranslate(0, -200);
    if(camSwitch)
        grayDiff.draw(20,20,ofGetWidth(),ofGetHeight());
    
    for (int j = 0; j < bgLineNum; j++) {
        ofSetColor(0, 255, 0,bgLineAlpha[j]);
        drawLines(0,200,j,j*10,ofMap(j, 0, 80, 1, 8));
    }
    ofPopMatrix();
    
    // Start to draw sentences
    ofSetLineWidth(1);
    for (int j = 0; j < SentenceNum; j++) {

        sentences[j]->draw();
        //font->draw(texts[j], 24, 0, 20*(j)+20);
        
    }
    
    
    // Hide/Show gui
    if( !bHide ){
    
		gui.draw();
        ofSetColor(255);
        fps = ofToString(ofGetFrameRate());
	}
    
    // Check if there is any viewer in front of related region

        if(weights[0] > thres1){
            if( !bHide ){
                ofSetColor(255);
                ofDrawBitmapString("Region "+ofToString(0), 500, 330+0*20);
            }
            
            // Loop through all sentences to set multiple highlighted words
            for (int j = 0; j < SentenceNum; j++) {
                sentences[j]->highLight( highlights[j],0 );
            }
             //if(!multitween.isCompleted() || !tweenexpo.isCompleted())
            resetToDefaultValues();

        }else{
            setIdleMode();
        }
    
    if(weights[1] > thres2){
        if( !bHide ){
            ofSetColor(255);
            ofDrawBitmapString("Region "+ofToString(1), 500, 330+1*20);
        }
        
        // Loop through all sentences to set multiple highlighted words
        for (int j = 0; j < SentenceNum; j++) {
            sentences[j]->highLight( highlights[j],1 );
        }
        //if(!multitween.isCompleted() || !tweenexpo.isCompleted())
        resetToDefaultValues();
        
    }else{
        setIdleMode();
    }
    
    
    if(weights[2] > thres3){
        if( !bHide ){
            ofSetColor(255);
            ofDrawBitmapString("Region "+ofToString(2), 500, 330+2*20);
        }
        
        // Loop through all sentences to set multiple highlighted words
        for (int j = 0; j < SentenceNum; j++) {
            sentences[j]->highLight( highlights[j],2 );
        }
        //if(!multitween.isCompleted() || !tweenexpo.isCompleted())
        resetToDefaultValues();
        
    }else{
        setIdleMode();
    }
    
    if(weights[3] > thres4){
        if( !bHide ){
            ofSetColor(255);
            ofDrawBitmapString("Region "+ofToString(3), 500, 330+3*20);
        }
        
        // Loop through all sentences to set multiple highlighted words
        for (int j = 0; j < SentenceNum; j++) {
            sentences[j]->highLight( highlights[j],3 );
        }
        //if(!multitween.isCompleted() || !tweenexpo.isCompleted())
        resetToDefaultValues();
        
    }else{
        setIdleMode();
    }
    
    
    if(weights[4] > thres5){
        if( !bHide ){
            ofSetColor(255);
            ofDrawBitmapString("Region "+ofToString(4), 500, 330+4*20);
        }
        
        // Loop through all sentences to set multiple highlighted words
        for (int j = 0; j < SentenceNum; j++) {
            sentences[j]->highLight( highlights[j],4 );
        }
        //if(!multitween.isCompleted() || !tweenexpo.isCompleted())
        resetToDefaultValues();
        
    }else{
        setIdleMode();
    }
    
}

//
// Reset Animation points to original state
//
void testApp::resetToDefaultValues() {
    
    if(!isResetCalled) {
        isResetCalled = true;
        //cout << "reset called" << endl;
        multitween.setParameters(easingexpoSpeed,ofxTween::easeOut,0,ofGetWidth()-100,duration,delay2);
        for (int j = 0; j < SentenceNum; j++) {
            
            multitween.addValue(sentences[j]->speed, speedVals[j]);
            
            //sentences[j]->useScaleFac=false;
            //tweenexpo.setParameters(2,easingexpoSpeed,ofxTween::easeOut,sentences[j]->speed,speedVals[j],duration,delay2);
            //sentences[j]->setFactorNoiseX(ofRandom(0.008, 0.01));
            //sentences[j]->setScale(1);
        }
        multitween.start();
        
        timer.start();
        
        // Tween Values
        scaleValStart = 0.2;
        scaleValEnd = 1;
        tweenexpo.setParameters(1,easingexpo,ofxTween::easeOut,scaleValStart,scaleValEnd,duration,delay);
        tweenexpoColor.setParameters(2, easingexpo, ofxTween::easeOut, 50, 255, duration, delay);
        /*bgNoise.setParameters(3,easingexpo, easingexpo, ofxTween::easeOut, 0.00004, <#float to#>, <#unsigned int duration#>, <#unsigned int delay#>)*/
    }
}

//
// Set idle mode if there is no interaction with cam
//
void testApp::setIdleMode() {
    
    if(isNoMovement) {
        
        multitween.setParameters(easingexpoSpeed,ofxTween::easeOut,0,ofGetWidth()-100,duration,delay2);
        
        for (int j = 0; j < SentenceNum; j++) {
            //sentences[j]->setSpeed(0.0008);
            //tweenexpo.setParameters(2,easingexpoSpeed,ofxTween::easeOut,sentences[j]->speed,0.0008,duration,delay2+(j*10));
            multitween.addValue(sentences[j]->speed, 0.0008);
            //sentences[j]->useScaleFac=true;
        }
        multitween.start();
        // Tween Values
        scaleValStart = 1;
        scaleValEnd = 0.2;
        tweenexpo.setParameters(1,easingexpo,ofxTween::easeOut,scaleValStart,scaleValEnd,duration,delay);
        
        isNoMovement = false;
        
        // Clear index arrays
        for (int j = 0; j < SentenceNum; j++) {
            sentences[j]->clearHighLight();
        }
        
    }
}


//
// Draw background lines
//
void testApp::drawLines(int min,int max, float _xoff, float _yoff,int linew) {
    float xoff = _xoff;
    
    for (float x = 0; x < ofGetWidth(); x += 20) {
        
		
        // Calculate a y value according to noise, map to
        float y = _yoff+ofMap(ofNoise(xoff+_xoff, yoff), 0, 1, min,max);
        
        ofSetLineWidth(linew);
            
        line.addVertex(ofPoint(x,y));
        
        
        
        xoff += 0.04;
	}
	yoff += 0.00004;
    
    line.draw();
    line.clear();
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    // hide gui
    if( key == 'h' ){
		bHide = !bHide;
        
        if(bHide){
            // ofHideCursor();
            CGDisplayHideCursor(NULL);
        }else{
            //  ofShowCursor();
            CGDisplayShowCursor(NULL);
        }
	}
    
    // save current settings
	if(key == 's') {
		gui.saveToFile("settings.xml");
	}
    
    // load settings from file / reset to default values
	if(key == 'l') {
		gui.loadFromFile("settings.xml");
	}
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
