/*
Semestre 2020-2
Pr�ctica 10: Animaci�n Sencilla
Pr�ctica 11: animaci�n compleja
Texturizado: se agrega c�digo para transparencia y blending en el shader
Skybox: Se agrega Skybox como textura ligada a la c�mara.
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include"Model.h"
#include "Skybox.h"
#include"SpotLight.h"

const float toRadians = 3.14159265f / 180.0f;
float movCoche;
float movOffset;
float movHelicopter;
float rotaHelicoptero;
float helicoptero_mueve_z;
float helicoptero_mueve_y;
float helicoptero_mueve_x;
float gira_eje;
int cont_temp;
bool avanza;
bool tope;
bool bandera;
int var;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

//Texturas
Texture Tagave;
//materiales
Material Material_brillante;
Material Material_opaco;
//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

//Modelos
Model Helicoptero ;
Model Helices;
Model Pino1;
Model Pino2;
Model Pino3;
Model Arbol;
Model Lampara1;
Model Lampara2;
Model Lampara3;
Model Bote;
Model Banca;
Model Kiosco;
Model Reja;
Model Piso;
Model Elfo;
Model Camino;
Model Kiosco_madera;
Model Kiosco_base;
Model Kiosco2;
Model Kiosco4;
Model Kiosco6;
Model Kiosco8;
Model Kiosco10;
Model Kiosco12;
Model Kiosco14;
Model Kiosco16;
Model Kiosco18;
Model Kiosco20;
Model Kiosco_columnas;
Model Kiosco_punta;
Model Escalones;
Model FocoRojo;
Model FocoAmarillo;
Model FocoAzul;
Model FocoVerde;
Model Sol;
Model TRIKE;
Model bronto;
Model raptor;
Model volcan;

Skybox skybox;
Skybox skybox_dia;
Skybox skybox_noche;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";
//c�lculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};


	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	//Importacion de Texturas
	Tagave = Texture("Textures/Agave.tga");
	Tagave.LoadTextureA();
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//Importacion de Modelos
	Helicoptero = Model();
	Helicoptero.LoadModel("Models/Helicoptero.obj");
	Helices = Model();
	Helices.LoadModel("Models/Helices.obj");
	Banca = Model();
	Banca.LoadModel("Models/banca.obj");
	Piso = Model();
	Piso.LoadModel("Models/piso.obj");
	Arbol = Model();
	Arbol.LoadModel("Models/arbol_lego.obj");
	Bote = Model();
	Bote.LoadModel("Models/botedebasura.obj");
	Camino = Model();
	Camino.LoadModel("Models/camino.obj");
	Lampara1 = Model();
	Lampara1.LoadModel("Models/lampara.obj");
	Lampara2 = Model();
	Lampara2.LoadModel("Models/lampara2.obj");
	Lampara3 = Model();
	Lampara3.LoadModel("Models/lampara3.obj");
	Pino3 = Model();
	Pino3.LoadModel("Models/pino_lego_realista.obj");
	Pino2 = Model();
	Pino2.LoadModel("Models/pinolegoplano.obj");
	Pino1 = Model();
	Pino1.LoadModel("Models/pinolego.obj");

	//Modelos del kiosco
	Kiosco_madera = Model();
	Kiosco_madera.LoadModel("Models/kioscoCuadroCafeobj.obj");
	Kiosco_base = Model();
	Kiosco_base.LoadModel("Models/basedekiosco.obj");
	Kiosco2 = Model();
	Kiosco2.LoadModel("Models/Techo2x2.obj");
	Kiosco4 = Model();
	Kiosco4.LoadModel("Models/Techo2x4.obj");
	Kiosco6 = Model();
	Kiosco6.LoadModel("Models/Techo2x6.obj");
	Kiosco8 = Model();
	Kiosco8.LoadModel("Models/Techo2x8.obj");
	Kiosco10 = Model();
	Kiosco10.LoadModel("Models/Techo2x10.obj");
	Kiosco12 = Model();
	Kiosco12.LoadModel("Models/Techo2x12.obj");
	Kiosco14 = Model();
	Kiosco14.LoadModel("Models/Techo2x14.obj");
	Kiosco16 = Model();
	Kiosco16.LoadModel("Models/Techo2x16.obj");
	Kiosco18 = Model();
	Kiosco18.LoadModel("Models/Techo2x18.obj");
	Kiosco20 = Model();
	Kiosco20.LoadModel("Models/Techo2x20.obj");
	Kiosco_columnas = Model();
	Kiosco_columnas.LoadModel("Models/columnas.obj");
	Kiosco_punta = Model();
	Kiosco_punta.LoadModel("Models/Cubo_lego_4x4.obj");
	Escalones = Model();
	Escalones.LoadModel("Models/Escalones.obj");
	FocoAmarillo = Model();
	FocoAmarillo.LoadModel("Models/FocoAmarillo.obj");
	FocoAzul = Model();
	FocoAzul.LoadModel("Models/FocoAzul.obj");
	FocoVerde = Model();
	FocoVerde.LoadModel("Models/FocoVerde.obj");
	FocoRojo = Model();
	FocoRojo.LoadModel("Models/FocoRojo.obj");
	Sol = Model();
	Sol.LoadModel("Models/sol.obj");
	TRIKE = Model();
	TRIKE.LoadModel("Models/TRIKE.obj");
	bronto = Model();
	bronto.LoadModel("Models/bronto.obj");
	raptor = Model();
	raptor.LoadModel("Models/raptor.obj");
	volcan = Model();
	volcan.LoadModel("Models/volcan.obj");

	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.025f, 0.005f,
		0.0f, -1.0f, 0.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
		3.0f, 0.0f,
		2.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
		2.0f,1.0f,
		-110.0f, 2.0f, -44.35f,
		0.9f, 0.9f, 0.2f);
	pointLightCount++;
	
	pointLights[2] = PointLight(1.0f, 1.0f, 1.0f,
		2.0f, 1.0f,
		-102.0f, 1.6f, -43.80f,
		0.9f, 0.9f, 0.2f);
	pointLightCount++;

	pointLights[3] = PointLight(0.0f, 0.0f, 1.0f,
		2.0f, 1.0f,
		-104.0f, 1.7f, -43.8f,
		0.9f, 0.9f, 0.2f);
	pointLightCount++;
	
	pointLights[4] = PointLight(1.0f, 1.0f, 1.0f,
		2.0f, 1.0f,
		-120.0f, 1.7f, -43.8f,
		0.9f, 0.9f, 0.2f);
	pointLightCount++;
	
	pointLights[5] = PointLight(1.0f, 1.0f, 1.0f,
	2.0f, 1.0f,
	-101.0f, 1.6f, -37.8f,
	0.9f, 0.9f, 0.2f);
	pointLightCount++;
	
	//luz azul
	pointLights[6] = PointLight(0.0f, 0.0f, 1.0f,
	2.0f, 1.0f,
	-110.0f, 1.8f, -37.8f,
	0.9f, 0.9f, 0.2f);
	pointLightCount++;
	
	pointLights[7] = PointLight(1.0f, 1.0f, 0.0f,
		2.0f, 1.0f,
		-115.0f, 1.8f, -37.8f,
		0.9f, 0.9f, 0.2f);
	pointLightCount++; 
	

	pointLights[8] = PointLight(1.0f, 1.0f, 1.0f,
		2.0f, 1.0f,
		-120.0f, 1.8f, -38.0f,
		0.9f, 0.9f, 0.2f);
	pointLightCount++;







	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f);
	spotLightCount++;

	/*
	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 2.0f,
		10.0f, 0.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	
	//luz de faro
	spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;
	*/
	
	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox_dia = Skybox(skyboxFaces);


	skyboxFaces.clear();

	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_ft.tga");

	skybox_noche = Skybox(skyboxFaces);



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);

	movCoche = -15.0f;
	movOffset = 1.0f;
	movHelicopter = 0.0f;
	helicoptero_mueve_z = 0.0f;
	helicoptero_mueve_y = 0.0f;
	helicoptero_mueve_x = 0.0f;
	gira_eje = -90;
	cont_temp = 0;
	tope = 0;
	var = -480.0f;
	bandera = 0;
	bool dia = false;
	int girahelices = 0;
	skybox = skybox_dia;
	int cont_tiempo_dia_noche = 999;
	bool helicoptero_avanza = true;
	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;
		// movimiento del coche

		if (cont_tiempo_dia_noche % 2500 == 0) {
			if (dia) {
				skybox = skybox_noche;
			}
			else {
				skybox = skybox_dia;
			}
			dia = !dia; //para hace cambio si es 1 se volvera 0, si es 0 se volvera 1
		}

		cont_tiempo_dia_noche += 1;

		//Recibir eventos del usuario
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//luces
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 modelaux(1.0);
		glm::mat4 model(1.0);

		//piso
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Piso.RenderModel();

		//banca 1 y 2 bote de basura 1 y 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-100.0f, -2.0f, -50.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Banca.RenderModel();
		modelaux = model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Bote.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 50.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Banca.RenderModel();
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, 10.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Bote.RenderModel();


		//Arbol
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -70.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Arbol.RenderModel();

		//Camino
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-119.0f, -2.0f, -40.2f));
		model = glm::scale(model, glm::vec3(0.40f, 0.43f, 0.44f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Camino.RenderModel();
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 32.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Camino.RenderModel();

		//Lampara
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-112.0f, -2.0f, -43.25f));
		model = glm::scale(model, glm::vec3(0.23f, 0.23f, 0.23f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara1.RenderModel();
		model = glm::translate(model, glm::vec3(24.5f, 0.0f, 40.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara1.RenderModel();
		model = glm::translate(model, glm::vec3(-24.5f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara2.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(24.5f, 0.0f,-40.7f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara2.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(24.5f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara3.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -40.7f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lampara3.RenderModel();
		

		//pino lego plano
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -70.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pino2.RenderModel();

		//pino raro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, -2.0f, -60.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pino1.RenderModel();


		//pino lego realista
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(50.0f, -2.0f, -50.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pino3.RenderModel();

		//kiosco por modelado jerarquico
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-90.75f,7.25f,-40.248));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_punta.RenderModel();
		modelaux = model;

		model = glm::translate(model, glm::vec3(2.0f,-1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco2.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.0f, -1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco2.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco2.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco2.RenderModel();
		model = modelaux;

		model = glm::translate(model, glm::vec3(0.0f, -4.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco2.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -4.0f, -3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco2.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco2.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco2.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(0.5f, -2.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoAzul.RenderModel();
		//foco //acomodado
		model = glm::translate(model, glm::vec3(0.5f, 0.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoVerde.RenderModel();
		model = modelaux;



		model = glm::translate(model, glm::vec3(3.0f, -2.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco4.RenderModel();

		model = modelaux;
		model = glm::translate(model, glm::vec3(-3.0f, -2.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco4.RenderModel();
		model = modelaux;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(3.0f, -2.0f, .0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco4.RenderModel();
		model = modelaux;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(3.0f, -2.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco4.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.0f, -4.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco4.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(2.0f, -4.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco4.RenderModel();
		model = modelaux;

		model = glm::translate(model, glm::vec3(4.0f, -3.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco6.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-4.0f, -3.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco6.RenderModel();
		model = modelaux;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(4.0f, -3.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco6.RenderModel();
		model = modelaux;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(4.0f, -3.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco6.RenderModel();
		model = modelaux;

		modelaux = model;
		model = glm::translate(model, glm::vec3(5.0f, -4.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco8.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-5.0f, -4.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco8.RenderModel();
		model = modelaux;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(5.0f, -4.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco8.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(0.5f, -4.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoAmarillo.RenderModel();
		model = modelaux;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(5.0f, -4.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco8.RenderModel();
		model = modelaux;
		
		model = glm::translate(model, glm::vec3(6.0f, -5.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco10.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-6.0f, -5.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco10.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(0.5f, -4.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoAmarillo.RenderModel();
		model = modelaux;

		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(6.0f, -5.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco10.RenderModel();
		model = modelaux;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(6.0f, -5.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco10.RenderModel();
		model = modelaux;

		model = glm::translate(model, glm::vec3(7.0f, -6.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco12.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-7.0f, -6.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco12.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(1.5f, -4.0f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoAzul.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoRojo.RenderModel();

		model = modelaux;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(7.0f, -6.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco12.RenderModel();
		model = modelaux;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(7.0f, -6.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco12.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(0.5f, -5.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoVerde.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -8.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoVerde.RenderModel();
		model = modelaux;

		model = glm::translate(model, glm::vec3(8.0f, -7.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco14.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoVerde.RenderModel();
		model = modelaux;
		model = modelaux;
		model = glm::translate(model, glm::vec3(-8.0f, -7.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco14.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoVerde.RenderModel();
		model = modelaux;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(8.0f, -7.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco14.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(0.5f, -4.0f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoAmarillo.RenderModel();
		model = modelaux;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(8.0f, -7.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco14.RenderModel();
		model = modelaux;

		model = glm::translate(model, glm::vec3(9.0f, -8.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco16.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-9.0f, -8.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco16.RenderModel();

		model = modelaux;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(9.0f, -8.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco16.RenderModel();
		model = modelaux;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(9.0f, -8.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco16.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(2.5f, -4.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoAzul.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(-3.0f, 0.0f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoRojo.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(10.0f, -9.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco18.RenderModel();
		//foco
		model = glm::translate(model, glm::vec3(1.5f, -4.0f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		FocoRojo.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-10.0f, -9.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco18.RenderModel();
		model = modelaux;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(10.0f, -9.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco18.RenderModel();
		model = modelaux;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(10.0f, -9.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco18.RenderModel();
		model = modelaux;

		model = glm::translate(model, glm::vec3(11.0f, -10.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco20.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-11.0f, -10.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco20.RenderModel();
		model = modelaux;
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(11.0f, -10.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco20.RenderModel();
		model = modelaux;
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(11.0f, -10.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco20.RenderModel();
		model = modelaux;

		model = glm::translate(model, glm::vec3(2.0f, -1.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = modelaux;

		model = glm::translate(model, glm::vec3(-2.0f, -1.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = modelaux;

		model = glm::translate(model, glm::vec3(-2.0f, -1.0f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = modelaux;

		model = glm::translate(model, glm::vec3(2.0f, -1.0f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_madera.RenderModel();
		model = modelaux;


		









		//columnas del kiosco
		model = glm::translate(model, glm::vec3(0.0f, -19.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_columnas.RenderModel();

		//base del kiosco esta ligado a las columnas por modelado jerarquico 
		model = glm::translate(model, glm::vec3(0.0f, -4.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Kiosco_base.RenderModel();

		//Escaleras, estan unidas por modelo jerarquico a la base
		model = glm::translate(model, glm::vec3(-17.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Escalones.RenderModel();
		model = modelaux;



		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));


		//TRICERATIOPS BEBE
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		TRIKE.RenderModel();

		//bronto
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.8f, -3.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		bronto.RenderModel();

		//raptor
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -5.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		raptor.RenderModel();

		//volcan
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 8.95f, 60.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		volcan.RenderModel();












		//Este es el helicoptero y helices 
		
		model = glm::mat4(1.0);
		posblackhawk = glm::vec3(0.0f, -1.44f, 4.0f);
		model = glm::translate(model, posblackhawk + glm::vec3(helicoptero_mueve_x, helicoptero_mueve_y, helicoptero_mueve_z));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, gira_eje * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Helicoptero.RenderModel();
		model = glm::translate(model, glm::vec3(0.0f, -0.1f, 1.2f));
		model = glm::rotate(model, girahelices * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Helices.RenderModel();
		model = modelaux;

		if (helicoptero_avanza) {
			girahelices += 4;
		}
		else
		{
			girahelices = 0;
		}


		//Agave 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.7f, -2.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Tagave.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		glDisable(GL_BLEND);
		glUseProgram(0);














		mainWindow.swapBuffers();
	}


	return 0;
}