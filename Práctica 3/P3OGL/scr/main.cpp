#include "BOX.h"
#include "auxiliar.h"


#include <gl/glew.h>
#define SOLVE_FGLUT_WARNING
#include <gl/freeglut.h> 

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>


//////////////////////////////////////////////////////////////
// Datos que se almacenan en la memoria de la CPU
//////////////////////////////////////////////////////////////

//Matrices
glm::mat4	proj = glm::mat4(1.0f);
glm::mat4	view = glm::mat4(1.0f);
glm::mat4	model = glm::mat4(1.0f);


//////////////////////////////////////////////////////////////
// Variables que nos dan acceso a Objetos OpenGL
//////////////////////////////////////////////////////////////

unsigned int vshader; //guarda vertex shader
unsigned int fshader; //guarda fragment shader
unsigned int program; //guarda el programa enlazado

// Variables Uniform
//Uniform: tiene la misma información para todos los vértices. Ej. matrices
int uModelViewMat;
int uModelViewProjMat;
int uNormalMat;
int uColorTex; //uniform sampler2D colorTex;
int uEmiTex; //uniform sampler2D emiTex;
//Sampler 2D es una referencia a una textura GPU

// Atributos
//Attribute: tiene información distinta por vértice: Ej. posivción, color, normal
int inPos;
int inColor;
int inNormal;
int inTexCoord;

// VAO - configuración, guarda cómo se usan los VBOs
unsigned int vao;

// VBOs que forman parte del objeto - es memoria en la GPU, guarda datos como posiciones, colores, normales y UVs
unsigned int posVBO;
unsigned int colorVBO;
unsigned int normalVBO;
unsigned int texCoordVBO; //UVs
unsigned int triangleIndexVBO; //Índices

//Texturas
unsigned int colorTexId; //color principal
unsigned int emiTexId; //partes que son brillantes


//////////////////////////////////////////////////////////////
// Funciones auxiliares
//////////////////////////////////////////////////////////////
//!!Por implementar

//Declaraci�n de CB
void renderFunc();
void resizeFunc(int width, int height);
void idleFunc();
void keyboardFunc(unsigned char key, int x, int y);
void mouseFunc(int button, int state, int x, int y);

//Funciones de inicializaci�n y destrucci�n
void initContext(int argc, char** argv);
void initOGL();
void initShader(const char *vname, const char *fname);
void initObj();
void destroy();


//Carga el shader indicado, devuele el ID del shader
//!Por implementar
GLuint loadShader(const char *fileName, GLenum type);

//Crea una textura, la configura, la sube a OpenGL, 
//y devuelve el identificador de la textura 
//!!Por implementar
unsigned int loadTex(const char *fileName);


int main(int argc, char** argv)
{
	std::locale::global(std::locale("spanish"));// acentos ;)

	initContext(argc, argv);
	initOGL();
	initShader("../shaders_P3/shader.v1.vert", "../shaders_P3/shader.v1.frag"); //Antes el color venía del VBO de colores y ahora el color va avenir de una imagen
	initObj();

	glutMainLoop(); //Bucle de eventos

	destroy();

	return 0;
}
	
//////////////////////////
// Funciones auxiliares //
/////////////////////////
void initContext(int argc, char** argv){
	glutInit(&argc, argv); //Inicializa GLTU

	glutInitContextVersion(3, 3); //Pide OpenGL 3.3.
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//GLUT_DOUBLE, OpenGL dibuja un buffer oculto y luego lo intercambia
	//GLTU_RGBA, así la venta tendrá color RGBA
	//GLUT_DEPTH activa el buffer de profundidad y luego servirá para el 3D
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(500, 500);

	glutInitWindowPosition(0, 0);

	//Comprueba si OpenGL moderno está disponible y carga las extensiones necesarias
	glutCreateWindow("Practicas OGL");

	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}

	//El siguiente bloque de código imprimirá algo como: This system supports OpenGL Version: 4.6.0 ...
	const GLubyte* oglVersion = glGetString(GL_VERSION);

	std::cout << "This system supports OpenGL Version: "
		<< oglVersion << std::endl;

	//Registrr los callBacks (cuando ocuarra esto, se llama a esta función)
	glutReshapeFunc(resizeFunc);
	glutDisplayFunc(renderFunc);
	glutIdleFunc(idleFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
}
void initOGL(){
	//ACTIVAR DEPTH TEST
	//Muy importante en 3D, sin esto OpenGL no sabe qué objeto está delante
	//sin esto OpenGl no sabe qué objeto está delante,
	//Pero con la siguiente línea de código OpenGL usa z-buffer
	glEnable(GL_DEPTH_TEST);

	//COLOR DE FONDO
	//Define el color de limpieza de pantalla (R G B A)
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f); //gris oscuro

	//RASTERIZADO Y CULLING
	//Define qué caras son frontales
	glFrontFace(GL_CCW); //Si los vértices están en sentido antihorario, esta cara e frontal

	//Rellena los triángulos
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 

	//Activa el culling
	glEnable(GL_CULL_FACE); //No dibuja las caras traseras y así ahorra trabajo a la GPU

	//MATRIZ DE PROYECCION
	//Crea una cámara en perspectiva
	proj = glm::perspective(
		glm::radians(60.0f), //FOV: ángulo de visión
		1.0f, //aspect ratio
		0.1f, //near plane
		50.0f //far plane
	);

	//MATRIZ DE VISTA
	view = glm::mat4(1.0f);

	view[3].z = -6; //mueve la cámara hacia atrás
}
void destroy(){
	//Liberar los recursos utilizados
	glDetachShader(program, vshader);
	glDetachShader(program, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	glDeleteProgram(program);

	//Liberar buffers GPU y VAO
	if (inPos != -1)
		glDeleteBuffers(1, &posVBO);
	if (inColor != -1)
		glDeleteBuffers(1, &colorVBO);
	if (inNormal != -1)
		glDeleteBuffers(1, &normalVBO);
	if (inTexCoord != -1)
		glDeleteBuffers(1, &texCoordVBO);

	glDeleteBuffers(1, &triangleIndexVBO);
	glDeleteVertexArrays(1, &vao);

	//Liberar texturas
	glDeleteTextures(1, &colorTexId);
	glDeleteTextures(1, &emiTexId);
}
void initShader(const char *vname, const char *fname){
	//CARGA DE SHADERS
	vshader = loadShader(vname, GL_VERTEX_SHADER);
	fshader = loadShader(fname, GL_FRAGMENT_SHADER);

	//CREAR EL PROGRAM
	program = glCreateProgram();
	
	//ENLAZAR LOS DOS SHADERS EN UN PROGRAMA
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);

	//brid attribute locations (IDENTIFICADOR A LOS ATRIBUOS DEL PROGRAMA)
	//MUY importante para los VBOs luego.
	glBindAttribLocation(program, 0, "inPos");
	glBindAttribLocation(program, 1, "inColor");
	glBindAttribLocation(program, 2, "inNormal");
	glBindAttribLocation(program, 3, "inTexCoord");

	glLinkProgram(program);

	//ERRORES DE LINK
	int linked;

	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint logLen;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);

		char* logString = new char[logLen];

		glGetProgramInfoLog(program, logLen, NULL, logString);

		std::cout << "Error: " << logString << std::endl;

		delete[] logString;

		glDeleteProgram(program);

		program = 0;

		exit(-1);
	}

	//OBTENER UNIFORMS A TRAVÉS DE IDENTIFICADORES
	uNormalMat = glGetUniformLocation(program, "normal");
	uModelViewMat = glGetUniformLocation(program, "modelView");
	uModelViewProjMat = glGetUniformLocation(program, "modelViewProj");

	//IDENTIFICADORES DE LOS ATRIBUTOS
	inPos = glGetAttribLocation(program, "inPos");
	inColor = glGetAttribLocation(program, "inColor");
	inNormal = glGetAttribLocation(program, "inNormal");
	inTexCoord = glGetAttribLocation(program, "inTexCoord");

	//CREAR LOS IDENTIFICADORES DE LAS VARIABLES Uniform
	uColorTex = glGetUniformLocation(program, "colorTex");
	uEmiTex = glGetUniformLocation(program, "emiTex");
}
void initObj(){
	//CREAR Y ACTIVAR VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//VBO DE POSICIONES (configuración de los atributos de la malla
	if (inPos != -1){
		glGenBuffers(1, &posVBO); //Crea buffer GPU
		glBindBuffer(GL_ARRAY_BUFFER, posVBO); //Activa este buffer
		glBufferData( //Copia los dato CPU a GPU
			GL_ARRAY_BUFFER,
			cubeNVertex * sizeof(float) * 3,
			cubeVertexPos, //Este es el array de posiociones del cubo
			GL_STATIC_DRAW //Indica que los datos cambiarán poco 
		);
		glVertexAttribPointer( //Le dice a OpenGL cómo leer este buffer
			inPos,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(inPos); //Activa el atributo
	}

	//COLOR VBO
	//Contien RGB por vértice
	if (inColor != -1){
		glGenBuffers(1, &colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glBufferData(
			GL_ARRAY_BUFFER,
			cubeNVertex * sizeof(float) * 3,
			cubeVertexColor,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer(
			inColor,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(inColor);
	}

	//NORMALES - que se utilizan para la iluminación, porque se necesita saber haciea dónde mira la superficie
	if (inNormal != -1){
		glGenBuffers(1, &normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(
			GL_ARRAY_BUFFER,
			cubeNVertex * sizeof(float) * 3,
			cubeVertexNormal,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer(
			inNormal,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(inNormal);
	}

	//UVs
	if (inTexCoord != -1){
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(
			GL_ARRAY_BUFFER,
			cubeNVertex * sizeof(float) * 2, //son ahora 2 floats porque UV es u  y v
			cubeVertexTexCoord,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer(
			inTexCoord,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);
		glEnableVertexAttribArray(inTexCoord);
	}

	//CREAR LA LISTA DE ÍNDICES (es decir, la lista de triángulos, indica qué vértices froman cada triángulo)
	//Se usan índices para evitar repetir vértices y es mucho más eficiente
	glGenBuffers(1, &triangleIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIndexVBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		cubeNTriangleIndex * sizeof(unsigned int) * 3,
		cubeTriangleIndex,
		GL_STATIC_DRAW
	);

	//CREAR DOS TEXTURAS
	colorTexId = loadTex("../img/color2.png");
	emiTexId = loadTex("../img/emissive.png");

	//MATRIZ MODEL de este objeto
	model = glm::mat4(1.0f);
}

GLuint loadShader(const char *fileName, GLenum type){ 
	//CARGAR EL FICHERO
	//Lee shader.v0.cert y .frag y los guarda en memoria
	unsigned int fileLen;

	char* source = loadStringFromFile(fileName, fileLen);

	//CREAR Y COMPILAR SHADER
	//Reserva un shader en GPU.
	GLuint shader;

	shader = glCreateShader(type);

	//ENVIAR CÓDIGO
	//Dice a OpenGL: que este es el código GLSL
	glShaderSource(
		shader,
		1,
		(const GLchar**)&source,
		(const GLint*)&fileLen
	);

	//COMPILAR
	glCompileShader(shader);

	//LIBERAR MEMORIA CPU
	delete[] source;

	//COMPROBACIÓN DE ERRORES:
	//Comprobamos que se compiló bien
	GLint compiled;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		//Calculamos una cadena de error
		GLint logLen;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

		char* logString = new char[logLen];

		glGetShaderInfoLog(shader, logLen, NULL, logString);

		std::cout << "Error: " << logString << std::endl;

		delete[] logString;

		glDeleteShader(shader);

		exit(-1);
	}

	return shader; 
}
unsigned int loadTex(const char *fileName){ 
	//CARGAR Y CONFIGURAR UNA TEXTURA GENÉRICA
	unsigned char* map;
	unsigned int w, h;

	map = loadTexture(fileName, w, h);

	if (!map)
	{
		std::cout << "Error cargando el fichero: "
			<< fileName << std::endl;

		exit(-1);
	}

	//CREAR EXTURA OPENGL, ACTIVARLA
	unsigned int texId;
	glGenTextures(1, &texId); //Crea textura GPU
	glBindTexture(GL_TEXTURE_2D, texId); //Activa la textura

	//SUBIR LA IMAGEN A GPU
	glTexImage2D( //Sube la imagen a VRAM
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		w,
		h,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		(GLvoid*)map
	);

	//LIBERAR MEMORIA DE LA CPU
	delete[] map;

	//MIPMAPS -son versiones pequeñas, medianas, grandes de la textura (sirven para evitar ruido, parpadeos y aliasing, para cuando la textura está lejos)
	glGenerateMipmap(GL_TEXTURE_2D);

	//CONFIGURAR MODO DE ACCESO
	//Configurar filtrado - sin esto se vería pixelado
	glTexParameterf( 
		GL_TEXTURE_2D, 
		GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(
		GL_TEXTURE_2D, 
		GL_TEXTURE_MAG_FILTER, 
		GL_LINEAR);
	// Wrap mode
	glTexParameterf(
		GL_TEXTURE_2D, 
		GL_TEXTURE_WRAP_T, 
		GL_CLAMP);
	glTexParameterf(
		GL_TEXTURE_2D, 
		GL_TEXTURE_WRAP_S, 
		GL_CLAMP);

	return texId;
}

void renderFunc(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Cada frame limpia pantalla y limipia la profundidad, sino se acumularán las imágenes anteriores
	glUseProgram(program); //Activa el shader program (la GPU usa nuestros shaders)
	
	//CALCULAR Y SUBIR LAS MATRICES REQUERIDAS POR EL SHADER DE VÉRTICES
	//combina la cámara (view) y la transformación del objeto (model)
	glm::mat4 modelView = view * model; //El orden de multiplicación es muy importante, ya que la matrices no conmutan
	glm::mat4 modelViewProj = proj * view * model; //aplica modelo, cámara y perpectiva
	glm::mat4 normal = glm::transpose(glm::inverse(modelView)); //necesita la inversa y la transpuesta para que la iluminación funcione correctamnete

	//subir modelview
	if (uModelViewMat != -1){
		glUniformMatrix4fv( //Énvía la matriz CPU a GPU
			uModelViewMat, //uniform de destino
			1, //sea sólo una matriz
			GL_FALSE, //no transponer
			&(modelView[0][0]) //puntero a datos
		);
	}

	//subir modelViewProj
	if (uModelViewProjMat != -1){
		glUniformMatrix4fv(
			uModelViewProjMat,
			1,
			GL_FALSE,
			&(modelViewProj[0][0])
		);
	}

	//subir normal matrix
	if (uNormalMat != -1){
		glUniformMatrix4fv(
			uNormalMat,
			1,
			GL_FALSE,
			&(normal[0][0])
		);
	}

	//TEXTURAS
	//Textura del color
	if (uColorTex != -1){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorTexId);
		glUniform1i(uColorTex, 0); //Le dice al shader que el colorText usa la textura 0 
	}

	//Textura emisiva
	if (uEmiTex != -1){
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, emiTexId);
		glUniform1i(uEmiTex, 1); //Le dice al shader que emiTex usa la textura 1
	}

	//BIBUJADO
	//ACTIVAR VAO - con la configuración del objeto y se puntan la lista de los triángulos
	glBindVertexArray(vao); //Le dice a OpenGL que use esta configuración de vértice
	glDrawElements( //aquí se dibujan los triángulos usando índices
		GL_TRIANGLES, //todo renderizado como triángulos
		cubeNTriangleIndex * 3, //cada triángulo utiliza 3 índices
		GL_UNSIGNED_INT, //los tipos de índices
		(void*)0 //empieza al principio del index buffer
	);

	glutSwapBuffers(); //Recuerda el doble buffer. OpenGL, dibuja ocultamente e intercambia buffers

}
void resizeFunc(int width, int height){
	glViewport(0, 0, width, height); //ahora usa toda la ventana para renderizar
	glutPostRedisplay(); // redibuja - le dice a GLUT que hay que renderizar otra vez, porqe o cambió el tamaño de la ventana o hay que refrescar la imagen
}
void idleFunc(){ //Se va a hacer que en cada frama se reinicia el model, aumeta un ángulo y rota el cubo, además de redibujarlo, el resultado es que el cubo girará continuamente
	//REINICIAR EL MODEL
	model = glm::mat4(1.0f);  //Si no se reinicia la rotación se acumula mal y aparecerán deformaciones numéricas
	//VARIABLE ESTÁTICA
	static float angle = 0.0f; //No se reinicia en cada frame entonce el ángulo se conserva y rota
	//ACTUALIZAR EL ÁGULO
	angle = (angle > 3.141592f * 2.0f) ? 0 : angle + 0.01f; //Incrementa el ángulo poco a poco y cuando llega a 2π vuelve a 0 porque 360° = 2π radianes
	//ROTAR MODELO
	model = glm::rotate(
		model,
		angle,
		glm::vec3(1.0f, 1.0f, 0.0f) //rotar alrededor del eje
	);
	//REDIBUJAR
	glutPostRedisplay();
}
void keyboardFunc(unsigned char key, int x, int y){}
void mouseFunc(int button, int state, int x, int y){}