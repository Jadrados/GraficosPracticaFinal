#include "BOX.h"
#include <IGL/IGlib.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>


//Idenficadores de los objetos de la escena
int objId =-1;
int obj2 = -1;
int obj3 = -1;

//Declaraci�n de CB
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);

//Variables globales (2)
glm::vec3 CoP = glm::vec3 (0, 0, 6);
glm::vec3 lookAt = glm::vec3 (0, 0, -1);
glm::vec3 up = glm::vec3 (0, 1, 0);

//Tarea 3
int loadModel(const std::string& path)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_Triangulate |        
		aiProcess_GenNormals |        
		aiProcess_JoinIdenticalVertices
	);

	aiMesh* mesh = scene->mMeshes[0];  

	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<unsigned int> indices;

	// --- VÉRTICES ---
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// Posiciones
		positions.push_back(mesh->mVertices[i].x);
		positions.push_back(mesh->mVertices[i].y);
		positions.push_back(mesh->mVertices[i].z);

		// Normales
		if (mesh->HasNormals())
		{
			normals.push_back(mesh->mNormals[i].x);
			normals.push_back(mesh->mNormals[i].y);
			normals.push_back(mesh->mNormals[i].z);
		}
		else
		{
			normals.push_back(0.0f);
			normals.push_back(1.0f);
			normals.push_back(0.0f);
		}
	}

	// --- ÍNDICES ---
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	// --- CREAR OBJETO IGLIB ---
	int id = IGlib::createObj(
		indices.size(),           
		mesh->mNumVertices,        
		indices.data(),            
		positions.data(),          
		nullptr,                   // colores
		normals.data(),           
		nullptr,                   // coords textura 
		nullptr                    // tangentes 
	);

	return id;
}


int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)
	if (!IGlib::init("../shaders_P1/shader.v5.vert", "../shaders_P1/shader.v5.frag"))
		return -1;
   
	//Se ajusta la c�mara
	//Si no se da valor se cojen valores por defecto
	//IGlib::setProjMat(const glm::mat4 &projMat);
	//IGlib::setViewMat(const glm::mat4 &viewMat);

	//Creamos el objeto que vamos a visualizar
	objId = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex, 
			cubeVertexPos, cubeVertexColor, cubeVertexNormal,cubeVertexTexCoord, cubeVertexTangent);
	
	glm::mat4 modelMat = glm::mat4(1.0f);
	IGlib::setModelMat(objId, modelMat);
	//Incluir texturas aqu�.
	IGlib::addColorTex(objId, "../img/color.png");

	//Tarea 4: Crear otro objeto
	obj2 = IGlib::createObj(cubeNTriangleIndex, cubeNVertex, cubeTriangleIndex,
		cubeVertexPos, cubeVertexColor, cubeVertexNormal, cubeVertexTexCoord, cubeVertexTangent);

	glm::mat4 model2 = glm::mat4(1.0f);
	model2[3].x = 5.0f; //Moverlo
	IGlib::setModelMat(obj2, model2);
	//Incluir texturas aqu�.
	IGlib::addColorTex(obj2, "../img/color.png");
	
	//Tarea 3
	obj3 = loadModel("../img/spot/spot_control_mesh.obj");

	glm::mat4 model3 = glm::mat4(1.0f);
	model3[3].x = -5.0f; //Moverlo
	model3 = glm::rotate(model3, glm::radians(180.0f), glm::vec3(0, 1, 0));	
	IGlib::setModelMat(obj3, model3);
	IGlib::addColorTex(obj3, "../img/spot/spot_texture.png");

	//Matriz parte obligatoria
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
	IGlib::setViewMat(view);


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
{	//Tarea 1
	//Ajusta el aspect ratio al tama�o de la ventana
	float aspect = float(width) / float(height);

	//Con apertura vertical de 60º
	float fovy = 60.0f * 3.1415f / 180.0f;
	float n = 1;
	float f = 10;
	float t = tan(fovy / 2.f) * n;
	float b = -t;
	float r = t * aspect;
	float l = -r;

	//Creamos la matriz
	glm::mat4 proj(0.0f);

	proj[0][0] = 2 * n / (r - l);
	proj[0][2] = (r + l) / (r - l);

	proj[1][1] = 2 * n / (t - b);
	proj[1][2] = (t + b) / (t - b);

	proj[2][2] = -(f + n) / (f - n);
	proj[2][3] = -(2 * f * n) / (f - n);

	proj[3][2] = -1.0f;

	IGlib::setProjMat(proj);

}

void idleFunc()
{
	static float angle = 0.f;
	angle = (angle > 2.f * 3.141599) ? 0.f : angle + 0.001;

	glm::mat4 model(1.f);

	model = glm::rotate(model, angle, glm::vec3(1.f));

	IGlib::setModelMat(objId, model);

	//Tarea 4
	glm::mat4 model3(1.f);

	model3[3].x = -5.f;

	model3 = glm::rotate(model3, -angle, glm::vec3(0, 1, 0));

	IGlib::setModelMat(obj3, model3);
}

void keyboardFunc(unsigned char key, int x, int y)
{   //Tarea 2
	//rotación
	float angle = 0.05f;

	if (key == 'q') { //Poner mayus también
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angle, up);
		lookAt = glm::vec3(rot * glm::vec4(lookAt, 0.f));
	}

	if (key == 'e') {
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), -angle, up);
		lookAt = glm::vec3(rot * glm::vec4(lookAt, 0.f));
	}
	//Movimiento
	glm::vec3 forward = glm::normalize(lookAt);
	glm::vec3 right = glm::normalize(glm::cross(forward, up));
	glm::vec3 u = glm::cross(right, forward);
	float speed = 0.1f;

	if (key == 'w')
		CoP += forward * speed;
	if (key == 's')
		CoP -= forward * speed;
	if (key == 'd')
		CoP += right * speed;
	if (key == 'a')
		CoP -= right * speed;
	
	//Matriz vista
	glm::mat4 view(1.0f);

	view[0][0] = right.x;
	view[1][0] = right.y;
	view[2][0] = right.z;
	view[3][0] = -glm::dot(right, CoP);

	view[0][1] = u.x;
	view[1][1] = u.y;
	view[2][1] = u.z;
	view[3][1] = -glm::dot(u, CoP);

	view[0][2] = -forward.x;
	view[1][2] = -forward.y;
	view[2][2] = -forward.z;
	view[3][2] = glm::dot(forward, CoP);

	view[0][3] = 0;
	view[1][3] = 0;
	view[2][3] = 0;
	view[3][3] = 1;

	IGlib::setViewMat(view);
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
