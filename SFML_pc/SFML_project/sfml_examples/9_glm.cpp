#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>

sf::RenderWindow window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

int main()
{
    // create window
    window.create(sf::VideoMode(1000, 1000), "My window");
    //window.setVerticalSyncEnabled(true);    // limit fps to 65fps
    window.setFramerateLimit(60);

	//vector
	glm::vec3 vecA = glm::vec3(10.0f, 0.0f, 0.0f);
	printf("vecA x>%f y>%f z>%f\n", vecA.x, vecA.y, vecA.z);
	glm::vec3 vecB = glm::vec3(0.0f, 1.0f, 0.0f);
	printf("vecB x>%f y>%f z>%f\n", vecB.x, vecB.y, vecB.z);
	glm::vec3 vecC = glm::cross(vecA, vecB);
	printf("Cross product between vecA and vecB = (%f,%f,%f)\n", vecC.x, vecC.y, vecC.z);
	float result = glm::dot(vecA, vecB);
	printf("Dot product between vecA and vecB = %f\n", result);
	result = glm::length(vecA);
	printf("Length of vecA = %f\n", result);

	//matrix
	glm::mat3 matA = glm::mat3(1.0f);
	printf("Matrix A elements>\n");
	printf("%f %f %f\n", matA[0][0], matA[1][0], matA[2][0]);
	printf("%f %f %f\n", matA[0][1], matA[1][1], matA[2][1]);
	printf("%f %f %f\n", matA[0][2], matA[1][2], matA[2][2]);
	glm::mat4 matB = glm::mat4(1.0f);
	matB = glm::translate(matB, glm::vec3(10.0f, 20.0f, 30.0f));
	printf("Matrix B elements>\n");
	printf("%f %f %f %f\n", matB[0][0], matB[1][0], matB[2][0], matB[3][0]);
	printf("%f %f %f %f\n", matB[0][1], matB[1][1], matB[2][1], matB[3][1]);
	printf("%f %f %f %f\n", matB[0][2], matB[1][2], matB[2][2], matB[3][2]);
	printf("%f %f %f %f\n", matB[0][3], matB[1][3], matB[2][3], matB[3][3]);
	glm::vec4 vecD(1.0f, 2.0f, 3.0f, 4.0f);
	glm::vec4 vecE = matB * vecD;
	printf("vecE = %f,%f,%f,%f\n", vecE.x, vecE.y, vecE.z, vecE.w);

    sf::Clock clock1;
    while (window.isOpen())
    {
        // handle event
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // draw framebuffer
        window.clear(sf::Color::Black);

        window.display();
    }



    return 0;
}