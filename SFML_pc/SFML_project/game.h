#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstring>
#include <math.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>


extern sf::RenderWindow window;

// -------------------------------------------
// Defines
// -------------------------------------------
#define PI							3.1415926
#define MESH_MAX					32				// The total number of Mesh (Shape)
#define TEXTURE_MAX					32				// The total number of texture
#define GAME_OBJ_INST_MAX			1024			// The total number of different game object instances
#define PLAYER_INITIAL_NUM			3				// initial number of ship lives
#define NUM_ASTEROID				30
#define SHIP_ACC_FWD				-150.0f			// ship forward acceleration (in m/s^2)
#define SHIP_ACC_BWD				180.0f			// ship backward acceleration (in m/s^2)
#define SHIP_ROT_SPEED				(2.0f * PI)		// ship rotation speed (degree/second)
#define HOMING_MISSILE_ROT_SPEED	(PI / 2.0f)		// homing missile rotation speed (degree/second)
#define BULLET_SPEED				-300.0f			
#define ASTEROID_SPEED				200.0f	
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
	int				frameSizeX;			// frame size, assume single row sprite sheet
	int				currOffset;
};



// functions to create/destroy a game object instance
static GameObj* gameObjInstCreate(int type, glm::vec3 pos, glm::vec3 vel, glm::vec3 scale, float orient,
								bool mortal, int life, bool anim, int numFrame, int currFrame, int offsetX);
static void		gameObjInstDestroy(GameObj& pInst);



void GameInit();
void GameUpdate(double dt, long frame, int &state);
void GameDraw(double dt);
void GameClean();
