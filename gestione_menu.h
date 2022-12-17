#pragma once
#include "materiali_base.h"
#include "Strutture.h"
#include "enum.h"
extern int selected_obj;
extern  vector<Mesh> Scena;
extern vector<Material> materials;
extern  vector<Shader> shaders;

void main_menu_func(int option)
{
	switch (option)
	{
	case MenuOption::WIRE_FRAME: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case MenuOption::FACE_FILL: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;


	default:
		break;
	}
}
void material_menu_function(int option)
{
	if (selected_obj > -1)
		Scena[selected_obj].material = (MaterialType)option;
}

void shader_menu_function(int option) {
	if (selected_obj > -1) {
		Scena[selected_obj].sceltaVS = shaders[option].value;
		if (Scena[selected_obj].sceltaVS == 3) {
			Scena[selected_obj].sceltaFS = 1;
		}
		else if (Scena[selected_obj].sceltaVS == 4) {
			Scena[selected_obj].sceltaFS = 2;
		}
		else {
			Scena[selected_obj].sceltaFS = 0;
		}
	}
}
void buildOpenGLMenu()
{
	int materialSubMenu = glutCreateMenu(material_menu_function);

	glutAddMenuEntry(materials[MaterialType::EMERALD].name.c_str(), MaterialType::EMERALD);
	glutAddMenuEntry(materials[MaterialType::BRASS].name.c_str(), MaterialType::BRASS);
	glutAddMenuEntry(materials[MaterialType::SNOW_WHITE].name.c_str(), MaterialType::SNOW_WHITE);
	glutAddMenuEntry(materials[MaterialType::YELLOW].name.c_str(), MaterialType::YELLOW);
	glutAddMenuEntry(materials[MaterialType::ROSA].name.c_str(), MaterialType::ROSA);
	glutAddMenuEntry(materials[MaterialType::MARRONE].name.c_str(), MaterialType::MARRONE);

	int shaderSubMenu = glutCreateMenu(shader_menu_function);
	glutAddMenuEntry(shaders[ShaderOption::NONE].name.c_str(), ShaderOption::NONE);
	glutAddMenuEntry(shaders[ShaderOption::PHONG_ILL_INT_SHADING].name.c_str(), ShaderOption::PHONG_ILL_INT_SHADING);
	glutAddMenuEntry(shaders[ShaderOption::BLINN_PHONG_ILL_INT_SHADING].name.c_str(), ShaderOption::BLINN_PHONG_ILL_INT_SHADING);
	glutAddMenuEntry(shaders[ShaderOption::PHONG_ILL_PHONG_SHADING].name.c_str(), ShaderOption::PHONG_ILL_PHONG_SHADING);
	glutAddMenuEntry(shaders[ShaderOption::BLINN_PHONG_ILL_PHONG_SHADING].name.c_str(), ShaderOption::BLINN_PHONG_ILL_PHONG_SHADING);
	glutAddMenuEntry(shaders[ShaderOption::CARTOON_SHADING].name.c_str(), ShaderOption::CARTOON_SHADING);


	glutCreateMenu(main_menu_func); // richiama main_menu_func() alla selezione di una voce menu
	glutAddMenuEntry("Opzioni", -1); //-1 significa che non si vuole gestire questa riga
	glutAddMenuEntry("", -1);
	glutAddMenuEntry("Wireframe", MenuOption::WIRE_FRAME);
	glutAddMenuEntry("Face fill", MenuOption::FACE_FILL);
	glutAddSubMenu("Material", materialSubMenu);
	glutAddSubMenu("Shader", shaderSubMenu);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}