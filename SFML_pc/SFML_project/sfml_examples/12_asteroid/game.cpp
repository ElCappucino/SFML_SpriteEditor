#include "game.h"

// -------------------------------------------
// Defines
// -------------------------------------------

#define MESH_MAX					32				// The total number of Mesh (Shape)
#define TEXTURE_MAX					32				// The total number of texture
#define GAME_OBJ_INST_MAX			1024			// The total number of different game object instances
#define PLAYER_INITIAL_NUM			3				// initial number of ship lives
#define NUM_ASTEROID				20
#define SHIP_ACC_FWD				150.0f			// ship forward acceleration (in m/s^2)
#define SHIP_ACC_BWD				-180.0f			// ship backward acceleration (in m/s^2)
#define SHIP_ROT_SPEED				(2.0f * PI)		// ship rotation speed (degree/second)
#define HOMING_MISSILE_ROT_SPEED	(PI / 2.0f)		// homing missile rotation speed (degree/second)
#define BULLET_SPEED				300.0f			
#define ASTEROID_SPEED				100.0f	
#define MAX_SHIP_VELOCITY			200.0f

enum GAMEOBJ_TYPE
{
	// list of game object types
	TYPE_BACKGROUND = 0,
	TYPE_BULLET,
	TYPE_MISSILE,
	TYPE_SHIP,
	TYPE_ASTEROID,
	TYPE_EXPLOSION

};

#define FLAG_INACTIVE		0
#define FLAG_ACTIVE			1

// -------------------------------------------
// Structure definitions
// -------------------------------------------

struct GameObj
{
	int				type;				// enum type
	int				flag;				// 0 - inactive, 1 - active
	glm::vec3		position;			// usually we will use only x and y
	glm::vec3		velocity;			// usually we will use only x and y
	glm::vec3		scale;				// usually we will use only x and y
	float			orientation;		// 0 radians is 3 o'clock, PI/2 radian is 12 o'clock
	glm::mat4		modelMatrix;
	int				target;				// only for Missile game obj, index to sGameObjInstArray
	//
	bool			mortal;
	int				lifespan;			// in frame unit
	bool			anim;				// has animation?
	int				numFrame;			// #frame in texture animation
	int				currFrame;
	int				offsetX;			// assume single row sprite sheet
	int				offsetY;			// gameObjInstreate() will auto compute offsetX for you

};


// -------------------------------------------
// Level variable, static - visible only in this file
// -------------------------------------------

static sf::Sprite	*sSpriteArray;							// Store all unique sprite in your game
static int			sNumSprite;
static sf::Texture	*sTexArray;							// Corresponding texture of the sprite
static int			sNumTex;
static GameObj		sGameObjInstArray[GAME_OBJ_INST_MAX];			// Store all game object instance
static int			sNumGameObj;

static GameObj*		sPlayer;										// Pointer to the Player game object instance

static int			sPlayerLives;									// The number of lives left
static int			sScore;


// functions to create/destroy a game object instance
static GameObj* gameObjInstCreate(int type, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient,
	bool mortal, int life, bool anim, int numFrame, int currFrame, int offsetX);
static void			gameObjInstDestroy(GameObj& pInst);




// View
sf::View view;



// -------------------------------------------
// Game object instant functions
// -------------------------------------------

GameObj* gameObjInstCreate(int type, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient,bool mortal, 
	int life, bool anim, int numFrame, int currFrame, int offsetX)
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
			pInst->offsetX = offsetX;
			pInst->offsetY = 0;
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
		0.0f, false, 0, false, 0, 0 ,0);

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



void GameUpdate(double dt, long frame) {

	//-----------------------------------------
	// Get user input
	//-----------------------------------------
	// Move Player sprite WSAD
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
		//marioSprite.move(sf::Vector2f(0.f, -10.f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
	
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
	// Update animation
	//-----------------------------------------
	//if ((frame % 1) == 0) {			// do once for each n frames, to slow the animation speed
	//	explodeX += 100;			// move 1 frame to the right
	//	if (explodeX > 801) {		// if reaching the end of this row
	//		explodeX = 0;			// set x to left most
	//		explodeY += 100;		// set to the row below
	//		if (explodeY > 801) {	// if reaching the last row
	//			explodeY = 0;		// go back to the top row
	//		}
	//	}
	//}
	//explosionSprite.setTextureRect(sf::IntRect(explodeX, explodeY, 100, 100));


	double fps = 1.0 / dt;
	/*printf("Level1: Update @> %f fps, frame>%ld\n", fps, frame);
	printf("Life> %i\n", sPlayerLives);
	printf("Score> %i\n", sScore);
	printf("num obj> %i\n", sNumGameObj);*/

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
			sSpriteArray[pInst->type].setTextureRect(sf::IntRect(pInst->offsetX, 0, w,h));
		}

		sSpriteArray[pInst->type].setPosition(sf::Vector2f(pInst->position.x, pInst->position.y));
		sSpriteArray[pInst->type].setScale(sf::Vector2f(pInst->scale.x, pInst->scale.y));
		sSpriteArray[pInst->type].setRotation(pInst->orientation);
		
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