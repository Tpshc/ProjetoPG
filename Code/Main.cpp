#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "Model.h"
#include "Light.h"
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>

// ---------------Global Vars Start ---------
void DisplayLights();
void loadModels();
void display();
void specialKeys();
void drawRetangle();
void drawGrid();
void selectNext();
void selectLast();
void translateModel(int selector, double deslocamento);
bool divided_view_port = false;
Camera cameraPrincipal = Camera();
Camera cameraDir = Camera();
Model cameraObj = Model("Resources/PolygonsWithNormals/camera.obj");
vector<Model> objs;
bool keyboard[256];
int modelIndex = 0;
double tempRotate_y = 0;
double tempRotate_x = 0;
const double translateQtd = 0.02;
Light L1 = Light();
Light L2 = Light();
int lightIndex = 0;
bool lightSelected = 0;
double mousepos_x = 0;
double mousepos_y = 0;
bool clickPrincipal = false;
bool clickDiretor = false;
bool mouse_right = false;
GLfloat wWidth = 1280.0;
GLfloat wHeight = 720.0;
float zNear = 2.0f;
void idle(void);
int frameCount = 0;
float fps = 0;
int currentTime = 0, previousTime = 0;
void calculateFPS();
void drawFPS();
void printw(float x, float y, float z, char* format, ...);
GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_24;


// -------------Global Vars End ------------

void display(){

	//  Clear screen and Z-buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--------------------------Camera Setup --------------------------------
	// normal mode

	if (!divided_view_port){

		glViewport(0, 0, wWidth, wHeight);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1.5, 1.5, -1, 1, zNear, 500.0f);
		//gluPerspective(45, wWidth / (wHeight), zNear, 3000.0f);
		cameraPrincipal.setView();
		DisplayLights();
		for (size_t i = 0; i < objs.size(); i++){
			objs[i].DrawModel();
		}
		drawFPS();
	}
	else{

		//left
		glMatrixMode(GL_PROJECTION);
		glViewport(0, 0, wWidth / 2, wHeight);
		glLoadIdentity();
		glFrustum(-1.5, 1.5, -1, 1, zNear, 500.0f);
		//gluPerspective(45, wWidth / (wHeight*2), zNear, 3000.0f);
		cameraPrincipal.setView();
		DisplayLights();
		for (size_t i = 0; i < objs.size(); i++){
			objs[i].DrawModel();
		}
		drawFPS();

		//right
		glMatrixMode(GL_PROJECTION);
		glViewport(wWidth / 2, 0, wWidth, wHeight);
		glLoadIdentity();
		//glFrustum(-1.5, 1.5, -1, 1, zNear, 500.0f);
		gluPerspective(45, (wWidth) / (wHeight), zNear, 3000.0f);
		cameraDir.setView();
		DisplayLights();
		for (size_t i = 0; i < objs.size(); i++){
			objs[i].DrawModel();
		}
		cameraObj.DrawModel();
		drawFPS();
	}

	//-----------------------------------------------------------------------
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();


}

void drawFPS()
{
	//  Load the identity matrix so that FPS string being drawn
	//  won't get animates
	glLoadIdentity();

	//  Print the FPS to the window
	printw(-0.9, -0.9, 0, "FPS: %4.2f", fps);
}

void DisplayLights()
{
	glPushMatrix();
	GLfloat position[] = { L1.pontos.x, L1.pontos.y, L1.pontos.z - 1, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glPopMatrix();

	glPushMatrix();
	GLfloat position2[] = { L2.pontos.x + 5, L2.pontos.y, L2.pontos.z - 1, 1.0 };
	GLfloat color[] = { 0.05, 0.98, 0.56 };
	glLightfv(GL_LIGHT1, GL_POSITION, position2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, color);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glPopMatrix();
}

void LoadModels()
{
	//m = Model("Resources/Triangles/venus.obj");
	//objs.push_back(m);
	Model m = Model("Resources/PolygonsWithNormals/camel.obj");
	objs.push_back(m);
	objs[modelIndex].translate_y = 1;
	objs[modelIndex].translate_z = -1;
}

void printw(float x, float y, float z, char* format, ...)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	va_list args;
	int len;
	int i;
	char * text;

	va_start(args, format);

	len = _vscprintf(format, args) + 1;
	text = (char *)malloc(len * sizeof(char));
	vsprintf_s(text, len, format, args);
	va_end(args);

	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(x, y, z);
	for (i = 0; text[i] != '\0'; i++)
		glutBitmapCharacter(font_style, text[i]);
	free(text);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}



void calculateFPS()
{
	frameCount++;
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	int timeInterval = currentTime - previousTime;

	if (timeInterval > 1000)
	{
		fps = frameCount / (timeInterval / 1000.0f);
		previousTime = currentTime;
		frameCount = 0;
	}
}



void idle(void)
{
	//  Calculate FPS
	calculateFPS();
	// botoes
	if (!lightSelected){
		if (keyboard['+'] || keyboard['='])
			objs[modelIndex].scale += 0.001;
		if (keyboard['-'] || keyboard['_'])
			objs[modelIndex].scale -= 0.001;
		if (keyboard['7'])
			objs[modelIndex].rotate_x += 5;
		if (keyboard['8'])
			objs[modelIndex].rotate_y += 5;
		if (keyboard['9'])
			objs[modelIndex].rotate_z += 5;
	}
	if (keyboard['1'])
		translateModel(0, -translateQtd);
	if (keyboard['2'])
		translateModel(0, +translateQtd);
	if (keyboard['3'])
		translateModel(1, -translateQtd);
	if (keyboard['4'])
		translateModel(1, +translateQtd);
	if (keyboard['5'])
		translateModel(2, -translateQtd);
	if (keyboard['6'])
		translateModel(2, +translateQtd);
	if (keyboard[27])// ESC
		exit(0);
	if (keyboard['w'] || keyboard['W']){
		cameraPrincipal.translateGlob(0, 0, 0.01);
		cameraObj.translate_z += 0.01;
	}
	if (keyboard['s'] || keyboard['S']){
		cameraPrincipal.translateGlob(0, 0, -0.01);
		cameraObj.translate_z -= 0.01;
	}
	if (keyboard['a'] || keyboard['A']){
		cameraPrincipal.translateGlob(-0.01, 0, 0);
		cameraObj.translate_x -= 0.01;
	}
	if (keyboard['d'] || keyboard['D']){
		cameraPrincipal.translateGlob(0.01, 0, 0);
		cameraObj.translate_x += 0.01;
	}
	if (keyboard[','] || keyboard['<'])
		selectLast();
	if (keyboard['.'] || keyboard['>'])
		selectNext();
	if (keyboard['i'] || keyboard['I'])
		cameraDir.translateGlob(0, 0, 0.01);
	if (keyboard['k'] || keyboard['K'])
		cameraDir.translateGlob(0, 0, -0.01);
	if (keyboard['j'] || keyboard['J'])
		cameraDir.translateGlob(-0.01, 0, 0);
	if (keyboard['l'] || keyboard['L'])
		cameraDir.translateGlob(0.01, 0, 0);
	if (keyboard['r'] || keyboard['R']){
		cameraPrincipal.translateLoc(0, 0, 0.009f);
		zNear -= 0.009f;
	}
	if (keyboard['f'] || keyboard['F']){
		cameraPrincipal.translateLoc(0, 0, -0.009f);
		zNear += 0.009f;
	}
	glutPostRedisplay();
}

void drawGrid() {
	int gridL = 2;
	glPushMatrix();
	glTranslatef(-(gridL / 2), 0, -(gridL / 2));

	glColor3f(.3, .3, .3);

	glBegin(GL_LINES);

	for (int i = 0; i <= gridL; i++) {
		glVertex3f(i, 0, 0);
		glVertex3f(i, 0, gridL);
		glVertex3f(0, 0, i);
		glVertex3f(gridL, 0, i);
	};

	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-(gridL / 2), 0, -(gridL / 2));

	glColor3f(1, 0, 0);

	glBegin(GL_LINES);

	for (int i = 0; i <= gridL; i++) {
		glVertex3f(i, 0, 0);
		glVertex3f(i, gridL, 0);
		glVertex3f(0, i, 0);
		glVertex3f(gridL, i, 0);
	};

	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-(gridL / 2), 0, -(gridL / 2));

	glColor3f(.3, .3, .3);

	glBegin(GL_LINES);

	for (int i = 0; i <= gridL; i++) {
		glVertex3f(0, i, 0);
		glVertex3f(0, i, gridL);
		glVertex3f(0, 0, i);
		glVertex3f(0, gridL, i);
	};

	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef((gridL / 2), 0, -(gridL / 2));

	glColor3f(.3, .3, .3);

	glBegin(GL_LINES);

	for (int i = 0; i <= gridL; i++) {
		glVertex3f(0, i, 0);
		glVertex3f(0, i, gridL);
		glVertex3f(0, 0, i);
		glVertex3f(0, gridL, i);
	};

	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-(gridL / 2), 0, (gridL / 2));

	glColor3f(.3, .3, .3);

	glBegin(GL_LINES);

	for (int i = 0; i <= gridL; i++) {
		glVertex3f(i, 0, 0);
		glVertex3f(i, gridL, 0);
		glVertex3f(0, i, 0);
		glVertex3f(gridL, i, 0);
	};

	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-(gridL / 2), gridL, -(gridL / 2));

	glColor3f(.3, .3, .3);

	glBegin(GL_LINES);

	for (int i = 0; i <= gridL; i++) {
		glVertex3f(i, 0, 0);
		glVertex3f(i, 0, gridL);
		glVertex3f(0, 0, i);
		glVertex3f(gridL, 0, i);
	};

	glEnd();
	glPopMatrix();
}
void handleKeyUp(unsigned char key, int x, int y){
	if (key != 'c')
		keyboard[key] = false;
}

void handleKeypress(unsigned char key, int x, int y){
	if (key == 'c' || key == 'C') divided_view_port = !divided_view_port;
	else keyboard[key] = true;

}

void translateModel(int selector, double deslocamento)
{
	const double translateLightBoost = 0.10;
	if (selector == 0)
	{
		if (!lightSelected)
		{
			objs[modelIndex].translate_x += deslocamento;
		}
		else{
			if (lightIndex == 0)
			{
				L1.translate(deslocamento + translateLightBoost, 0, 0);
			}
			else{
				L2.translate(deslocamento + translateLightBoost, 0, 0);
			}
		}

	}
	else if (selector == 1){
		if (!lightSelected)
		{
			objs[modelIndex].translate_y += deslocamento;
		}
		else{
			if (lightIndex == 0)
			{
				L1.translate(0, deslocamento + translateLightBoost, 0);
			}
			else{
				L2.translate(0, deslocamento + translateLightBoost, 0);
			}
		}
	}
	else if (selector == 2){
		if (!lightSelected)
		{
			objs[modelIndex].translate_z += deslocamento;
		}
		else{
			if (lightIndex == 0)
			{
				L1.translate(0, 0, deslocamento + translateLightBoost);
			}
			else{
				L2.translate(0, 0, deslocamento + translateLightBoost);
			}
		}
	}
}
void selectLast()
{
	if (modelIndex > 0 && !lightSelected)
	{
		//caso 0
		modelIndex = modelIndex - 1;
		printf("Selected: %s %d \n", objs[modelIndex].nome.c_str(), modelIndex);
	}
	else
	{
		if (!lightSelected)
		{
			//caso 1
			lightSelected = 1;
			lightIndex = 1;
			printf("Selected: %s %d \n", "Light ", lightIndex);
		}
		else
		{
			if (lightIndex == 1)
			{
				//caso 2
				lightIndex = 0;
				printf("Selected: %s %d \n", "Light ", lightIndex);
			}
			else
			{
				//caso 3
				lightSelected = 0;
				modelIndex = objs.size() - 1;
				printf("Selected: %s %d \n", objs[modelIndex].nome.c_str(), modelIndex);
			}
		}
	}
}

void selectNext()
{
	if (modelIndex < objs.size() - 1 && !lightSelected)
	{
		modelIndex = modelIndex + 1;
		printf("Selected: %s %d \n", objs[modelIndex].nome.c_str(), modelIndex);
	}
	else
	{
		if (!lightSelected)
		{
			//caso 4
			lightSelected = 1;
			lightIndex = 0;
			printf("Selected: %s %d \n", "Light ", lightIndex);
		}
		else
		{
			if (lightIndex == 0)
			{
				//caso 5
				lightIndex = 1;
				printf("Selected: %s %d \n", "Light ", lightIndex);
			}
			else
			{
				//caso 6
				lightSelected = 0;
				modelIndex = 0;
				printf("Selected: %s %d \n", objs[modelIndex].nome.c_str(), modelIndex);
			}
		}
	}
}

void mouseClickFunction(int btn, int state, int x, int y){

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		if (divided_view_port){
			if (x < wWidth / 2){
				if (!clickPrincipal){
					clickPrincipal = true;
					mousepos_x = x;
					mousepos_y = y;
				}
			}
			else{
				if (!clickDiretor){
					clickDiretor = true;
					mousepos_x = x;
					mousepos_y = y;
				}
			}
		}
		else{
			if (!clickPrincipal){
				clickPrincipal = true;
				mousepos_x = x;
				mousepos_y = y;
			}
		}

	}
	else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP){

		clickPrincipal = false;

		clickDiretor = false;

	}
}

void mouseMotion(int x, int y){
	if (!mouse_right){
		if (clickPrincipal){
			cameraPrincipal.rotateGlob((x - mousepos_x)*0.2, 0, 1, 0);
			cameraPrincipal.rotateGlob((y - mousepos_y)*0.2, 1, 0, 0);
			cameraObj.rotate_y += (x - mousepos_x)*0.2;
			cameraObj.rotate_x += (y - mousepos_y)*0.2;
		}
		if (clickDiretor){
			cameraDir.rotateGlob((x - mousepos_x)*0.2, 0, 1, 0);
			cameraDir.rotateGlob((y - mousepos_y)*0.2, 1, 0, 0);
		}
		mousepos_x = x;
		mousepos_y = y;
	}
}

void myreshape(GLsizei w, GLsizei h)
{
	glMatrixMode(GL_PROJECTION);
	wWidth = w;
	wHeight = h;
	double g_Width = wWidth;
	double g_Height = wHeight;
	glViewport(0, 0, g_Width, g_Height);
	glLoadIdentity();
	glFrustum(-1.5, 1.5, -1, 1, zNear, 500.0f);
}

int main(int argc, char* argv[]){

	LoadModels();
	glutInit(&argc, argv);

	//  Request double buffered true color window with Z-buffer
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(wWidth, wHeight);
	cameraObj.scale *= 4;
	cameraObj.translate_x = -1;
	cameraObj.translate_y = 0;
	cameraObj.translate_z = 4;
	cameraDir.translateLoc(0, 0, -0.1);
	// Create window
	glutCreateWindow("Projeto PG");

	//  Enable Z-buffer depth test
	glEnable(GL_DEPTH_TEST);

	// Callback functions


	glutDisplayFunc(display);
	glutReshapeFunc(myreshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(handleKeypress);
	glutKeyboardUpFunc(handleKeyUp);

	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouseClickFunction);

	//  Pass control to GLUT for events
	glutMainLoop();

	//  Return to OS
	return 0;
}
