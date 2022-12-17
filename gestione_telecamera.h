#pragma once
#include "Lib.h"
#include "Strutture.h"
#include "enum.h"
extern float cameraSpeed;
extern bool visualizzaAncora;
extern double adder;
extern vec3 asse;
extern string Operazione;
extern string stringa_asse;
extern vector<Mesh> Scena;
void modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor)
{
	//ricordare che mat4(1) costruisce una matrice identità di ordine 4
	mat4 traslation = glm::translate(glm::mat4(1), translation_vector);
	mat4 scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor));
	mat4 rotation = glm::rotate(glm::mat4(1), angle, rotation_vector);

	//Modifica la matrice di Modellazione dell'oggetto della scena selezionato postmolitplicando per le matrici scale*rotarion*traslation

	Scena[selected_obj].ModelM = Scena[selected_obj].ModelM * scale * rotation * traslation;

	glutPostRedisplay();

}
void moveCameraForward()
{
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.position += ViewSetup.direction * cameraSpeed;

}

void moveCameraBack()
{
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.position -= ViewSetup.direction * cameraSpeed;

}

void moveCameraLeft()
{
	//Calcolo la direzione perpendicolare alla direzione della camera e l'alto della camera
		// e muovo la camera a sinistra lungo questa direzione
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec3 direzione_scorrimento = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector));
	ViewSetup.position -= vec4((direzione_scorrimento), .0) * cameraSpeed;
	ViewSetup.target -= vec4((direzione_scorrimento), .0) * cameraSpeed;
}

void moveCameraRight()
{
	//Calcolo la direzione perpendicolare alla direzione della camera e l'alto della camera
	// e muovo la camera a destra lungo questa direzione
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec3 direzione_scorrimento = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector)) * cameraSpeed;
	ViewSetup.position += vec4(direzione_scorrimento, 0);
	ViewSetup.target += vec4(direzione_scorrimento, 0);
}



void moveCameraUp()
{

	ViewSetup.direction = ViewSetup.target - ViewSetup.position;  //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	vec3 direzione_scorrimento = normalize(cross(vec3(ViewSetup.direction), vec3(ViewSetup.upVector)));
	vec3 upDirection = cross(vec3(ViewSetup.direction), direzione_scorrimento) * cameraSpeed;
	ViewSetup.position -= vec4(upDirection, 0.0);
	ViewSetup.target -= vec4(upDirection, 0.0);
}

void moveCameraDown()
{

	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec3 direzione_scorrimento = normalize(cross(vec3(ViewSetup.direction), vec3(ViewSetup.upVector)));
	vec3 upDirection = cross(vec3(ViewSetup.direction), direzione_scorrimento) * cameraSpeed;
	ViewSetup.position += vec4(upDirection, 0.0);
	ViewSetup.target += vec4(upDirection, 0.0);
}

//Gestione eventi tastiera per il movimento della telecamera
void keyboardPressedEvent(unsigned char key, int x, int y)
{
	char* intStr;
	string str;
	switch (key)
	{

	case 'a':
		moveCameraLeft(); break;

	case 'd':
		moveCameraRight();
		break;


	case 'w':
		moveCameraForward();
		break;

	case 's':
		moveCameraBack();
		break;

	case 'U':
		moveCameraUp();
		break;

	case 'D':
		moveCameraDown();
		break;

	case 'v':  //Visualizzazione ancora
		visualizzaAncora = TRUE;
		break;
	
	case 'g':  //Si entra in modalità di operazione traslazione
		OperationMode = TRASLATING;
		Operazione = "TRASLAZIONE";
		break;
	case 'r': //Si entra in modalità di operazione rotazione
		OperationMode = ROTATING;
		Operazione = "ROTAZIONE";
		break;
	case 'S':
		OperationMode = SCALING; //Si entra in modalità di operazione scalatura
		Operazione = "SCALATURA";
		break;
	case 27:
		glutLeaveMainLoop();
		break;
		// Selezione dell'asse
	case 'x':
		WorkingAxis = X;  //Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		stringa_asse = " Asse X";
		break;
	case 'y':
		WorkingAxis = Y;  //Seleziona l'asse Y come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		stringa_asse = " Asse Y";
		break;
	case 'z':
		WorkingAxis = Z;
		stringa_asse = " Asse Z";  //Seleziona l'asse Z come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		break;
	case ' ':
		adder += 0.007;
	default:
		break;
	}

	// Selezione dell'asse per le trasformazioni
	switch (WorkingAxis) {
	case X:	asse = glm::vec3(1.0, 0.0, 0.0);

		break;
	case Y: asse = glm::vec3(0.0, 1.0, 0.0);

		break;
	case Z: asse = glm::vec3(0.0, 0.0, 1.0);

		break;
	default:
		break;
	}



	// I tasti + e -  aggiornano lo spostamento a destra o a sinistra, la rotazione in segno antiorario o in senso orario, la scalatura come amplificazione o diminuizione delle dimensioni

	float amount = .01;
	if (key == '+')
		amount *= 1;

	if (key == '-')
		amount *= -1;


	switch (OperationMode) {

		//la funzione modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor) 
		// definisce la matrice di modellazione che si vuole postmoltiplicare alla matrice di modellazione dell'oggetto selezionato, per poterlo traslare, ruotare scalare.

	case TRASLATING:
		// si passa angle 0 e scale factor =1, 
		modifyModelMatrix(asse * amount, asse, 0.0f, 1.0f);
		break;
	case ROTATING:
		// SI mette a zero il vettore di traslazione (vec3(0) e ad 1 il fattore di scale
		modifyModelMatrix(glm::vec3(0), asse, amount * 2.0f, 1.0f);
		break;
	case SCALING:
		// SI mette a zero il vettore di traslazione (vec3(0), angolo di rotazione a 0 e ad 1 il fattore di scala 1+amount.
		modifyModelMatrix(glm::vec3(0), asse, 0.0f, 1.0f + amount);
		break;



	}
	glutPostRedisplay();
}
