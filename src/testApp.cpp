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
    
    actionThreshold[0] = 10;
    actionThreshold[1] = 10;
    actionThreshold[2] = 10;
    actionThreshold[3] = 10;
    actionThreshold[4] = 10;
    
    
    // GUI
    gui.setup();
    gui.add(fps.setup("FrameRate",framerate));
    gui.add(camSwitch.setup("show Video",false));
    gui.add(thres1.setup("Threshold 1st Region" ,actionThreshold[0],0,1000));
    gui.add(thres2.setup("Threshold 2st Region" ,actionThreshold[1],0,1000));
    gui.add(thres3.setup("Threshold 3st Region" ,actionThreshold[2],0,1000));
    gui.add(thres4.setup("Threshold 4st Region" ,actionThreshold[3],0,1000));
    gui.add(thres5.setup("Threshold 5st Region" ,actionThreshold[4],0,1000));
    gui.add(threshold.setup("Threshold Gray Image" ,80,0,200));
    gui.add(timeLimit.setup("Set Delay Time" ,8,0,40));
    thres.push_back(thres1);
    thres.push_back(thres2);
    thres.push_back(thres3);
    thres.push_back(thres4);
    thres.push_back(thres5);
    
    // Load recent values at startup
    gui.loadFromFile("settings.xml");
    
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
    
    sentences.resize(SentenceNum);
    
    for (int j = 0; j < SentenceNum; j++) {
        Sentence *sentence = new Sentence(texts[j],font, 0, 12*j);
        sentences[j] = sentence;
        sentences[j]->setSpeed(ofRandom(0.0008, 0.01));
        sentences[j]->setFactorNoiseX(ofRandom(0.003, 0.02));
    }
    
    
    // Tween Values
    /*unsigned duration = 1000;
    unsigned delay = 0;
    tweenexpo.setParameters(1,easingexpo,ofxTween::easeOut,0,1,duration,delay);*/
    
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
}

//--------------------------------------------------------------
void testApp::draw(){
    //Ugly hack move background line upper
    ofPushMatrix();
    ofTranslate(0, -200);
    if(camSwitch)
        grayDiff.draw(20,20,ofGetWidth(),ofGetHeight());
    
    for (int j = 0; j < 80; j++) {
        ofSetColor(0, 255, 0,ofMap(j, 0, 80, 0, 100));
        drawLines(0,200,j,j*10,ofMap(j, 0, 80, 1, 8));
    }
    ofPopMatrix();
    
    // Start to draw sentences
    ofSetLineWidth(1);
    
    for (int j = 0; j < SentenceNum; j++) {

        sentences[j]->draw();
        
    }
    
    
    // Hide/Show gui
    if( !bHide ){
    
		gui.draw();
	}
    
    // Check if there is any viewer in front of related region
    for(int i = 0; i<5 ; i++){
        
        if(weights[i] > thres[i]){
            if( !bHide ){
                ofSetColor(255);
                ofDrawBitmapString("Region "+ofToString(i), 500, 330+i*20);
            }
            resetToDefaultValues();

        }else{
            setIdleMode();
        }
    }
    
    ofSetColor(255);
    fps = ofToString(ofGetFrameRate());
    //ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, 14);
}

//
// Reset Animation points to original state
//
void testApp::resetToDefaultValues() {
    
    if(!isResetCalled) {
        isResetCalled = true;
        //cout << "reset called" << endl;
        for (int j = 0; j < SentenceNum; j++) {
            sentences[j]->setSpeed(ofRandom(0.008, 0.01));
            sentences[j]->setFactorNoiseX(ofRandom(0.008, 0.01));
             sentences[j]->setScale(1);
        }
        
        timer.start();
        
    }
}

//
// Set idle mode if there is no interaction with cam
//
void testApp::setIdleMode() {
    
    if(isNoMovement) {
        for (int j = 0; j < SentenceNum; j++) {
            sentences[j]->setSpeed(0.0008);
            sentences[j]->setScale(0.2);
            sentences[j]->setFactorNoiseX((j+4)*0.0003);
        }
        
        isNoMovement = false;
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
        
        if(x >0){
            ofSetLineWidth(linew);
            
            line.addVertex(ofPoint(x,y));
        }
        
        
        xoff += 0.01;
	}
	yoff += 0.00005;
    line.draw();
    line.clear();
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    // hide gui
    if( key == 'h' ){
		bHide = !bHide;
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
