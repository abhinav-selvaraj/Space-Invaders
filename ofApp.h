#include "ofMain.h"
#include "ofxGui.h"

class Shape {
public:
	glm::vec3 sizeChanger = glm::vec3(1, 1, 1);
	Shape() {}
	virtual void draw() {}
	virtual bool inside(glm::vec3 p) {
		return true;
	}
	glm::mat4 getMatrix() {
		glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(pos));
		glm::mat4 rot = glm::rotate(trans, glm::radians(rotation), glm::vec3(0, 0, 1));
		glm::mat4 scale = glm::scale(rot, sizeChanger);
		return (scale);
	}
	glm::vec3 pos;
	float rotation = 0.0;    // degrees 
	glm::vec3 scale = glm::vec3(1, 1, 1);

	vector<glm::vec3> verts;
};

class TriangleShape : public Shape {
public:
	TriangleShape() {}
	TriangleShape(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		verts.push_back(p1);
		verts.push_back(p2);
		verts.push_back(p3);
	}
	void draw();
	bool inside(glm::vec3 p) {
		// convert point to "object space"
		//
		glm::vec3 p0 = glm::inverse(getMatrix()) * glm::vec4(p, 1);
		return (inside(p0, verts[0], verts[1], verts[2]));
	}

	bool inside(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		glm::vec3 v1 = glm::normalize(p1 - p);
		glm::vec3 v2 = glm::normalize(p2 - p);
		glm::vec3 v3 = glm::normalize(p3 - p);
		float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, 1));
		float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, 1));
		float a3 = glm::orientedAngle(v3, v1, glm::vec3(0, 0, 1));
		if (a1 < 0 && a2 < 0 && a3 < 0) return true;
		else return false;
	}
	//------
	void sizeChange(float x, float y) {
		sizeChanger.x = x;
		sizeChanger.y = y;
	}
};

class Player : public TriangleShape {
public:
	float width;
	float height;
	float speed;
	float rotSpeed;
	ofImage* picture;
	glm::vec3 nnH();

	Player() {
	}
	Player(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) : TriangleShape(p1, p2, p3) {
	}

	glm::vec3 translate(glm::vec3 trans) {
		return glm::vec3(getMatrix() * glm::vec4(trans, 1));
	}

	void setScale(float scale) {
		sizeChanger = glm::vec3(scale, scale, scale);
	}

	void setSpeed(float spd) {
		speed = spd;
	}

	void setRotation(float rotSpd) {
		rotSpeed = rotSpd;
	}

	void imageStats(ofImage* image) {
		picture = image;
		width = picture->getWidth();
		height = picture->getHeight();
	}

	void drawImage() {
		float spriteWidth = width * sizeChanger.x;
		float spriteHeight = height * sizeChanger.x;

		ofPushMatrix();
		ofTranslate(translate(glm::vec3(0, 0, 0)));
		ofRotateZ(rotation);
		ofSetColor(ofColor::white);
		picture->draw(-spriteWidth / 2, -spriteHeight / 2, spriteWidth, spriteHeight);
		ofPopMatrix();
	}
	glm::vec3 heading();
	void drawHeading();
};

//Sprite Class
class Sprite : public TriangleShape {
public:
	Sprite();
	void draw() {
		ofSetColor(255, 255, 255);
		if (haveImage) {
			spriteImage.draw(pos.x, pos.y);
		}
		else {
			/*float x = -width / 2.0 + pos.x;
			float y = -height / 2.0 + pos.y;

			ofSetColor(255, 0, 0);
			ofDrawRectangle(-width / 2.0 + pos.x, -height / 2.0 + pos.y, width, height);*/
			ofSetColor(255, 0, 0);
			ofDrawRectangle(pos.x, pos.y, 20, 20);
		}

	}
	float age() {
		return (ofGetElapsedTimeMillis() - birthtime);
	}

	void setImage(ofImage img) {
		spriteImage = img;
		bShowImage = true;
		width = img.getWidth();
		height = img.getHeight();
	}

	//bool inside(const glm::vec3 p);

	void setSelected(bool state) { bSelected = state; }
	void setHighlight(bool state) { bHighlight = state; }
	bool isSelected() { return bSelected; }
	bool isHighlight() { return bHighlight; }


	glm::vec3 heading() {
		return (glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1)) * glm::vec4(0, 1, 0, 1));
	}

	bool bHighlight = false;
	bool bSelected = false;
	bool bShowImage = false;

	glm::vec3 velocity = glm::vec3(0, 0, 0);
	float birthtime = 0; // elapsed time in ms
	float lifespan = -1;  //  time in ms
	string name = "UnammedSprite";
	float width = 40;
	float height = 40;
	float speed;
	bool haveImage = true;
	ofImage spriteImage;
	//spriteImage.loadImage("image/Agents.png");
	float radius;

	ofVec3f velocity1 = ofVec3f(0, 0, 0);
	ofVec3f acceleration = ofVec3f(0, 0, 0);
	ofVec3f forces = ofVec3f(0, 0, 0);

	float angvelocity = 0;
	float angacceleration = 0;
	float angforces = 0;
	float mass = 1.0;
	float dt;
	float damping = 1.0;


	void integrate() {
		float dt = 1 / 60;
		pos += velocity1 * dt;
		ofVec3f a = acceleration;
		a += forces;
		velocity1 += a * dt;
		velocity1 *= damping;
		forces.set(0, 0, 0);
	}
};

//Sprite System
class SpriteList {
public:
	void add(Sprite);
	void remove(int);
	void update();
	
	void draw();
	int removeNear(ofVec3f point, float dist) {
		vector<Sprite>::iterator s = sprites.begin();
		vector<Sprite>::iterator tmp;
		int count = 0;
		while (s != sprites.end()) {
			ofVec3f v = s->pos - point;
			if (v.length() < dist) {
				cout << "removing sprite" << endl;
				//sprites[count].lifespan = 0;
				tmp = sprites.erase(s);
				count+=10;
				s = tmp;
			}
			else s++;
		}
		return count;
	}
	vector<Sprite> sprites;

};

//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class Emitter : public Shape {
public:
	Emitter(SpriteList*);
	//void init();
	void draw();
	void start();
	void stop();
	void setLifespan(float);
	void setVelocity(const glm::vec3 v);
	void setChildImage(ofImage);
	void setImage(ofImage);
	void setRate(float);
	void update();
	void move(Emitter* e, Player p);
	
	void integrate() {
		dt = (1.0 / 60.0);
		pos += velocity1* dt;
		ofVec3f accel = acceleration;
		accel += forces;
		forces.set(0, 0, 0);
	}

	// virtuals - can overloaded
	//virtual void moveSprite(Sprite*);
	//virtual void spawnSprite();

	SpriteList* sys;
	float rate;
	glm::vec3 velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	ofImage childImage;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool haveImage;
	float width, height;
	float childWidth, childHeight;
	glm::vec3 pos;


	ofVec3f velocity1 = ofVec3f(0, 0, 0);
	ofVec3f acceleration = ofVec3f(0, 0, 0);
	ofVec3f forces = ofVec3f(0, 0, 0);

	float angvelocity = 0;
	float angacceleration = 0;
	float angforces = 0;
	float mass = 1.0;
	float dt;
	float damping = 1.0;

};

// Emits bullets from Player
class PlayerEmitter : public Emitter {
public:
	
	PlayerEmitter(SpriteList* spriteSys) : Emitter(spriteSys) {};

	void update();
};


class Explosion : public Emitter {
public:
	Explosion(SpriteList* spriteSys) : Emitter(spriteSys) {};
	void update();
		
};



class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void collision(Emitter* e, glm::vec3 p, float dist);
	void missileCollision();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	TriangleShape tri;
	Player sprite;

	ofImage playerImage;
	ofImage enemyImage;
	ofImage bgImage;
	ofImage bulletImage;

	ofSoundPlayer thrust;
	ofSoundPlayer collide;
	ofSoundPlayer explosion;
	ofSoundPlayer shooting;

	//GUI
	ofxPanel gui;
	ofxFloatSlider spdSlider, scaSlider, rotSlider;
	ofxFloatSlider nAgents, agentLifepsan;
	ofxIntSlider nEnergy;
	ofxButton imgButton, triButton;
	ofxToggle headToggle;

	// state variables
	//
	bool bInDrag = false;
	bool bInRotate = false;
	bool bCtrlKeyDown = false;
	bool bStartGame = false;
	bool bEndScreen = false;
	bool bRestart = false;
	bool bHide = false;
	bool impact = false;
	glm::vec3 mouseLast;
	int energyLevel = 3;

	Emitter* enemyEmitter;
	PlayerEmitter* bullets;
	Explosion* booms;
	
	
	
	
	float enemyRate;


};