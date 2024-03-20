#include "game.h"

// Wolfenstein 3D
typedef struct {
	float x0, y0, x1, y1;
	int   orient;				// x=1,y=2
} Segment;
Segment		arrSegment[12];

typedef struct {
	float x, y;
	float dirX, dirY;
	float angle;				// in radian, between ray & positive X axis
} Ray;
Ray			mainRay;

float		col[1000];			// 1D detph map, store distance between ray & wall for each column pixel

// wall, enemy texture 
sf::Image wallImage;
const sf::Uint8*	wallTex;				
int					wallTexH, wallTexW;
sf::Image enemyImage;
const sf::Uint8* enemyTex;				
int					enemyTexH, enemyTexW;


// Enemy
typedef struct {
	float x, y;
	float size;
	Segment enemySegment;
} Enemy;
Enemy		arrEnemy[2];


// test Fill();
// in debug: 76fps
// in release: 600fps
void DrawPixel(int x, int y, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b) {
	int pos = (y * 1000 + x) * 4;
	pixels[pos++] = r;
	pixels[pos++] = g;
	pixels[pos++] = b;
	pixels[pos] = 255;
}

// test Fill();
// in debug: if update input parameter directly -> speed drop from 80fps to 43 fps
// in release: no difference, 900fps
void DrawNextPixel(int pos, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b) {
	//int pos = in_pos;
	pixels[pos++] = r;
	pixels[pos++] = g;
	pixels[pos++] = b;
	pixels[pos++] = 255;
}

void DrawNextYPixel(int in_pos, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a) {
	int pos = in_pos;
	pixels[pos] = r;
	pixels[pos+1] = g;
	pixels[pos+2] = b;
	pixels[pos+3] = a;
}

void Fill() {
	int pos = 0;
	for (int x = 0; x < 1000; x++) {
		for (int y = 0; y < 1000; y++) {
			DrawNextPixel(pos, x, 0, 0);			// let it overflow
			//DrawPixel(x,y, x, 0, 0);
			pos += 4;
		}
	}
}


// convert from world space [-5,5] -> screen space [0,999]
int ConvertX(float x) {
	int xi;
	xi = (x + 5) * 100;
	if (xi > 999) xi = 999; if (xi < 0) xi = 0;
	return xi;
}
// convert from world space [5,-5] -> screen space [0,999]
int ConvertY(float y) {
	int yi;
	yi = -100 * (y - 5);
	if (yi > 999) yi = 999; if (yi < 0) yi = 0;
	return yi;
}

bool RaySegmentIntersection(Ray ray, Segment segment, float& x, float& y) {

	float x1 = segment.x0;
	float y1 = segment.y0;
	float x2 = segment.x1;
	float y2 = segment.y1;
	float x3 = ray.x;
	float y3 = ray.y;
	float x4 = ray.x + cos(ray.angle);
	float y4 = ray.y + sin(ray.angle);

	float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

	if (den == 0) {
		return false;
	}

	float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
	float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;
	if (t > 0 && t < 1 && u > 0) {
		x = x1 + t * (x2 - x1);
		y = y1 + t * (y2 - y1);
		return true;
	}
	else {
		return false;
	}
}


float vLength(float x0, float y0, float x1, float y1) {
	float result = 0;
	result = sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
	return result;
}

// input coordinate in pixel screen space
void DrawlineDDA(int X0, int Y0, int X1, int Y1, sf::Uint8 r, sf::Uint8 g, sf::Uint8 b) {
	// calculate dx & dy
	int dx = X1 - X0;
	int dy = Y1 - Y0;

	// calculate steps required for generating pixels
	int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

	// calculate increment in x & y for each steps
	float Xinc = dx / (float)steps;
	float Yinc = dy / (float)steps;

	// Put pixel for each step
	float X = X0;
	float Y = Y0;
	for (int i = 0; i <= steps; i++)
	{
		DrawPixel(round(X), round(Y), r, g, b);  // put pixel at (X,Y)
		X += Xinc;           // increment in x at each step
		Y += Yinc;           // increment in y at each step

	}
}

void DrawlineTex(int X0, int Y0, int X1, int Y1, float u) {
	// calculate dx & dy
	int dx = X1 - X0;
	int dy = Y1 - Y0;

	// calculate steps required for generating pixels
	int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

	// calculate increment in x & y for each steps
	float Xinc = dx / (float)steps;
	float Yinc = dy / (float)steps;

	// Put pixel for each step
	float X = X0;
	float Y = Y0;
	sf::Uint8 r, g, b;
	float v;
	int ui, vi;
	ui = u * wallTexW;
	for (int i = 0; i <= steps; i++)
	{
		vi = (Y - Y0) * wallTexH / dy;
		r = wallTex[(vi * wallTexW + ui) * 4 + 0];
		g = wallTex[(vi * wallTexW + ui) * 4 + 1];
		b = wallTex[(vi * wallTexW + ui) * 4 + 2];
		DrawPixel(round(X), round(Y), r, g, b);  // put pixel at (X,Y)
		X += Xinc;           // increment in x at each step
		Y += Yinc;           // increment in y at each step

	}
}

void DrawVlineTexZoom(int X0, int Y0, int X1, int Y1, float u, float realHeight) {

	// cal scale level
	int ui, vi;
	float v;
	float scale; 
	if (realHeight > 500) {
		scale = 500.0f / realHeight;
		v = (1 - scale) * 0.5 * wallTexH; 
	}
	else {
		v = 0;
		scale = 1.0f;
	} 

	// calculate steps required for generating pixels
	int dy = Y1 - Y0;
	int steps = abs(dy);

	// Put pixel for each step
	int Y = Y0;
	sf::Uint8 r, g, b;
	ui = u * wallTexW;

	float incV = scale * wallTexH / steps;
	int pos = (Y0 * 1000 + X0) * 4;

	// current debug 27fps release 270fps
	//	- round() very slow: get 46fps->270fps
	//	- if draw Horizontal line: get 330fps (cache hit), have to rotate sprite in main.cpp
	//	- if use const color: get 430fps			(no mem access)
	//
	// pi 4
	// g++ -Ofast main.cpp game.cpp -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
	// -Ofast 26fps, x opt 8fps
	vi = 0;
	for (int i = 0; i <= steps; i++)
	{
		vi = v;
		if (vi > wallTexH) vi = wallTexH;
		vi = (vi * wallTexW + ui) * 4;
		r = wallTex[vi];
		g = wallTex[vi + 1];
		b = wallTex[vi + 2];
		DrawNextYPixel(pos, r,g,b,255);
		v += incV;
		pos += 4000;	//Y++
	}
}

void DrawEnemy(int X0, int Y0, int X1, int Y1, float u, float realHeight) {

	// cal scale level
	int ui, vi;
	float v;
	float scale;
	if (realHeight > 500) {
		scale = 500.0f / realHeight;
		v = (1 - scale) * 0.5 * enemyTexH;
	}
	else {
		v = 0;
		scale = 1.0f;
	}

	// calculate steps required for generating pixels
	int dy = Y1 - Y0;
	int steps = abs(dy);

	// Put pixel for each step
	int Y = Y0;
	sf::Uint8 r, g, b, a;
	ui = u * enemyTexW;

	float incV = scale * enemyTexH / steps;
	int pos = (Y0 * 1000 + X0) * 4;

	vi = 0;
	for (int i = 0; i <= steps; i++)
	{
		vi = v;
		if (vi > enemyTexH) vi = enemyTexH;
		vi = (vi * enemyTexW + ui) * 4;
		r = enemyTex[vi];
		g = enemyTex[vi + 1];
		b = enemyTex[vi + 2];
		a = enemyTex[vi + 3];
		if (a > 0) {
			DrawNextYPixel(pos, r, g, b, 255);
		}
		v += incV;
		pos += 4000;	//Y++
	}
}



void DrawCelingFloor() {
	int x, y;
	int ix, iy;
	float dist = 0;
	for (x = -500; x < 500; x++) {
		for (y = -499; y <= 0; y++) {
			// x,y -> ix,iy		translation
			ix = x + 500;			// [-500 499] -> (+500) ->  [0 999]
			iy = (-1) * (y - 500);	// [-499 500] -> (-500) ->  [-999 0] -> (*-1) -> [999 0]

			DrawPixel(ix, iy, abs(y) / 2, abs(y) / 2, abs(y) / 2);
			//DrawPixel(ix, iy, 120, 75, 75);
		}
	}
	for (x = -500; x < 500; x++) {
		for (y = 0; y < 500; y++) {
			// x,y -> ix,iy		translation
			ix = x + 500;			// [-500 499] -> (+500) ->  [0 999]
			iy = (-1) * (y - 500);	// [-499 500] -> (-500) ->  [-999 0] -> (*-1) -> [999 0]

			//DrawPixel(ix, iy, abs(y)/2, abs(y) / 2, abs(y) / 2);
			DrawPixel(ix, iy, 70, 70, 70);
		}
	}
}




void GameInit() {

	// wall Segment
	arrSegment[0].x0 = -8; arrSegment[0].y0 = 8; arrSegment[0].x1 = -2; arrSegment[0].y1 = 8; arrSegment[0].orient = 1;
	arrSegment[1].x0 = -2; arrSegment[1].y0 = 8; arrSegment[1].x1 = -2; arrSegment[1].y1 = 2; arrSegment[1].orient = 2;
	arrSegment[2].x0 = -2; arrSegment[2].y0 = 2; arrSegment[2].x1 = 2; arrSegment[2].y1 = 2; arrSegment[2].orient = 1;
	arrSegment[3].x0 = 2; arrSegment[3].y0 = 2; arrSegment[3].x1 = 2; arrSegment[3].y1 = 8; arrSegment[3].orient = 2;
	arrSegment[4].x0 = 2; arrSegment[4].y0 = 8; arrSegment[4].x1 = 8; arrSegment[4].y1 = 8; arrSegment[4].orient = 1;
	arrSegment[5].x0 = 8; arrSegment[5].y0 = 8; arrSegment[5].x1 = 8; arrSegment[5].y1 = -8; arrSegment[5].orient = 2;
	arrSegment[6].x0 = 8; arrSegment[6].y0 = -8; arrSegment[6].x1 = 2; arrSegment[6].y1 = -8; arrSegment[6].orient = 1;
	arrSegment[7].x0 = 2; arrSegment[7].y0 = -8; arrSegment[7].x1 = 2; arrSegment[7].y1 = -2; arrSegment[7].orient = 2;
	arrSegment[8].x0 = 2; arrSegment[8].y0 = -2; arrSegment[8].x1 = -2; arrSegment[8].y1 = -2; arrSegment[8].orient = 1;
	arrSegment[9].x0 = -2; arrSegment[9].y0 = -2; arrSegment[9].x1 = -2; arrSegment[9].y1 = -8; arrSegment[9].orient = 2;
	arrSegment[10].x0 = -2; arrSegment[10].y0 = -8; arrSegment[10].x1 = -8; arrSegment[10].y1 = -8; arrSegment[10].orient = 1;
	arrSegment[11].x0 = -8; arrSegment[11].y0 = -8; arrSegment[11].x1 = -8; arrSegment[11].y1 = 8; arrSegment[10].orient = 2;

	// Enemy
	arrEnemy[0].x = -4; arrEnemy[0].y = 1; arrEnemy[0].size = 0.4;
	arrEnemy[1].x = -2; arrEnemy[1].y = 0; arrEnemy[1].size = 0.4;

	// init main player ray
	mainRay.x = 0; mainRay.y = 0;
	mainRay.angle = 3.14159 / 2.0;		// 90, Y+

	// load wall, enemy tex
	wallImage.loadFromFile("tex2.png");
	wallTexH = wallImage.getSize().y;
	wallTexW = wallImage.getSize().x;
	wallTex = wallImage.getPixelsPtr();
	enemyImage.loadFromFile("enemy1.png");
	enemyTexH = enemyImage.getSize().y;
	enemyTexW = enemyImage.getSize().x;
	enemyTex = enemyImage.getPixelsPtr();



}

void GameClean() {


}

void GameUpdate(int frame) {

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
		mainRay.x += 0.1 * cos(mainRay.angle);
		mainRay.y += 0.1 * sin(mainRay.angle);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		mainRay.x += 0.1 * cos(mainRay.angle - 3.14159 / 2.0);
		mainRay.y += 0.1 * sin(mainRay.angle - 3.14159 / 2.0);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		mainRay.x -= 0.1 * cos(mainRay.angle);
		mainRay.y -= 0.1 * sin(mainRay.angle);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		mainRay.x -= 0.1 * cos(mainRay.angle - 3.14159 / 2.0);
		mainRay.y -= 0.1 * sin(mainRay.angle - 3.14159 / 2.0);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
		mainRay.angle += 0.05;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
		mainRay.angle -= 0.05;
	}

	

}


void GameDraw(int frame) {

	memset(pixels, 0, 1000 * 1000 * 4);

// -------------------------------------------
// 2D
// -------------------------------------------
/*
{
	// test draw top view
	for (int i = 0; i < 360; i += 3) {
		bool hit = true;
		float hitX = 0;
		float hitY = 0;
		float closestX = 0;
		float closestY = 0;
		float minDistance = 100000;
		float distance = 0;
		int x0, x1, y0, y1;
		Ray aRay;
		aRay.x = mainRay.x; aRay.y = mainRay.y;
		aRay.angle = i * (3.1415 / 180.0);

		for (int j = 0; j < 12; j++) {
			hit = RaySegmentIntersection(aRay, arrSegment[j], hitX, hitY);
			if (hit) {
				distance = vLength(aRay.x, aRay.y, hitX, hitY);
				if (distance < minDistance) {
					minDistance = distance;
					closestX = hitX;
					closestY = hitY;
				}
			}
		}
		if (minDistance < 10000) {
			x0 = ConvertX(aRay.x);
			x1 = ConvertX(closestX);
			y0 = ConvertY(aRay.y);
			y1 = ConvertY(closestY);
			DrawlineDDA(x0, y0, x1, y1, 0, 0, 255);
		}
	}

	// test draw wall segment
	for (int i = 0; i < 12; i++) {

		int x0, x1, y0, y1;
		x0 = ConvertX(arrSegment[i].x0);
		x1 = ConvertX(arrSegment[i].x1);
		y0 = ConvertY(arrSegment[i].y0);
		y1 = ConvertY(arrSegment[i].y1);

		DrawlineDDA(x0, y0, x1, y1, 255, 0, 0);
	}
}
*/

// -------------------------------------------
// 3D
// -------------------------------------------
	
{

	// compute ray & map for each col
	float fov = 3.1415 / 5.0;
	float fromAngle = mainRay.angle + fov / 2.0;
	float toAngle = mainRay.angle - fov / 2.0;
	float deltaAngle = (fromAngle - toAngle) / 1000;

	DrawCelingFloor();

	// create enemy billboard, rotate it according to the main ray
	for (int j = 0; j < 2; j++) {
		// find vec2D from the mainRay
		float rayVecX = cos(mainRay.angle);
		float rayVecY = sin(mainRay.angle);

		// find enemy left / right edge
		arrEnemy[j].enemySegment.x0 = arrEnemy[j].x - rayVecY * arrEnemy[j].size;
		arrEnemy[j].enemySegment.y0 = arrEnemy[j].y + rayVecX * arrEnemy[j].size;
		arrEnemy[j].enemySegment.x1 = arrEnemy[j].x + rayVecY * arrEnemy[j].size;
		arrEnemy[j].enemySegment.y1 = arrEnemy[j].y -rayVecX * arrEnemy[j].size;
	}


	// shoot ray to wall store distance in col[]
	for (int i = 0; i < 1000; i++) {
		bool hit = true;
		float hitX = 0;
		float hitY = 0;
		float closestX = 0;
		float closestY = 0;
		float minDistance = 100000;
		float enemyDistance = 100000;
		float distance = 0;
		int x0, x1, y0, y1;
		float dtheta = 0;
		float orient;
		float u, enemyU;

		// init ray, 1D depth map col[]
		Ray aRay;
		aRay.x = mainRay.x; aRay.y = mainRay.y;
		aRay.angle = fromAngle - i * deltaAngle;
		col[i] = minDistance;
		dtheta = abs(mainRay.angle - aRay.angle);

		// test with all the wall
		for (int j = 0; j < 12; j++) {
			hit = RaySegmentIntersection(aRay, arrSegment[j], hitX, hitY);
			if (hit) {
				distance = vLength(aRay.x, aRay.y, hitX, hitY);
				if (distance < minDistance) {
					minDistance = distance;
					closestX = hitX;
					closestY = hitY;
					orient = j % 2;
				}
			}
		}


		sf::Uint8 shade;
		float height, height2;
		float realHeight;
		
		if (minDistance < 10000) {
			minDistance = abs(minDistance) + 0.01;
			col[i] = minDistance * cos(dtheta);		// fix fisheye effect
			height = 600.0 / col[i];
			realHeight = height;
			if (height > 499) height = 499;

			// Draw solid color wall
			//int color = round(250.0 / col[i]);
			//if (color > 255) shade = 255; else shade = color;
			//DrawlineDDA(i, 500 - round(height), i, 500 + round(height), 0, shade, shade);

			// Draw texture wall
			if (orient == 0) {
				closestX = abs(closestX);
				u = closestX - floor(closestX);
			}
			else {
				closestY = abs(closestY);
				u = closestY - floor(closestY);
			}
			DrawVlineTexZoom(i, 500 - round(height), i, 500 + round(height), u, realHeight);
			//DrawlineTex(i, 500 - round(height), i, 500 + round(height), u);

			// sort enemy, this code is a special case for only 2 sprites
			float dist1 = 0;
			float dist2 = 0;
			hit = RaySegmentIntersection(aRay, arrEnemy[0].enemySegment, hitX, hitY);
			if (hit) dist1 = vLength(aRay.x, aRay.y, hitX, hitY);
			hit = RaySegmentIntersection(aRay, arrEnemy[1].enemySegment, hitX, hitY);
			if (hit) dist2 = vLength(aRay.x, aRay.y, hitX, hitY);
			if (dist1 < dist2) {
				Enemy tmpEnemy = arrEnemy[0];
				arrEnemy[0] = arrEnemy[1];
				arrEnemy[1] = tmpEnemy;
			}


			// test with all the enemy
			for (int j = 0; j < 2; j++) {
				// find tex u of the intersection point
				enemyDistance = 10000;
				hit = RaySegmentIntersection(aRay, arrEnemy[j].enemySegment, hitX, hitY);
				if (hit) {
					distance = vLength(aRay.x, aRay.y, hitX, hitY);
					if (distance < enemyDistance) {
						enemyDistance = distance;
						enemyU = 1.25*vLength(arrEnemy[j].enemySegment.x0, arrEnemy[j].enemySegment.y0, hitX, hitY);

						// Draw Enemy, if it is closer than the wall
						if (enemyDistance < minDistance) {
							enemyDistance = abs(enemyDistance) + 0.01;
							col[i] = enemyDistance * cos(dtheta);		// fix fisheye effect
							height = 600.0 / col[i];
							height2 = 400.0 / col[i];
							realHeight = height;
							if (height > 499) height = 499;
							if (height2 > 499) height2 = 499;

							DrawEnemy(i, 500 - round(height2), i, 500 + round(height), enemyU, realHeight);
						}
					}
				}
			}

		}
	}
}

}