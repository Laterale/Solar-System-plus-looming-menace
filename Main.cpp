#include <iostream>
#include "ShaderMaker.h"
#include "Lib.h"
#include "geometria.h"
#include "gestioneTexture.h"
#include "gestione_menu.h"
#include "materiali_base.h"
#include "load_meshes_assimp.hpp"
#include "gestione_mouse.h"
#include "gestione_telecamera.h"
#include "Strutture.h"
#include "enum.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


string Meshdir = "Meshes/";
string Imagedir = "Textures/";
string SkyboxDir = "Skybox/";

bool visualizzaAncora = FALSE;
double adder = 0.0;
int width = 1024;
int height = 800;
double ast = 0.01;
double siz = 0.5;

//Gestione Mouse

static int last_mouse_pos_Y;
static int last_mouse_pos_X;
static bool moving_trackball = 0;
bool firstMouse = true;
float lastX = (float)width / 2;
float lastY = (float)height / 2;

//Varibili per il reshape
int w_up = width;
int h_up = height;

//Inizializzazione dei modelli 
static vector<MeshObj> Model3D;
vector<vector<MeshObj>> ScenaObj;
string stringa_asse;
float cameraSpeed = 0.1;

//variabili per la comunicazione delle variabili uniformi con gli shader
static unsigned int programId, programId_text, programId1, MatrixProj, MatModel, MatView;
static unsigned int lsceltaFS, lsceltaVS, loc_texture, MatViewS, MatrixProjS;
static unsigned int loc_view_pos, MatModelR, MatViewR, MatrixProjR, loc_view_posR, loc_cubemapR;

unsigned int idTex, texture, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9,texture10, cubemapTexture, programIdr;

float raggio_sfera = 2.5;

string Operazione;
vec3 asse = vec3(0.0, 1.0, 0.0);
int selected_obj = 0;


float Theta = -90.0f;
float Phi = 0.0f;

vector<std::string> faces
{
	/*"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg"*/
		SkyboxDir + "right1.jpg",
		SkyboxDir + "left1.jpg",
		SkyboxDir + "top1.jpg",
		SkyboxDir + "bottom1.jpg",
		SkyboxDir + "front1.jpg",
		SkyboxDir + "back1.jpg"
};

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
//

mat4 Projection, Model, View;
point_light light;
static vector<Material> materials;
static vector<Shader> shaders;

LightShaderUniform light_unif = {};

void INIT_SHADER(void)

{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_C.glsl";
	char* fragmentShader = (char*)"fragmentShader_C.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);



	vertexShader = (char*)"vertexShader_CubeMap.glsl";
	fragmentShader = (char*)"fragmentShader_CubeMap.glsl";

	programId1 = ShaderMaker::createProgram(vertexShader, fragmentShader);

	vertexShader = (char*)"vertexShader_riflessione.glsl";
	fragmentShader = (char*)"fragmentShader_riflessione.glsl";

	programIdr = ShaderMaker::createProgram(vertexShader, fragmentShader);
}


void INIT_Illuminazione()
{
	//Setup della luce
	light.position = { 0.0, 80.0, 0.0 };
	light.color = { 0.8, 0.8, 0.8 };
	light.power = 1.0;

	//Setup dei materiali
	// Materials setup
	materials.resize(9);
	materials[MaterialType::RED_PLASTIC].name = "ROSSO";
	materials[MaterialType::RED_PLASTIC].ambient = red_plastic_ambient;
	materials[MaterialType::RED_PLASTIC].diffuse = red_plastic_diffuse;
	materials[MaterialType::RED_PLASTIC].specular = red_plastic_specular;
	materials[MaterialType::RED_PLASTIC].shininess = red_plastic_shininess;

	materials[MaterialType::EMERALD].name = "SMERALDO";
	materials[MaterialType::EMERALD].ambient = emerald_ambient;
	materials[MaterialType::EMERALD].diffuse = emerald_diffuse;
	materials[MaterialType::EMERALD].specular = emerald_specular;
	materials[MaterialType::EMERALD].shininess = emerald_shininess;

	materials[MaterialType::BRASS].name = "OTTONE";
	materials[MaterialType::BRASS].ambient = brass_ambient;
	materials[MaterialType::BRASS].diffuse = brass_diffuse;
	materials[MaterialType::BRASS].specular = brass_specular;
	materials[MaterialType::BRASS].shininess = brass_shininess;

	materials[MaterialType::SNOW_WHITE].name = "BIANCO";
	materials[MaterialType::SNOW_WHITE].ambient = snow_white_ambient;
	materials[MaterialType::SNOW_WHITE].diffuse = snow_white_diffuse;
	materials[MaterialType::SNOW_WHITE].specular = snow_white_specular;
	materials[MaterialType::SNOW_WHITE].shininess = snow_white_shininess;

	materials[MaterialType::YELLOW].name = "GIALLO";
	materials[MaterialType::YELLOW].ambient = yellow_ambient;
	materials[MaterialType::YELLOW].diffuse = yellow_diffuse;
	materials[MaterialType::YELLOW].specular = yellow_specular;
	materials[MaterialType::YELLOW].shininess = yellow_shininess;

	materials[MaterialType::ROSA].name = "ROSA";
	materials[MaterialType::ROSA].ambient = rosa_ambient;
	materials[MaterialType::ROSA].diffuse = rosa_diffuse;
	materials[MaterialType::ROSA].specular = rosa_specular;
	materials[MaterialType::ROSA].shininess = rosa_shininess;

	materials[MaterialType::MARRONE].name = "MARRONE";
	materials[MaterialType::MARRONE].ambient = marrone_ambient;
	materials[MaterialType::MARRONE].diffuse = marrone_diffuse;
	materials[MaterialType::MARRONE].specular = marrone_specular;
	materials[MaterialType::MARRONE].shininess = marrone_shininess;

	materials[MaterialType::CHROME].name = "CROMATO";
	materials[MaterialType::CHROME].ambient = chrome_ambient;
	materials[MaterialType::CHROME].diffuse = chrome_diffuse;
	materials[MaterialType::CHROME].specular = chrome_specular;
	materials[MaterialType::CHROME].shininess = chrome_shininess;

	materials[MaterialType::NO_MATERIAL].name = "NO_MATERIAL";
	materials[MaterialType::NO_MATERIAL].ambient = glm::vec3(1, 1, 1);
	materials[MaterialType::NO_MATERIAL].diffuse = glm::vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].specular = glm::vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].shininess = 1.f;

	//Setup degli shader
	shaders.resize(6);
	shaders[ShaderOption::NONE].value = 0;
	shaders[ShaderOption::NONE].name = "NONE";
	shaders[ShaderOption::PHONG_ILL_INT_SHADING].value = 1;
	shaders[ShaderOption::PHONG_ILL_INT_SHADING].name = "ILLUMINAZIONE DI PHONG CON SHADING INTERPOLATIVO";
	shaders[ShaderOption::BLINN_PHONG_ILL_INT_SHADING].value = 2;
	shaders[ShaderOption::BLINN_PHONG_ILL_INT_SHADING].name = "ILLUMINAZIONE DI BLINN-PHONG CON SHADING INTERPOLATIVO";
	shaders[ShaderOption::PHONG_ILL_PHONG_SHADING].value = 3;
	shaders[ShaderOption::PHONG_ILL_PHONG_SHADING].name = "ILLUMINAZIONE DI PHONG CON SHADING DI PHONG";
	shaders[ShaderOption::BLINN_PHONG_ILL_PHONG_SHADING].value = 4;
	shaders[ShaderOption::BLINN_PHONG_ILL_PHONG_SHADING].name = "ILLUMINAZIONE DI BLINN-PHONG CON SHADING DI PHONG";
	shaders[ShaderOption::CARTOON_SHADING].value = 5;
	shaders[ShaderOption::CARTOON_SHADING].name = "CARTOON SHADING";

}

void crea_VAO_Vector_MeshObj(MeshObj* mesh)
{

	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &mesh->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertici.size() * sizeof(vec3), mesh->vertici.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &mesh->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, mesh->colori.size() * sizeof(vec4), mesh->colori.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	//Genero , rendo attivo, riempio il VBO delle normali
	glGenBuffers(1, &mesh->VBO_normali);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_normali);
	glBufferData(GL_ARRAY_BUFFER, mesh->normali.size() * sizeof(vec3), mesh->normali.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO delle NORMALI nel layer 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	//EBO di tipo indici
	glGenBuffers(1, &mesh->EBO_indici);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO_indici);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indici.size() * sizeof(GLuint), mesh->indici.data(), GL_STATIC_DRAW);

}


void crea_VAO_Vector(Mesh* mesh)
{
	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &mesh->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertici.size() * sizeof(vec3), mesh->vertici.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &mesh->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, mesh->colori.size() * sizeof(vec4), mesh->colori.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	//Genero , rendo attivo, riempio il VBO delle normali
	glGenBuffers(1, &mesh->VBO_normali);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_normali);
	glBufferData(GL_ARRAY_BUFFER, mesh->normali.size() * sizeof(vec3), mesh->normali.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO delle NORMALI nel layer 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);


	glGenBuffers(1, &mesh->VBO_coord_texture);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_coord_texture);
	glBufferData(GL_ARRAY_BUFFER, mesh->texCoords.size() * sizeof(vec2), mesh->texCoords.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO delle NORMALI nel layer 2
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(3);

	//EBO di tipo indici
	glGenBuffers(1, &mesh->EBO_indici);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO_indici);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indici.size() * sizeof(GLuint), mesh->indici.data(), GL_STATIC_DRAW);
}

void INIT_VAO(void)
{
	Mesh Sky, Mercurio, Venere, Terra, Luna, Marte, Giove, Saturno, Anello, Urano, Nettuno, Sole, tra, tra1, tra2, tra3, tra4, tra5, tra6, tra7;

	string name = "mercurio.jpg";
	string path = Imagedir + name;
	texture = loadTexture(path.c_str(), 0);

	name = "venere1.jpg";
	path = Imagedir + name;
	texture1 = loadTexture(path.c_str(), 0);

	name = "terra.jpg";
	path = Imagedir + name;
	texture2 = loadTexture(path.c_str(), 0);

	name = "luna1.jpg";
	path = Imagedir + name;
	texture3 = loadTexture(path.c_str(), 0);

	name = "marte1.jpg";
	path = Imagedir + name;
	texture4 = loadTexture(path.c_str(), 0);

	name = "giove1.jpg";
	path = Imagedir + name;
	texture5 = loadTexture(path.c_str(), 0);

	name = "saturno1.jpg";
	path = Imagedir + name;
	texture6 = loadTexture(path.c_str(), 0);

	name = "anello.jpg";
	path = Imagedir + name;
	texture7 = loadTexture(path.c_str(), 0);

	name = "urano1.jpg";
	path = Imagedir + name;
	texture8 = loadTexture(path.c_str(), 0);

	name = "nettuno1.jpg";
	path = Imagedir + name;
	texture9 = loadTexture(path.c_str(), 0);

	name = "sole.jpg";
	path = Imagedir + name;
	texture10 = loadTexture(path.c_str(), 0);


	cubemapTexture = loadCubemap(faces, 0);

	//Sky
	crea_cubo(&Sky);
	crea_VAO_Vector(&Sky);
	Scena.push_back(Sky);

	crea_sfera(&Mercurio, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Mercurio);
	Mercurio.ModelM = mat4(1.0);
	Mercurio.nome = "Mercurio";
	Mercurio.material = MaterialType::NO_MATERIAL;
	Mercurio.sceltaVS = 1;
	Mercurio.sceltaFS = 0;
	Scena.push_back(Mercurio);

	crea_sfera(&Venere, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Venere);
	Venere.ModelM = mat4(1.0);
	Venere.nome = "Venere";
	Venere.material = MaterialType::NO_MATERIAL;
	Venere.sceltaVS = 1;
	Venere.sceltaFS = 0;
	Scena.push_back(Venere);

	crea_sfera(&Terra, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Terra);
	Terra.ModelM = mat4(1.0);
	Terra.nome = "Terra";
	Terra.material = MaterialType::NO_MATERIAL;
	Terra.sceltaVS = 1;
	Terra.sceltaFS = 0;
	Scena.push_back(Terra);

	crea_sfera(&Luna, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Luna);
	Luna.ModelM = mat4(1.0);
	Luna.nome = "Luna";
	Luna.material = MaterialType::NO_MATERIAL;
	Luna.sceltaVS = 1;
	Luna.sceltaFS = 0;
	Scena.push_back(Luna);

	crea_sfera(&Marte, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Marte);
	Marte.ModelM = mat4(1.0);
	Marte.nome = "Marte";
	Marte.material = MaterialType::NO_MATERIAL;
	Marte.sceltaVS = 1;
	Marte.sceltaFS = 0;
	Scena.push_back(Marte);

	crea_sfera(&Giove, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Giove);
	Giove.ModelM = mat4(1.0);
	Giove.nome = "Giove";
	Giove.material = MaterialType::NO_MATERIAL;
	Giove.sceltaVS = 1;
	Giove.sceltaFS = 0;
	Scena.push_back(Giove);

	crea_sfera(&Saturno, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Saturno);
	Saturno.ModelM = mat4(1.0);
	Saturno.nome = "Saturno";
	Saturno.material = MaterialType::NO_MATERIAL;
	Saturno.sceltaVS = 1;
	Saturno.sceltaFS = 0;
	Scena.push_back(Saturno);

	crea_toro(&Anello, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Anello);
	Anello.ModelM = mat4(1.0);
	Anello.nome = "Anelli";
	Anello.material = MaterialType::NO_MATERIAL;
	Anello.sceltaVS = 1;
	Anello.sceltaFS = 0;
	Scena.push_back(Anello);

	crea_sfera(&Urano, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Urano);
	Urano.ModelM = mat4(1.0);
	Urano.nome = "Urano";
	Urano.material = MaterialType::NO_MATERIAL;
	Urano.sceltaVS = 1;
	Urano.sceltaFS = 0;
	Scena.push_back(Urano);

	crea_sfera(&Nettuno, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Nettuno);
	Nettuno.ModelM = mat4(1.0);
	Nettuno.nome = "Nettuno";
	Nettuno.material = MaterialType::NO_MATERIAL;
	Nettuno.sceltaVS = 1;
	Nettuno.sceltaFS = 0;
	Scena.push_back(Nettuno);

	crea_sfera(&Sole, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Sole);
	Sole.ModelM = mat4(1.0);
	Sole.ModelM = translate(Sole.ModelM, vec3(0.0, 0.0, 0.0));
	Sole.ModelM = scale(Sole.ModelM, vec3(siz * 100, siz * 100, siz * 100));
	Sole.nome = "Sole";
	Sole.material = MaterialType::NO_MATERIAL;
	Sole.sceltaVS = 1;
	Sole.sceltaFS = 0;
	Scena.push_back(Sole);

	crea_toro1(&tra, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&tra);
	tra.ModelM = mat4(1.0);
	tra.ModelM = translate(tra.ModelM, vec3(0.0, 0.0, 0.0));
	tra.ModelM = scale(tra.ModelM, vec3(38.0 + 80, 100 , 38.0 + 80));
	tra.nome = "Traiettoria Mercurio";
	tra.material = MaterialType::SNOW_WHITE;
	tra.sceltaVS = 1;
	tra.sceltaFS = 0;
	Scena.push_back(tra);

	crea_toro1(&tra1, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&tra1);
	tra1.ModelM = mat4(1.0);
	tra1.ModelM = translate(tra1.ModelM, vec3(0.0, 0.0, 0.0));
	tra1.ModelM = scale(tra1.ModelM, vec3(72.0 + 80, 100, 72.0 + 80));
	tra1.nome = "Traiettoria Venere";
	tra1.material = MaterialType::SNOW_WHITE;
	tra1.sceltaVS = 1;
	tra1.sceltaFS = 0;
	Scena.push_back(tra1);

	crea_toro1(&tra2, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&tra2);
	tra2.ModelM = mat4(1.0);
	tra2.ModelM = translate(tra2.ModelM, vec3(0.0, 0.0, 0.0));
	tra2.ModelM = scale(tra2.ModelM, vec3(100.0 + 80, 100, 100.0 + 80));
	tra2.nome = "Traiettoria Terra";
	tra2.material = MaterialType::SNOW_WHITE;
	tra2.sceltaVS = 1;
	tra2.sceltaFS = 0;
	Scena.push_back(tra2);

	crea_toro1(&tra3, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&tra3);
	tra3.ModelM = mat4(1.0);
	tra3.ModelM = translate(tra3.ModelM, vec3(0.0, 0.0, 0.0));
	tra3.ModelM = scale(tra3.ModelM, vec3(150.0 + 80, 100, 150.0 + 80));
	tra3.nome = "Traiettoria Marte";
	tra3.material = MaterialType::SNOW_WHITE;
	tra3.sceltaVS = 1;
	tra3.sceltaFS = 0;
	Scena.push_back(tra3);

	crea_toro1(&tra4, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&tra4);
	tra4.ModelM = mat4(1.0);
	tra4.ModelM = translate(tra4.ModelM, vec3(0.0, 0.0, 0.0));
	tra4.ModelM = scale(tra4.ModelM, vec3(520.0 + 80, 100, 520.0 + 80));
	tra4.nome = "Traiettoria Giove";
	tra4.material = MaterialType::SNOW_WHITE;
	tra4.sceltaVS = 1;
	tra4.sceltaFS = 0;
	Scena.push_back(tra4);

	crea_toro1(&tra5, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&tra5);
	tra5.ModelM = mat4(1.0);
	tra5.ModelM = translate(tra5.ModelM, vec3(0.0, 0.0, 0.0));
	tra5.ModelM = scale(tra5.ModelM, vec3(950.0 + 80, 100, 950.0 + 80));
	tra5.nome = "Traiettoria Saturno";
	tra5.material = MaterialType::SNOW_WHITE;
	tra5.sceltaVS = 1;
	tra5.sceltaFS = 0;
	Scena.push_back(tra5);

	crea_toro1(&tra6, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&tra6);
	tra6.ModelM = mat4(1.0);
	tra6.ModelM = translate(tra6.ModelM, vec3(0.0, 0.0, 0.0));
	tra6.ModelM = scale(tra6.ModelM, vec3(1980.0 + 80, 100, 1980.0 + 80));
	tra6.nome = "Traiettoria Urano";
	tra6.material = MaterialType::SNOW_WHITE;
	tra6.sceltaVS = 1;
	tra6.sceltaFS = 0;
	Scena.push_back(tra6);

	crea_toro1(&tra7, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&tra7);
	tra7.ModelM = mat4(1.0);
	tra7.ModelM = translate(tra7.ModelM, vec3(0.0, 0.0, 0.0));
	tra7.ModelM = scale(tra7.ModelM, vec3(3010.0 + 80, 100, 3010.0 + 80));
	tra7.nome = "Traiettoria Nettuno";
	tra7.material = MaterialType::SNOW_WHITE;
	tra7.sceltaVS = 1;
	tra7.sceltaFS = 0;
	Scena.push_back(tra7);

	bool obj;

	name = "Stone.obj";
	path = Meshdir + name;
	obj = loadAssImp(path.c_str(), Model3D);
	int nmeshes = Model3D.size();

	for (int i = 0; i < nmeshes; i++)
	{
		crea_VAO_Vector_MeshObj(&Model3D[i]);
		Model3D[i].ModelM = mat4(1.0);
		Model3D[i].ModelM = translate(Model3D[i].ModelM, vec3(90.0, 90.0, 90.0));
		Model3D[i].ModelM = scale(Model3D[i].ModelM, vec3(7.0, 7.0, 7.0));
		Model3D[i].ModelM = rotate(Model3D[i].ModelM, radians(45.0f), vec3(0.5, -1.0, 0.0));
		Model3D[i].nome = "Asteroid";
		Model3D[i].sceltaVS = 1;
		Model3D[i].sceltaFS = 4;
	}

	ScenaObj.push_back(Model3D);

	Model3D.clear();
}

void keyboardReleasedEvent(unsigned char key, int x, int y)
{
	{
		switch (key)
		{
		case 'v':
			visualizzaAncora = !visualizzaAncora;
			break;
		default:
			break;
		}
	}
	glutPostRedisplay();
}

void INIT_CAMERA_PROJECTION(void)
{
	//Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = glm::vec4(0.0, 80, 70.0, 0.0);
	ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);

	//Imposto la proiezione prospettica
	PerspectiveSetup = {};
	PerspectiveSetup.aspect = (GLfloat)width / (GLfloat)height;
	PerspectiveSetup.fovY = 45.0f;
	PerspectiveSetup.far_plane = 5000.0f;
	PerspectiveSetup.near_plane = 0.1f;
}

void resize(int w, int h)
{
	//Imposto la matrice di Proiezione per il rendering del testo


	//Imposto la matrice di proiezione per la scena da diegnare
	Projection = perspective(radians(PerspectiveSetup.fovY), PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);

	float AspectRatio_mondo = (float)(width) / (float)(height); //Rapporto larghezza altezza di tutto ciò che è nel mondo
	 //Se l'aspect ratio del mondo è diversa da quella della finestra devo mappare in modo diverso 
	 //per evitare distorsioni del disegno
	if (AspectRatio_mondo > w / h)   //Se ridimensioniamo la larghezza della Viewport
	{
		glViewport(0, 0, w, w / AspectRatio_mondo);
		w_up = (float)w;
		h_up = w / AspectRatio_mondo;
	}
	else {  //Se ridimensioniamo la larghezza della viewport oppure se l'aspect ratio tra la finestra del mondo 
			//e la finestra sullo schermo sono uguali
		glViewport(0, 0, h * AspectRatio_mondo, h);
		w_up = h * AspectRatio_mondo;
		h_up = (float)h;
	}
}

void drawScene(void)
{
	glUniformMatrix4fv(MatrixProj, 1, GL_FALSE, value_ptr(Projection));
	View = lookAt(vec3(ViewSetup.position), vec3(ViewSetup.target), vec3(ViewSetup.upVector));
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Disegno Sky box

	glDepthMask(GL_FALSE);
	glUseProgram(programId1);
	glUniform1i(glGetUniformLocation(programId1, "skybox"), 0);
	glUniformMatrix4fv(MatrixProjS, 1, GL_FALSE, value_ptr(Projection));
	glUniformMatrix4fv(MatViewS, 1, GL_FALSE, value_ptr(View));
	// skybox cube
	glBindVertexArray(Scena[0].VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawElements(GL_TRIANGLES, Scena[0].indici.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);

	//Costruisco la matrice di Vista che applicata ai vertici in coordinate del mondo li trasforma nel sistema di riferimento della camera.
	//Passo al Vertex Shader il puntatore alla matrice View, che sarà associata alla variabile Uniform mat4 Projection
	// 
   //all'interno del Vertex shader. Uso l'identificatio MatView
	glPointSize(10.0);

	glUseProgram(programId);
	glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));

	//Passo allo shader il puntatore a  colore luce, posizione ed intensità

	//glUniform3f(light_unif.light_position_pointer, light.position.x + 10 * cos(radians(angolo)), light.position.y, light.position.z + 10 * sin(radians(angolo)));
	glUniform3f(light_unif.light_position_pointer, light.position.x, light.position.y, light.position.z);

	glUniform3f(light_unif.light_color_pointer, light.color.r, light.color.g, light.color.b);
	glUniform1f(light_unif.light_power_pointer, light.power);

	//Passo allo shader il puntatore alla posizione della camera
	glUniform3f(loc_view_pos, ViewSetup.position.x, ViewSetup.position.y, ViewSetup.position.z);

	for (int k = 1; k < Scena.size(); k++)
	{
		if (k == 1) {
			Scena[k].ModelM = mat4(1.0);
			Scena[k].ModelM = translate(Scena[k].ModelM, vec3((38.0 + 80) * cos(adder * 4.7), 0.0, (38.0 + 80) * sin(adder * 4.7)));
			Scena[k].ModelM = scale(Scena[k].ModelM, vec3(siz / 3.0, siz / 3.0, siz / 3.0));
		}
		else if (k == 2) {
			Scena[k].ModelM = mat4(1.0);
			Scena[k].ModelM = translate(Scena[k].ModelM, vec3((72.0 + 80) * cos(adder * 3.5), 0.0, (72.0 + 80) * sin(adder * 3.5)));
			Scena[k].ModelM = scale(Scena[k].ModelM, vec3(siz / 1.5, siz / 1.5, siz / 1.5));
		}
		else if (k == 3) {
			Scena[k].ModelM = mat4(1.0);
			Scena[k].ModelM = translate(Scena[k].ModelM, vec3((100.0 + 80) * cos(adder * 2.9), 0.0, (100.0 + 80) * sin(adder * 2.9)));
			Scena[k].ModelM = scale(Scena[k].ModelM, vec3(siz, siz, siz));
		}
		else if (k == 4) {
			Scena[k].ModelM = mat4(1.0);
			Scena[k].ModelM = translate(Scena[k].ModelM, vec3((100.0 + 80) * cos(adder * 2.9), 0.0,(100.0 + 80) * sin((adder + 0.007) * 2.9)));
			Scena[k].ModelM = scale(Scena[k].ModelM, vec3(siz / 7, siz / 7, siz / 7));
		}
		else if (k == 5) {
			Scena[k].ModelM = mat4(1.0);
			Scena[k].ModelM = translate(Scena[k].ModelM, vec3((150.0 + 80) * cos(adder * 2.4), 0.0, (150.0 + 80) * sin(adder * 2.4)));
			Scena[k].ModelM = scale(Scena[k].ModelM, vec3(siz / 1.9, siz / 1.9, siz / 1.9));
		}
		else if (k == 6) {
			Scena[k].ModelM = mat4(1.0);
			Scena[k].ModelM = translate(Scena[k].ModelM, vec3((520.0 + 80) * cos(adder * 1.3), 0.0, (520.0 + 80) * sin(adder * 1.3)));
			Scena[k].ModelM = scale(Scena[k].ModelM, vec3(siz * 11, siz * 11, siz * 11));
		}
		else if (k == 7) {
			Scena[k].ModelM = mat4(1.0);
			Scena[k].ModelM = translate(Scena[k].ModelM, vec3((950.0 + 80) * cos(adder * 0.97), 0.0, (950.0 + 80) * sin(adder * 0.97)));
			Scena[k].ModelM = scale(Scena[k].ModelM, vec3(siz * 9, siz * 9, siz * 9));
		}
		else if (k == 8) {
			Scena[k].ModelM = mat4(1.0);
			Scena[k].ModelM = translate(Scena[k].ModelM, vec3((950.0 + 80) * cos(adder * 0.97), 0.0, (950.0 + 80) * sin(adder * 0.97)));
			Scena[k].ModelM = scale(Scena[k].ModelM, vec3(siz * 11, 0.5, siz * 11));
		}
		else if (k == 9) {
			Scena[k].ModelM = mat4(1.0);
			Scena[k].ModelM = translate(Scena[k].ModelM, vec3((1980.0 + 80) * cos(adder * 0.68), 0.0, (1980.0 + 80) * sin(adder * 0.68)));
			Scena[k].ModelM = scale(Scena[k].ModelM, vec3(siz * 4, siz * 4, siz * 4));
		}
		else if (k == 10) {
			Scena[k].ModelM = mat4(1.0);
			Scena[k].ModelM = translate(Scena[k].ModelM, vec3((3010.0 + 80) * cos(adder * 0.54), 0.0, (3010.0 + 80) * sin(adder * 0.54)));
			Scena[k].ModelM = scale(Scena[k].ModelM, vec3(siz * 3, siz * 3, siz * 3));
		}
		//Trasformazione delle coordinate dell'ancora dal sistema di riferimento dell'oggetto in sistema
		//di riferimento del mondo premoltiplicando per la matrice di Modellazione.
		Scena[k].ancora_world = Scena[k].ancora_obj;
		Scena[k].ancora_world = Scena[k].ModelM * Scena[k].ancora_world;
		//Passo al Vertex Shader il puntatore alla matrice Model dell'oggetto k-esimo della Scena, che sarà associata alla variabile Uniform mat4 Projection
		//all'interno del Vertex shader. Uso l'identificatio MatModel

		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k].ModelM));
		glUniform1i(lsceltaVS, Scena[k].sceltaVS);
		glUniform1i(lsceltaFS, Scena[k].sceltaFS);
		//Passo allo shader il puntatore ai materiali
		glUniform3fv(light_unif.material_ambient, 1, glm::value_ptr(materials[Scena[k].material].ambient));
		glUniform3fv(light_unif.material_diffuse, 1, glm::value_ptr(materials[Scena[k].material].diffuse));
		glUniform3fv(light_unif.material_specular, 1, glm::value_ptr(materials[Scena[k].material].specular));
		glUniform1f(light_unif.material_shininess, materials[Scena[k].material].shininess);
		glBindVertexArray(Scena[k].VAO);

		if (visualizzaAncora == TRUE)
		{
			//Visualizzo l'ancora dell'oggetto
			int ind = Scena[k].indici.size() - 1;
			glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, BUFFER_OFFSET(ind * sizeof(GLuint)));
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (k == 1) {
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, texture);
		}
		else if (k == 2) {
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, texture1);
		}
		else if (k == 3) {
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, texture2);
		}
		else if (k == 4) {
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, texture3);
		}
		else if (k == 5) {
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, texture4);
		}
		else if (k == 6) {
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, texture5);
		}
		else if (k == 7) {
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, texture6);
		}
		else if (k == 8) {
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, texture7);
		}
		else if (k == 9) {
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, texture8);
		}
		else if (k == 10) {
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, texture9);
		}
		else if (k == 11) {
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, texture10);
		}
		else {

		}
		glDrawElements(GL_TRIANGLES, (Scena[k].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}

	//Visualizzo gli oggetti di tipo Mesh Obj caricati dall'esterno: 
	//la j-esima Mesh è costituita da ScenaObj[j].size() mesh. 
	for (int j = 0; j < ScenaObj.size(); j++)
	{

		for (int k = 0; k < ScenaObj[j].size(); k++)
	    {

			//Trasformazione delle coordinate dell'ancora dal sistema di riferimento dell'oggetto in sistema
			//di riferimento del mondo premoltiplicando per la matrice di Modellazione.

		    //Passo al Vertex Shader il puntatore alla matrice Model dell'oggetto k-esimo della Scena, che sarà associata alla variabile Uniform mat4 Projection
			//all'interno del Vertex shader. Uso l'identificatio MatModel


			glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(ScenaObj[j][k].ModelM));
			glUniform1i(lsceltaVS, ScenaObj[j][k].sceltaVS);
			glUniform1i(lsceltaFS, ScenaObj[j][k].sceltaFS);
			//Passo allo shader il puntatore ai materiali
			glUniform3fv(light_unif.material_ambient, 1, value_ptr(ScenaObj[j][k].materiale.ambient));
			glUniform3fv(light_unif.material_diffuse, 1, value_ptr(ScenaObj[j][k].materiale.diffuse));
			glUniform3fv(light_unif.material_specular, 1, value_ptr(ScenaObj[j][k].materiale.specular));
			glUniform1f(light_unif.material_shininess, ScenaObj[j][k].materiale.shininess);

			glBindVertexArray(ScenaObj[j][k].VAO);


			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, (ScenaObj[j][k].indici.size()) * sizeof(GLuint), GL_UNSIGNED_INT, 0);


			glBindVertexArray(0);
		}
	}
	glutSwapBuffers();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);


	//Inizializzo finestra per il rendering della scena 3d con tutti i suoi eventi le sue inizializzazioni e le sue impostazioni

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Solar System");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);

	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboardPressedEvent);
	glutKeyboardUpFunc(keyboardReleasedEvent);
	//glutPassiveMotionFunc(my_passive_mouse);


	glutMotionFunc(mouseActiveMotion); // Evento tasto premuto
	glewExperimental = GL_TRUE;
	glewInit();


	//Inizializzazione setup illuminazione, materiali
	INIT_Illuminazione();
	//Inizializzazione setup Shader
	INIT_SHADER();
	//Inizializzazione VAO
	INIT_VAO();
	//Inizializzazione setup telecamera
	INIT_CAMERA_PROJECTION();

	//Menu collegato al tasto centrale
	buildOpenGLMenu();



	//Abilita l'uso del Buffer di Profondità per la gestione dell'eliminazione dlele superifici nascoste
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Projection (in vertex shader).
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Projection al Vertex Shader
	MatrixProj = glGetUniformLocation(programId, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatModel = glGetUniformLocation(programId, "Model");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 View (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice View al Vertex Shader
	MatView = glGetUniformLocation(programId, "View");
	lsceltaVS = glGetUniformLocation(programId, "sceltaVS");
	lsceltaFS = glGetUniformLocation(programId, "sceltaFS");
	loc_view_pos = glGetUniformLocation(programId, "ViewPos");
	loc_texture = glGetUniformLocation(programId, "id_tex");
	//location delle variabili uniformi per la gestione della luce
	light_unif.light_position_pointer = glGetUniformLocation(programId, "light.position");
	light_unif.light_color_pointer = glGetUniformLocation(programId, "light.color");
	light_unif.light_power_pointer = glGetUniformLocation(programId, "light.power");

	//location delle variabili uniformi per la gestione dei materiali
	light_unif.material_ambient = glGetUniformLocation(programId, "material.ambient");
	light_unif.material_diffuse = glGetUniformLocation(programId, "material.diffuse");
	light_unif.material_specular = glGetUniformLocation(programId, "material.specular");
	light_unif.material_shininess = glGetUniformLocation(programId, "material.shininess");

	//location variabili uniformi per lo shader della gestione della cubemap
	MatrixProjS = glGetUniformLocation(programId1, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader


	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatViewS = glGetUniformLocation(programId1, "View");


	MatModelR = glGetUniformLocation(programIdr, "Model");
	MatViewR = glGetUniformLocation(programIdr, "View");
	MatrixProjR = glGetUniformLocation(programIdr, "Projection");
	loc_view_posR = glGetUniformLocation(programIdr, "ViewPos");
	loc_cubemapR = glGetUniformLocation(programIdr, "cubemap");
	glutMainLoop();
}
