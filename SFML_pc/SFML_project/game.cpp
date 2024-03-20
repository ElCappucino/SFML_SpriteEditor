#include "game.h"


// -------------------------------------------
// Level variable, static - visible only in this file
// -------------------------------------------

static sf::Sprite	*sSpriteArray;							// Store all unique sprite in your game
static sf::Texture	*sTexArray;								// Corresponding texture of the sprite
static GameObj		sGameObjInstArray[GAME_OBJ_INST_MAX];	// Store all game object instance
static int			sNumGameObj;
static int			sNumSprite;
static int			sNumTex;

static GameObj*		sPlayer;										// Pointer to the Player game object instance

static int			sPlayerLives;									// The number of lives left
static int			sScore;

// View
sf::View view;


// -------------------------------------------
// Game object instant functions
// -------------------------------------------

GameObj* gameObjInstCreate(int type, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient,bool mortal, 
	int life, bool anim, int numFrame, int currFrame, int frameSizeX)
{
	// loop through all object instance array to find the free slot
	for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
		GameObj* pInst = sGameObjInstArray + i;
		if (pInst->flag == FLAG_INACTIVE) {

			pInst->type = type;
			pInst->flag = FLAG_ACTIVE;
			pInst->position = pos;
			pInst->velocity = vel;
			pInst->scale = scale;
			pInst->orientation = orient;
			pInst->modelMatrix = glm::mat4(1.0f);
			pInst->mortal = mortal;
			pInst->lifespan = life;
			pInst->anim = anim;
			pInst->numFrame = numFrame;
			pInst->currFrame = currFrame;
			pInst->frameSizeX = frameSizeX;
			pInst->currOffset = currFrame * frameSizeX;
			pInst->target = -1;

			// find an asteroid target for the missile, by loop through all game object
			if (pInst->type == TYPE_MISSILE) {
				for (int j = 0; j < GAME_OBJ_INST_MAX; j++) {
					GameObj* pInst2 = sGameObjInstArray + j;
					if (pInst2->flag == FLAG_ACTIVE) {
						if (pInst2->type == TYPE_ASTEROID) {
							pInst->target = j;
							break;
						}
					}
				}
			}

			sNumGameObj++;
			return pInst;
		}
	}

	// Cannot find empty slot => return 0
	return NULL;
}

void gameObjInstDestroy(GameObj& pInst)
{
	// Lazy deletion, not really delete the object, just set it as inactive
	if (pInst.flag == FLAG_INACTIVE)
		return;

	sNumGameObj--;
	pInst.flag = FLAG_INACTIVE;
}




void GameInit() {

	// clear the Sprite array
	sNumSprite = 0;

	// clear the Texture array
	sNumTex = 0;

	// clear the game object instance array
	memset(sGameObjInstArray, 0, sizeof(GameObj) * GAME_OBJ_INST_MAX);
	sNumGameObj = 0;

	// Set the ship object instance to NULL
	sPlayer = NULL;

	// --------------------------------------------------------------------------
	// Create all of the unique sprites/textures and put them in SpriteArray/TexArray
	//		- The order of sprite should follow enum GAMEOBJ_TYPE 
	/// --------------------------------------------------------------------------


	// Create Sprite/texture
	sSpriteArray = new sf::Sprite[MESH_MAX];
	sTexArray = new sf::Texture[TEXTURE_MAX];
	
	sTexArray[sNumTex].loadFromFile("asset\\space_bg1.png");
	sSpriteArray[sNumSprite].setTexture(sTexArray[sNumTex]);
	sSpriteArray[sNumSprite].setOrigin(sTexArray[sNumTex].getSize().x / 2, sTexArray[sNumTex].getSize().y / 2);
	sNumTex++;
	sNumSprite++;

	sTexArray[sNumTex].loadFromFile("asset\\bullet.png");
	sSpriteArray[sNumSprite].setTexture(sTexArray[sNumTex]);
	sSpriteArray[sNumSprite].setOrigin(sTexArray[sNumTex].getSize().x / 2, sTexArray[sNumTex].getSize().y / 2);
	sNumTex++;
	sNumSprite++;

	sTexArray[sNumTex].loadFromFile("asset\\missile.png");
	sSpriteArray[sNumSprite].setTexture(sTexArray[sNumTex]);
	sSpriteArray[sNumSprite].setOrigin(sTexArray[sNumTex].getSize().x / 2, sTexArray[sNumTex].getSize().y / 2);
	sNumTex++;
	sNumSprite++;

	sTexArray[sNumTex].loadFromFile("asset\\ship1.png");
	sSpriteArray[sNumSprite].setTexture(sTexArray[sNumTex]);
	sSpriteArray[sNumSprite].setOrigin(sTexArray[sNumTex].getSize().x / 2, sTexArray[sNumTex].getSize().y / 2);
	sNumTex++;
	sNumSprite++;

	sTexArray[sNumTex].loadFromFile("asset\\asteroid2.png");
	sSpriteArray[sNumSprite].setTexture(sTexArray[sNumTex]);
	//sSpriteArray[sNumSprite].setOrigin(sTexArray[sNumTex].getSize().x / 2, sTexArray[sNumTex].getSize().y / 2);
	sSpriteArray[sNumSprite].setTextureRect(sf::IntRect(0, 0, 72, 70));
	sNumTex++;
	sNumSprite++;
	

	sTexArray[sNumTex].loadFromFile("asset\\explosion3.png");
	sSpriteArray[sNumSprite].setTexture(sTexArray[sNumTex]);
	//sSpriteArray[sNumSprite].setOrigin(sTexArray[sNumTex].getSize().x / 2, sTexArray[sNumTex].getSize().y / 2);
	sSpriteArray[sNumSprite].setTextureRect(sf::IntRect(0, 0, 64, 52));
	sNumTex++;
	sNumSprite++;


	// Create the background instance
	//	- Creation order is important when rendering, so we should create the background first
	gameObjInstCreate(TYPE_BACKGROUND, glm::vec3(window.getSize().x/2, window.getSize().y/2, 0.0f), 
		glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(window.getSize().x/1920.0f, window.getSize().y/1200.0f, 1.0f), 
		0, false, 0, false, 0, 0, 0);

	// Create player game object instance
	//	- the position.z should be set to 0
	//	- the scale.z should be set to 1
	//	- the velocity.z should be set to 0
	sPlayer = gameObjInstCreate(TYPE_SHIP, glm::vec3(window.getSize().x / 2, window.getSize().y / 1.2, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(50.0f/1000, 50.0f/1000, 1.0f), 
		0.0f, false, 0, false, 0, 0 ,1000);

	// Create all asteroid instance with random pos and velocity
	//	- int a = rand() % 30 + 20;							// a is in the range 20-50
	//	- float b = (float)rand()/(float)(RAND_MAX);		// b is the range 0..1
	for (int i = 0; i < NUM_ASTEROID; i++) {
		int rand_s = rand();
		gameObjInstCreate(TYPE_ASTEROID, glm::vec3(rand() % window.getSize().x, rand() % (window.getSize().y-300), 0.0f),
			glm::vec3(ASTEROID_SPEED * ((2 * (float)rand() / (float)(RAND_MAX)) - 1), ASTEROID_SPEED * ((2 * (float)rand() / (float)(RAND_MAX)) - 1), 0.0f),
			//glm::vec3(rand() % 50 + 20, rand() % 50 + 20, 1.0f), (2 * PI * (float)rand() / (float)(RAND_MAX)));
			glm::vec3((rand_s % 50 + 20) / 72.0f, (rand_s % 50 + 20)/70.0f, 1.0f), 0.0f, false, 0, true, 19, rand() % 19, 72);


	}


	
	// set view
	view.setCenter(window.getSize().x/2, window.getSize().y/2);
	view.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
	view.setRotation(0.0f);

	// reset the score and player life
	sScore = 0;
	sPlayerLives = PLAYER_INITIAL_NUM;

	std::cout << "game init" << std::endl;

}



void GameUpdate(double dt, long frame, int &state) {

	//-----------------------------------------
	// Get user input
	//-----------------------------------------
	// Moving the Player
	//	- WS accelereate/deaccelerate the ship
	//		- use accelerateion to change velocity
	//		- has velocity cap
	//	- AD turn the ship
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
		
	}
	//zoom-UO
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)) {
		view.zoom(0.99f); 
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
		view.zoom(1 / 0.99f);
	}


	//-----------------------------------------
	// Update some game obj behavior
	//	- wrap ship around the screen
	//	- destroy bullet the go out of the screen
	//-----------------------------------------

	for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
		GameObj* pInst = sGameObjInstArray + i;

		// skip inactive object
		if (pInst->flag == FLAG_INACTIVE)
			continue;

		if ((pInst->type == TYPE_SHIP) || (pInst->type == TYPE_ASTEROID)) {
			if (pInst->position.x > (window.getSize().x + (pInst->frameSizeX*pInst->scale.x)/2)) {
				pInst->position.x = -1* (pInst->frameSizeX * pInst->scale.x)/2;
			}
			if (pInst->position.x < -1 * (pInst->frameSizeX * pInst->scale.x)/2) {
				pInst->position.x = window.getSize().x;
			}
			if (pInst->position.y > (window.getSize().y + (pInst->frameSizeX * pInst->scale.x)/2)) {
				pInst->position.y = -1 * (pInst->frameSizeX * pInst->scale.x)/2;
			}
			if (pInst->position.y < -1 * (pInst->frameSizeX * pInst->scale.x)/2) {
				pInst->position.y = window.getSize().y;
			}

		}
		else if ((pInst->type == TYPE_BULLET) || (pInst->type == TYPE_MISSILE)) {
			
			//++ 
		}
	}

	//-----------------------------------------
	// Check for collsion, O(n^2)
	//-----------------------------------------

	for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
		GameObj* pInst1 = sGameObjInstArray + i;

		// skip inactive object
		if (pInst1->flag == FLAG_INACTIVE)
			continue;

		// if pInst1 is an asteroid
		if (pInst1->type == TYPE_ASTEROID) {

			// compare pInst1 with all game obj instances 
			for (int j = 0; j < GAME_OBJ_INST_MAX; j++) {
				GameObj* pInst2 = sGameObjInstArray + j;

				//++
			}
		}
	}


	double fps = 1.0 / dt;
	//printf("Level1: Update @> %f fps, frame>%ld\n", fps, frame);
	//printf("Score> %i\n", sScore);
	//printf("num obj> %i\n", sNumGameObj);

}


void GameDraw(double dt) {

	window.setView(view);

	// clear framebuffer
	window.clear(sf::Color::Black);

	// draw all game object instance in the sGameObjInstArray
	for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
		GameObj* pInst = sGameObjInstArray + i;

		// skip inactive object
		if (pInst->flag == FLAG_INACTIVE)
			continue;

		if (pInst->numFrame > 0) {
			int w = sSpriteArray[pInst->type].getTextureRect().width;
			int h = sSpriteArray[pInst->type].getTextureRect().height;
			sSpriteArray[pInst->type].setTextureRect(sf::IntRect(pInst->currOffset, 0, w,h));
		}

		sSpriteArray[pInst->type].setPosition(sf::Vector2f(pInst->position.x, pInst->position.y));
		sSpriteArray[pInst->type].setScale(sf::Vector2f(pInst->scale.x, pInst->scale.y));
		sSpriteArray[pInst->type].setRotation(glm::degrees(pInst->orientation));
		
		window.draw(sSpriteArray[pInst->type]);
		
		
	}


	// swap framebuffer
	window.display();


}


void GameClean() {

	// call gameObjInstDestroy for all object instances in the sGameObjInstArray
	for (int i = 0; i < GAME_OBJ_INST_MAX; i++) {
		gameObjInstDestroy(sGameObjInstArray[i]);
	}

	delete[] sTexArray;
	delete[] sSpriteArray;

	std::cout << "game clean" << std::endl;
	

}