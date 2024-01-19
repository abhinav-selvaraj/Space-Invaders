#include "ofApp.h"

glm::vec3 velocity;
bool triIsPressed;
map<int, bool> keymap;

void TriangleShape::draw() {

	ofSetColor(ofColor::red);
	ofFill();
	ofPushMatrix();
	ofMultMatrix(getMatrix());
	ofDrawTriangle(verts[0], verts[1], verts[2]);
	ofPopMatrix();
}

glm::vec3 Player::heading() {
	glm::vec3 point = translate(verts[2]);
	glm::vec3 middle = translate(glm::vec3(0, 0, 0));
	return glm::normalize(point - middle);
}

glm::vec3 Player::nnH() {
	glm::vec3 point = translate(verts[2]);
	glm::vec3 middle = translate(glm::vec3(0, 0, 0));
	return (point - middle);
}

//Sprite Constructor
Sprite::Sprite() {
	speed = 0;
	velocity = glm::vec3(0, 0, 0);
	lifespan = -1;
	birthtime = 0;
	bSelected = false;
	//haveImage = true;
	name = "Agents";
	width = 100;
	height = 100;


}

//Sprite System Methods
void SpriteList::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
}

void SpriteList::add(Sprite s) {
	sprites.push_back(s);

}

void SpriteList::remove(int i) {
	if(!sprites.empty())
		sprites.erase(sprites.begin() + i);
}

void SpriteList::update() {
	if (sprites.size() == 0) return;
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
			//			cout << "deleting sprite: " << s->name << endl;
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].pos += sprites[i].velocity / ofGetFrameRate();
	}

	/*for (int i = 0; i < sprites.size(); i++) {

		sprites[i].forces = ofVec3f(ofRandom(-1000, 1000), ofRandom(-1000, 1000), 0);
		sprites[i].integrate();
	}*/
}


//Constructor for Emitter

Emitter::Emitter(SpriteList* system) {
	sys = system;
	lifespan = 3000;
	started = false;
	lastSpawned = 0;
	rate = 0;
	haveChildImage = true;
	haveImage = true;
	velocity = glm::vec3(0, 100, 0);
	drawable = true;
	width = 50;
	height = 50;
	childWidth, childHeight = 10;
}

void Emitter::draw() {
	if (drawable) {
		if (haveImage) {
			image.draw(-image.getWidth() / 2.0 + pos.x, -image.getHeight() / 2.0 + pos.y);
		}
		else {
			ofSetColor(0, 0, 200);
			ofDrawRectangle(-width / 2 + pos.x, -height / 2 + pos.y, width, height);
		}
	}
	sys->draw();
}

void Emitter::update() {
	if (!started)
		return;

	float time = ofGetElapsedTimeMillis();
	
	if ((time - lastSpawned) > (500.0 / rate)) {
		Sprite sprite;
		if (haveChildImage)
			sprite.setImage(childImage);
		sprite.velocity = velocity;
		sprite.lifespan = lifespan;
		sprite.pos = (glm::vec3(ofRandomWidth(), ofRandomHeight(), 0));
		sprite.birthtime = time;
		sprite.rotation = 1;
		sys->add(sprite);
		lastSpawned = time;
	}
	sys->update();
}

//bullet emitter
void PlayerEmitter::update() {
	if (!started)
		return;

	float time = ofGetElapsedTimeMillis();

	if ((time - lastSpawned) > (500.0 / rate)) {
		Sprite sprite;
		if (haveChildImage)
			sprite.setImage(childImage);
		sprite.haveImage = false;
		sprite.velocity = velocity;
		sprite.lifespan = lifespan;
		sprite.pos = (pos);
		sprite.birthtime = time;
		sprite.width = childWidth;
		sprite.height = childHeight;
		sys->add(sprite);
		lastSpawned = time;
		//sprite.rotation = 1;
	}
	sys->update();
}

void Explosion::update() {
	ofVec3f velocities[12] = {
		   ofVec3f(100, 0, 0),      // Right
		   ofVec3f(100, 100, 0),    // Down Right
		   ofVec3f(100, 50, 0),
		   ofVec3f(0, 100, 0),      // Down
		   ofVec3f(-100, 100, 0),   // Down Left
		   ofVec3f(-100, 50, 0),
		   ofVec3f(-100, 0, 0),     // Left
		   ofVec3f(-100, -100, 0),  // Up Left
		   ofVec3f(-100, -50, 0),
		   ofVec3f(0, -100, 0),     // Up
		   ofVec3f(100, -100, 0),
		   ofVec3f(100, -50, 0),
	};

	if (!started)
		return;
	float time = ofGetElapsedTimeMillis();
	if ((time - lastSpawned) > (500.0 / rate)) {

		for (ofVec3f dir : velocities) {
			// spawn a new sprite
			Sprite sprite;
			if (haveChildImage) sprite.setImage(childImage);
			sprite.velocity = dir;    // Use preset velocities
			sprite.lifespan = lifespan;
			sprite.pos = pos;
			sprite.birthtime = time;
			sprite.width = childWidth;
			sprite.height = childHeight;

			sys->add(sprite);
			lastSpawned = time;
		}
	}
	sys->update();

}





void Emitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();

}

void Emitter::stop() {
	started = false;
}

void Emitter::setLifespan(float life) {

	lifespan = life;
}
void Emitter::setVelocity(glm::vec3 v) {
	velocity = v;
}

void Emitter::setChildImage(ofImage img) {
	childImage = img;
	haveChildImage = true;
}

void Emitter::setImage(ofImage img) {
	image = img;
	haveImage = true;
}

void Emitter::setRate(float r) {
	rate = r;
}

void Emitter::move(Emitter* e, Player p) {
	vector<Sprite>::iterator s = e->sys->sprites.begin();

	while (s != e->sys->sprites.end()) {
		s->velocity = glm::vec3(5, 5, 0);
		s->velocity += (p.pos - s->pos);
		s++;
	}
}

void ofApp::collision(Emitter* e, glm::vec3 p, float dist) {
	vector<Sprite>::iterator s = e->sys->sprites.begin();
	vector<Sprite>::iterator temp = e->sys->sprites.begin();
	while (s != e->sys->sprites.end()) {
		ofVec3f v = s->pos - p;
		if (v.length() < dist)
		{
			explosion.play();
			cout << "collide" << endl;
			energyLevel--;
			e->sys->sprites.clear();
			impact = true;
			s = temp;
			break;
		}
		else s++;
	}
	
}

void ofApp::missileCollision() {
	float dist = bullets->childHeight  + enemyEmitter->childHeight ;
	
	for (int i = 0; i < bullets->sys->sprites.size(); i++) {
		int agentsHit = enemyEmitter->sys->removeNear(bullets->sys->sprites[i].pos, dist);
		if (agentsHit > 0) {
			//cout << agentsHit << endl;
			energyLevel += 1;
			enemyEmitter->sys->remove(i);
			impact = true;
		}
	}

	
}

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetVerticalSync(true);

	//load BG image
	bgImage.load("images/BG.png");
	ofEnableAlphaBlending();

	// Load player image
	if (!playerImage.load("images/Player.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}

	//load enemy image
	if (!enemyImage.load("images/Agents.png")) {
		cout << "Can't load image" << endl;
		ofExit();
	}
	
	//load player bullet image
	bulletImage.load("images/laser.png");
	bulletImage.resize(20, 20);

	//load sounds
	thrust.load("sound/thrust.mp3");
	explosion.load("sound/explosion.wav");
	collide.load("sound/collision.mp3");
	shooting.load("sound/laserShoot.wav");

	//create the player
	sprite = Player(glm::vec3(-50, 50, 0), glm::vec3(50, 50, 0), glm::vec3(0, -50, 0));
	sprite.setScale(1.0);
	sprite.pos = glm::vec3(ofGetWindowWidth() / 2.0 - 100, ofGetWindowHeight() / 2.0 - 100, 0);
	ofSetColor(ofColor::white);
	sprite.imageStats(&playerImage);
	sprite.rotation = 0;

	//emitter for player "particle ray" bullets
	bullets = new PlayerEmitter(new SpriteList());
	bullets->pos = sprite.pos;
	bullets->setRate(0);
	
	//emitter for explosion
	booms = new Explosion(new SpriteList());
	booms->setRate(0);
	booms->drawable = true;
	booms->haveChildImage = true;
	booms->childImage = bulletImage;
	
	//rate at which enemy spawn
	enemyRate = .5;

	//Enemy Emitter
	enemyEmitter = new Emitter(new SpriteList());
	enemyEmitter->rotation = 0.0;
	enemyEmitter->scale = ofVec3f(1, 1, 0);
	enemyEmitter->rate = enemyRate;
	enemyEmitter->pos = (ofVec3f(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2.0, 1));
	enemyEmitter->setVelocity(ofVec3f(-100, 0, 0));
	enemyEmitter->setChildImage(enemyImage);
	enemyEmitter->start();
	//enemyEmitter->integrate();
	
	
	gui.setup();
	gui.add(spdSlider.setup("Player Speed", 10, 0.000, 100));
	gui.add(rotSlider.setup("Rotation Speed", 4, 0.000, 10));
	gui.add(scaSlider.setup("Scale", 1, 0.000, 5));
	gui.add(triButton.setup("Triangle"));
	gui.add(imgButton.setup("Sprite"));
	gui.add(nAgents.setup("Agents Rate of Spawn", 0.5, 0.5, 5));
	gui.add(agentLifepsan.setup("AgentLifeSpan", 0, 0, 20000));
	gui.add(nEnergy.setup("Energy Level", 3, 3, 10));
	
	energyLevel = nEnergy;
}

//--------------------------------------------------------------
void ofApp::update() {
	sprite.setScale(scaSlider);
	sprite.setSpeed(spdSlider);
	sprite.setRotation(rotSlider);
	
	collision(enemyEmitter, glm::vec3(sprite.pos.x, sprite.pos.y, 0), 30);
	missileCollision();
	
	enemyEmitter->setLifespan(agentLifepsan);
	enemyEmitter->setRate(nAgents);
	enemyEmitter->move(enemyEmitter, sprite);
	enemyEmitter->update();

	bullets->update();
	booms->update();
	
	
	if (energyLevel == 0) {
		energyLevel--;
		collide.play();
		booms->started = true;
		booms->pos = sprite.pos;
		booms->setRate(1);
		booms->setLifespan(500);
		enemyEmitter->started = false;
		bEndScreen = true;
		
	}

	//Player Move up
	if (keymap[OF_KEY_UP]) {
		velocity = sprite.speed * sprite.heading();
		sprite.pos += velocity;
		if (sprite.pos.x > ofGetWidth() - sprite.width / 2)
			sprite.pos -= velocity;
		if (sprite.pos.x < 0 + sprite.width / 2)
			sprite.pos -= velocity;
		if (sprite.pos.y > ofGetHeight() - sprite.height / 2)
			sprite.pos -= velocity;
		if (sprite.pos.y < ofGetHeight() - ofGetHeight() + sprite.height / 2)
			sprite.pos -= velocity;
	}

	//Player rotate left
	if (keymap[OF_KEY_LEFT]) {
		sprite.rotation -= sprite.rotSpeed;

	}

	//Player move down
	if (keymap[OF_KEY_DOWN]) {
		velocity = sprite.speed * sprite.heading();
		sprite.pos -= velocity;
		if (sprite.pos.x > ofGetWidth() - sprite.width / 2)
			sprite.pos += velocity;
		if (sprite.pos.x < 0 + sprite.width / 2)
			sprite.pos += velocity;
		if (sprite.pos.y > ofGetHeight() - sprite.height / 2)
			sprite.pos += velocity;
		if (sprite.pos.y < ofGetHeight() - ofGetHeight() + sprite.height / 2)
			sprite.pos += velocity;
	}

	//Player rotate right
	if (keymap[OF_KEY_RIGHT]) {
		sprite.rotation += sprite.rotSpeed;
	}

	if (imgButton) {
		triIsPressed = false;
	}
	if (triButton) {
		triIsPressed = true;
	}

	if (bRestart == true) {
		bStartGame = false;
		bEndScreen = false;
		bRestart = false;
	}

	
}

//--------------------------------------------------------------
void ofApp::draw() {
	bgImage.resize(ofGetWidth(), ofGetHeight());
	bgImage.draw(0, 0);



	if (bStartGame == false) {
		string startMessage = "Press Spacebar to start the game, \n Use N to shoot \n Press F1 for GUI";
		ofDrawBitmapString(startMessage, glm::vec3(ofGetWindowWidth() / 2 - 130, ofGetWindowHeight() / 2 - 60, 0));
		if (bHide == true)
			gui.draw();
		energyLevel = nEnergy;
		impact = false;
	}
	else if (bEndScreen == true) {
		//booms->pos = sprite.pos;
		//booms->start();
		booms->draw();
		string endMessage = "GAME OVER. YOU RAN OUT OF ENERGY! \n Click R to restart";
		ofDrawBitmapString(endMessage, glm::vec3(ofGetWindowWidth() / 2 - 130, ofGetWindowHeight() / 2 - 60, 0));

	}
	else {
		impact = false;
		enemyEmitter->started = true;
		enemyEmitter->draw();
		bullets->started = true;
		bullets->draw();
		booms->started = false;
		//booms->pos = sprite.pos;
		//booms->draw();
		if (bHide == true)
			gui.draw();


		if (triIsPressed) {
			sprite.draw();
		}
		else {
			sprite.drawImage();
		}
	}

	//Framerate and Elapsed Time info
	string info = "FPS: " + ofToString(ofGetFrameRate());
	string time = "Elapsed Time: " + ofToString(ofGetElapsedTimef());
	string energy = "Energy Level: " + ofToString(energyLevel);
	ofDrawBitmapString(info, ofGetWidth() - 100, 40);
	ofDrawBitmapString(time, ofGetWidth() - 173, 60);
	ofDrawBitmapString(energy, ofGetWidth() - 125, 80);
}





//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	keymap[key] = true;
	if (keymap[' '])
		bStartGame = true;
	if (keymap['R'] || keymap['r'])
		bRestart = true;
	if (keymap[OF_KEY_F1])
		bHide = !bHide;
	if (keymap[OF_KEY_UP] || keymap[OF_KEY_DOWN] || keymap[OF_KEY_LEFT] || keymap[OF_KEY_RIGHT]) {
		thrust.play();
		
	}
		


	switch (key) {
	case 'F':
		ofToggleFullscreen();
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	case 'n':
		shooting.play();
		bullets->pos = sprite.pos;
		bullets->setVelocity(sprite.nnH() * 5);
		bullets->setRate(1);
		bullets->setLifespan(3500);
		bullets->update();
		bullets->setRate(0);
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	keymap[key] = false;
	
	switch (key) {
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	default:
		break;
	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	glm::vec3 currentMouse(x, y, 0);

	if (bInDrag) {
		glm::vec3 delta = currentMouse - mouseLast;
		sprite.pos += delta;
		mouseLast = currentMouse;
	}
	else if (bInRotate) {
		glm::vec3 delta = currentMouse - mouseLast;
		sprite.rotation += delta.x / 2.0;
		mouseLast = currentMouse;
	}

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	if (sprite.inside(glm::vec3(x, y, 0))) {
		if (bCtrlKeyDown) {
			bInRotate = true;
			mouseLast = glm::vec3(x, y, 0);
		}
		else {
			bInDrag = true;
			mouseLast = glm::vec3(x, y, 0);
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}