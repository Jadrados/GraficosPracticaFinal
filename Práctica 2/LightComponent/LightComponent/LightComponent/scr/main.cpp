#include "BOX.h"
#include "mushroom_04.h"
#include <IGL/IGlib.h>
#include "../CAMERA.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


//Idenficadores de los objetos de la escena
int objId =-1;

//Declaraci�n de CB
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);
void updateView();

Camera camera(glm::vec3(0, 0, 6), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));


void updateView() {
	glm::mat4 view = camera.getViewMatrix(); //se reconstruye view de cero
	IGlib::setViewMat(view);
}


int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)
	if (!IGlib::init("../shaders_P1/shader.v0.vert", "../shaders_P1/shader.v0.frag"))
		return -1;
 

	//Creamos el objeto que vamos a visualizar
	objId = IGlib::createObj(mushroomNTriangleIndex, mushroomNVertex, mushroomTriangleIndex, 
			mushroomVertexPos, mushroomVertexColor, mushroomVertexNormal, mushroomVertexTexCoord, mushroomVertexTangent);
		
	glm::mat4 modelMat = glm::mat4(1.0f);
	IGlib::setModelMat(objId, modelMat);
	//Incluir texturas aqu�.
	IGlib::addColorTex(objId, "../img/color.png");
	
	glm::mat4 view = glm::mat4(1.f);
	view[3].z = -6.f;

	float n = 1.;
	float f = 10.;
	float inv_t30 = 1.73205080757;

	glm::mat4 proj = glm::mat4(0.f);
	proj[0].x = inv_t30;
	proj[1].y = inv_t30;
	proj[2].z = (n + f) / (n - f);
	proj[3].z = 2. * f * n / (n - f);
	proj[2][3] = -1;

	IGlib::setProjMat(proj);
	updateView();


  //CBs
  IGlib::setResizeCB(resizeFunc);
  IGlib::setIdleCB(idleFunc);
  IGlib::setKeyboardCB(keyboardFunc);
  IGlib::setMouseCB(mouseFunc);
  IGlib::setMouseMoveCB(mouseMotionFunc);

	
	//Mainloop
	IGlib::mainLoop();
	IGlib::destroy();
	return 0;
}

void resizeFunc(int width, int height)
{
	//Ajusta el aspect ratio al tama�o de la venta
}

void idleFunc()
{
	static float angle = 0.f;
	angle = (angle > 2.f * 3.141599) ? 0.f : angle + 0.002;

	glm::mat4 model(1.f);

	model = glm::rotate(model, angle, glm::vec3(1.f));

	IGlib::setModelMat(objId, model);
}

void keyboardFunc(unsigned char key, int x, int y)
{
	float speed = 0.2f;
	float turnSpeed = 5.0f;

	switch (key) {
	case 'w':
		camera.moveForward(speed);
		break;
	case 's':
		camera.moveBack(speed);
		break;
	case 'd':
		camera.moveRight(speed);
		break;
	case 'a':
		camera.moveLeft(speed);
		break;

	case 'q':
		camera.yaw(-turnSpeed);
		break;   // Girar izquierda

	case 'e':
		camera.yaw(turnSpeed);
		break;    // Girar derecha

	case 'r':
		camera.pitch(turnSpeed);
		break;  // Mirar arriba

	case 'f':
		camera.pitch(-turnSpeed);
		break; // Mirar abajo
	}

	updateView();
}


void mouseFunc(int button, int state, int x, int y)
{
	if (state==0)
		std::cout << "Se ha pulsado el bot�n ";
	else
		std::cout << "Se ha soltado el bot�n ";
	
	if (button == 0) std::cout << "de la izquierda del rat�n " << std::endl;
	if (button == 1) std::cout << "central del rat�n " << std::endl;
	if (button == 2) std::cout << "de la derecha del rat�n " << std::endl;

	std::cout << "en la posici�n " << x << " " << y << std::endl << std::endl;
}

void mouseMotionFunc(int x, int y)
{
}
