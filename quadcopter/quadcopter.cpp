#include <GL\glut.h>
#define DEG2RAD 3.14159/180.0
#include <math.h>

/*Fonksiyon prototipleri*/
void init(void);/*baslangic ayarlari*/
void keyboard(unsigned char key, int x, int y);/*klavye etkilesimi ile tetiklenen fonksiyon*/
void keyUp(unsigned char key, int x, int y);/*klavye tuslarindan biri up duruma gecince tetiklenen fonksiyon*/
void mouse(int button, int state, int x, int y);
void display();/*goruntulenecek sekilleri cizen fonksiyon*/
void reshape(int width, int height);/*pencere boyutu degigince tetiklenen fonksiyon*/
void drawTape(void);/*siyah bir serit cizen fonksiyon*/
void drawStick(void);/*siyah seritlerden bir cubuk olusturan fonksiyon*/
void drawSkeleton(void);/*siyah cubuklardan bir quadrocopter iskeleti olusturan fonksiyon*/
void drawCircle(float radiusX, float radiusY, float radiusZ);/*daire cizen fonksiyon*/
void drawCylinder(void);/*daire cizen fonksiyonu kullanarak silindir cizen fonksiyon*/
void drawHead(void);/*ucgen prizma cizen fonksiyon*/
void drawWheel(float radiusX, float radiusY, float radiusZ);/*icinde jantlari olan halka silindir cizen fonksiyon*/
void drawRotor(void);/*drawWheel fonksiyonunu kullanarak perfane oluþturan fonksiyon*/
void placeRotors(void);/*olusturulan pervaneleri cubuklarin ucuna yerlestiren fonksiyon*/
void turnRotors(int id);/*pervaneleri donduren timer fonksiyonu*/
void setCam(int camera);/*kamera konumunu dortdonere gore ayarlayan fonksiyon*/
void look(void);/*kameranin bakisini belirleyen fonksiyon*/
void drawCoordinates(void);/*X,Y ve Z koordinatlarini renkleriyle cizen fonksiyon*/

/*hiz derecelerini temsil eder*/
enum SPEED
{
	VERY_SLOW = 7, SLOW = 15, NORMAL = 20, FAST = 25, VERY_FAST = 30
};

/*X, Y ve Z koordinatlarini temsil eder*/
enum AXES
{
	X = 0, Y = 1, Z = 2
};

/*etkilesimde bulunulan klavye tuslarini temsil eder*/
enum KEYS
{
	LEFT, RIGHT, UP, DOWN, RISE, DESCEND, FLY
};

/*pervaneleri temsil eder*/
enum ROTORS
{
	LEFT_FRONT_ROTOR, LEFT_BACK_ROTOR, RIGHT_BACK_ROTOR, RIGHT_FRONT_ROTOR
};

enum CAMERAS
{
	FRONT_CAM, BACK_CAM, LEFT_CAM, RIGHT_CAM
};

int fanSpeed = NORMAL;/*pervanelerin es zamanli donerken kullandiklari hiz degiskeni*/
int fanAngle[4] = { 0 };/*pervanelerin kendilerine ait donus acilari*/
int camera = BACK_CAM;/*kullanici etkilesimi ile kameranin pozisyonu degistirmek icin kullanilan degisken*/
float camPos[] = { 0.0, 0.0, 0.0 };/*kamera pozisyonunu saklar*/
float quadroPos[] = { 0.0, 0.0, 0.0 };/*dortdonerin pozisyonunu saklar*/
float quadroSlope[] = { 0.0, 0.0, 0.0, 0.0 };/*dortdoner hareket ederken aldigi egimi saklar*/ /*HENUZ GELISTIRME ASAMASINDA*/
bool keys[7] {false};/*etkilesimde bulunulan klavye tuslarinin basili olup olmadigini tutar*/

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(850, 650);
	glutInitWindowPosition(400, 150);
	glutCreateWindow("QUADCOPTER");

	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyUp);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}

void init(void){
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glShadeModel(GL_SMOOTH);
}

void display(){
	if (keys[FLY]){/*FLY seceneginin acik olup olmadigini(yani space tusuna bir kez basip acildigini veya bir kez basip kapandigini) kontrol eder*/
		glutTimerFunc(0, turnRotors, 0);/*timer fonksiyonu pervaneleri FLY secenegi on oldukca dondurur*/
		look();/*kamera dortdonerden hic ayrilmaz*/
	}
	glClear(GL_COLOR_BUFFER_BIT);
	drawCoordinates();/*X, Y ve Z koordinatlarini renkleriyle cizer*/
	glPushMatrix();
	glTranslatef((GLfloat)quadroPos[X], (GLfloat)quadroPos[Y], (GLfloat)quadroPos[Z]);
	drawSkeleton();/*dortdonerin iskeletini cizer*/
	drawCylinder();/*dortdonerin ortasýndaki silindiri cizer*/
	drawHead();/*dortonerin on tarafini gosteren ucgeni cizer*/
	placeRotors();/*dortdonerin kanatlarini cizer*/
	glPopMatrix();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case ' ':/*space tusuna basildiginda ucus moduna gecilir, tekrar basildiginda kapanir*/
		if (keys[FLY]){
			keys[FLY] = false;
		}
		else{
			keys[FLY] = true;
		}
		glutPostRedisplay();
		break;
	case '1':
		fanSpeed = VERY_SLOW;/*pervanelerin donus hizi*/
		glutPostRedisplay();
		break;
	case '2':
		fanSpeed = SLOW;/*pervanelerin donus hizi*/
		glutPostRedisplay();
		break;
	case '3':
		fanSpeed = NORMAL;/*pervanelerin donus hizi*/
		glutPostRedisplay();
		break;
	case '4':
		fanSpeed = FAST;/*pervanelerin donus hizi*/
		glutPostRedisplay();
		break;
	case '5':
		fanSpeed = VERY_FAST;/*pervanelerin donus hizi*/
		glutPostRedisplay();
		break;
	case 'c':
	case 'C':
		if (camera == BACK_CAM){/*kameranin pozisyonu degistirildi*/
			camera = FRONT_CAM;
		}
		else if (camera == FRONT_CAM){/*kameranin pozisyonu degistirildi*/
			camera = LEFT_CAM;
		}
		else if (camera == LEFT_CAM){/*kameranin pozisyonu degistirildi*/
			camera = RIGHT_CAM;
		}
		else if (camera == RIGHT_CAM){/*kameranin pozisyonu degistirildi*/
			camera = BACK_CAM;
		}
		setCam(camera);/*kameranin pozisyonu ayarlandi*/
		glutPostRedisplay();
		break;
	case '+':
		if (keys[RISE]){/*dortdonerin Y eksenince yukselmesini saglar*/
			fanSpeed = VERY_FAST;
			quadroPos[Y] += 0.1;
			look();
			glutPostRedisplay();
		}
		keys[RISE] = true;
		break;
	case '-':
		if (keys[DESCEND]){/*dortdonerin Y eksenince alcalmasini saglar*/
			fanSpeed = VERY_SLOW;
			quadroPos[Y] -= 0.1;
			look();
			glutPostRedisplay();
		}
		keys[DESCEND] = true;
		break;
	case 'w':/*dortdonerin ileri gitmesini saglar*/
	case 'W':
		if (keys[UP]){
			keys[FLY] = false;
			if (keys[DOWN]){/*dortdonerin hem ileri hem geri gitmemesini saglar*/
				break;
			}
			if (keys[LEFT]){/*dortdonerin ileri ve sola gitmesini saglar*/
				quadroPos[X] += 0.1;
				fanAngle[LEFT_FRONT_ROTOR] += VERY_SLOW;
				fanAngle[LEFT_BACK_ROTOR] += SLOW;
				fanAngle[RIGHT_BACK_ROTOR] += VERY_FAST;
				fanAngle[RIGHT_FRONT_ROTOR] += SLOW;
			}
			if (keys[RIGHT]){/*dortdonerin ileri ve saga gitmesini saglar*/
				quadroPos[Z] += 0.1;
				fanAngle[LEFT_FRONT_ROTOR] += SLOW;
				fanAngle[LEFT_BACK_ROTOR] += VERY_FAST;
				fanAngle[RIGHT_BACK_ROTOR] += SLOW;
				fanAngle[RIGHT_FRONT_ROTOR] += VERY_SLOW;
			}
			if (keys[LEFT] == false && keys[RIGHT] == false && keys[DOWN] == false){/*dortdonerin yalnizca ileri gitmesini saglar*/
				quadroPos[Z] += 0.1;
				quadroPos[X] += 0.1;
				fanAngle[LEFT_FRONT_ROTOR] += VERY_SLOW;
				fanAngle[LEFT_BACK_ROTOR] += VERY_FAST;
				fanAngle[RIGHT_BACK_ROTOR] += VERY_FAST;
				fanAngle[RIGHT_FRONT_ROTOR] += VERY_SLOW;
			}
			look();
			glutPostRedisplay();
		}
		keys[UP] = true;
		break;
	case 's':/*dortdonerin ileri gitmesini saglar*/
	case 'S':
		if (keys[DOWN]){
			keys[FLY] = false;
			if (keys[UP]){/*dortdonerin hem ileri hem geri gitmemesini saglar*/
				break;
			}
			if (keys[LEFT]){/*dortdonerin geri ve sola gitmesini saglar*/
				quadroPos[Z] -= 0.1;
				fanAngle[LEFT_FRONT_ROTOR] += SLOW;
				fanAngle[LEFT_BACK_ROTOR] += VERY_SLOW;
				fanAngle[RIGHT_BACK_ROTOR] += SLOW;
				fanAngle[RIGHT_FRONT_ROTOR] += VERY_FAST;
			}
			if (keys[RIGHT]){/*dortdonerin geri ve saga gitmesini saglar*/
				quadroPos[X] -= 0.1;
				fanAngle[LEFT_FRONT_ROTOR] += VERY_FAST;
				fanAngle[LEFT_BACK_ROTOR] += SLOW;
				fanAngle[RIGHT_BACK_ROTOR] += VERY_SLOW;
				fanAngle[RIGHT_FRONT_ROTOR] += SLOW;
			}
			if (keys[LEFT] == false && keys[RIGHT] == false && keys[UP] == false){/*dortdonerin yalnizca geri gitmesini saglar*/
				quadroPos[Z] -= 0.1;
				quadroPos[X] -= 0.1;
				fanAngle[LEFT_FRONT_ROTOR] += VERY_FAST;
				fanAngle[LEFT_BACK_ROTOR] += VERY_SLOW;
				fanAngle[RIGHT_BACK_ROTOR] += VERY_SLOW;
				fanAngle[RIGHT_FRONT_ROTOR] += VERY_FAST;
			}
			look();
			glutPostRedisplay();
		}
		keys[DOWN] = true;
		break;
	case 'd':/*dortdonerin saga gitmesini saglar*/
	case 'D':
		if (keys[RIGHT]){
			keys[FLY] = false;
			if (keys[LEFT]){/*dortdonerin hem saga hem sola gitmemesini saglar*/
				break;
			}
			if (keys[UP]){/*dortdonerin ileri ve saga gitmesini saglar*/
				quadroPos[Z] += 0.1;
				fanAngle[LEFT_FRONT_ROTOR] += SLOW;
				fanAngle[LEFT_BACK_ROTOR] += VERY_FAST;
				fanAngle[RIGHT_BACK_ROTOR] += SLOW;
				fanAngle[RIGHT_FRONT_ROTOR] += VERY_SLOW;
			}
			if (keys[DOWN]){/*dortdonerin geri ve saga gitmesini saglar*/
				quadroPos[X] -= 0.1;
				fanAngle[LEFT_FRONT_ROTOR] += VERY_FAST;
				fanAngle[LEFT_BACK_ROTOR] += VERY_SLOW;
				fanAngle[RIGHT_BACK_ROTOR] += SLOW;
				fanAngle[RIGHT_FRONT_ROTOR] += SLOW;
			}
			if (keys[LEFT] == false && keys[UP] == false && keys[DOWN] == false){/*dortdonerin yalnizca saga gitmesini saglar*/
				quadroPos[X] -= 0.1;
				quadroPos[Z] += 0.1;
				fanAngle[LEFT_FRONT_ROTOR] += VERY_FAST;
				fanAngle[LEFT_BACK_ROTOR] += VERY_FAST;
				fanAngle[RIGHT_BACK_ROTOR] += VERY_SLOW;
				fanAngle[RIGHT_FRONT_ROTOR] += VERY_SLOW;
			}
			look();
			glutPostRedisplay();
		}
		keys[RIGHT] = true;
		break;
	case 'a':/*dortdonerin sola gitmesini saglar*/
	case 'A':
		if (keys[LEFT]){
			keys[FLY] = false;
			if (keys[RIGHT]){/*dortdonerin hem saga hem sola gitmemesini saglar*/
				break;
			}
			if (keys[UP]){/*dortdonerin ileri ve sola gitmesini saglar*/
				quadroPos[X] += 0.1;
				fanAngle[LEFT_FRONT_ROTOR] += VERY_SLOW;
				fanAngle[LEFT_BACK_ROTOR] += SLOW;
				fanAngle[RIGHT_BACK_ROTOR] += VERY_FAST;
				fanAngle[RIGHT_FRONT_ROTOR] += SLOW;
			}
			if (keys[DOWN]){/*dortdonerin geri ve sola gitmesini saglar*/
				quadroPos[Z] -= 0.1;
				fanAngle[LEFT_FRONT_ROTOR] += SLOW;
				fanAngle[LEFT_BACK_ROTOR] += VERY_SLOW;
				fanAngle[RIGHT_BACK_ROTOR] += SLOW;
				fanAngle[RIGHT_FRONT_ROTOR] += VERY_FAST;
			}
			if (keys[UP] == false && keys[RIGHT] == false && keys[DOWN] == false){/*dortdonerin yalnizca sola gitmesini saglar*/
				quadroPos[X] += 0.1;
				quadroPos[Z] -= 0.1;
				fanAngle[LEFT_FRONT_ROTOR] += VERY_SLOW;
				fanAngle[LEFT_BACK_ROTOR] += VERY_SLOW;
				fanAngle[RIGHT_BACK_ROTOR] += VERY_FAST;
				fanAngle[RIGHT_FRONT_ROTOR] += VERY_FAST;
			}
			look();
			glutPostRedisplay();
		}
		keys[LEFT] = true;
		break;
	default:
		break;
	}
}

void keyUp(unsigned char key, int x, int y){/*tus ile etkilesim sona erdiginde tetiklenir*/
	switch (key) {
	case 'w':
	case 'W':
		keys[FLY] = true;
		keys[UP] = false;
		glutPostRedisplay();
		break;
	case 's':
	case 'S':
		keys[FLY] = true;
		keys[DOWN] = false;
		glutPostRedisplay();
		break;
	case '+':
		fanSpeed = NORMAL;
		glutPostRedisplay();
		keys[RISE] = false;
		break;
	case '-':
		fanSpeed = NORMAL;
		glutPostRedisplay();
		keys[DESCEND] = false;
		break;
	case 'd':
	case 'D':
		keys[FLY] = true;
		keys[RIGHT] = false;
		glutPostRedisplay();
		break;
	case 'a':
	case 'A':
		keys[FLY] = true;
		keys[LEFT] = false;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y){/*HENUZ GELISTIRME ASAMASINDA*/

}

void reshape(int width, int height){/*ekran boyutu degistiginde tetiklenir*/
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(65.0, (GLfloat)width / (GLfloat)height, 1.0, 500.0);
	look();
}

void drawTape(void){/*bir serit cizer*/
	glBegin(GL_QUADS);
	glVertex3f(-2.5, -0.2, -0.2);
	glVertex3f(-2.5, -0.2, 0.2);
	glVertex3f(2.5, -0.2, 0.2);
	glVertex3f(2.5, -0.2, -0.2);
	glEnd();
}

void drawStick(void){/*cizilen seriti kullanarak cubuk olusturur*/
	drawTape();
	glRotatef(90.0, 1.0, 0.0, 0.0);
	drawTape();
	glRotatef(90.0, 1.0, 0.0, 0.0);
	drawTape();
	glRotatef(90.0, 1.0, 0.0, 0.0);
	drawTape();
}

void drawSkeleton(void){/*olusturulan cubuklari kullanarak dortdoner iskeleti olusturur*/
	glColor3f(0.0, 0.0, 0.0);
	glPushMatrix();
	glTranslatef(2.5, 0.0, 0.0);
	drawStick();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-2.5, 0.0, 0.0);
	drawStick();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, 0.0, 2.5);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	drawStick();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, 0.0, -2.5);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	drawStick();
	glPopMatrix();
}

void drawCircle(float radiusX, float radiusY, float radiusZ){/*bir daire cizer*/
	int i;
	float rad;
	glBegin(GL_POLYGON);
	for (i = 0; i<360; i++) {
		rad = i*DEG2RAD;
		glVertex3f(cos(rad)*(float)radiusX, radiusY, sin(rad)*(float)radiusZ);
	}
	glEnd();
}

void drawCylinder(void){/*cizilen daireyi y ekseninde uzatarak silindir olusturur*/
	float y = -0.5;

	glColor3f(0.1, 0.5, 1.0);
	drawCircle(0.8, -0.5, 0.8);
	y += 0.01;

	glColor3f(0.9, 0.8, 0.7);
	while (y < 0.5){
		drawCircle(1.2, y, 1.2); 
		y += 0.01;
	}

	glColor3f(0.1, 0.5, 1.0);
	drawCircle(0.8, 0.5, 0.8);
}

void drawHead(void){/*ucgen prizma seklinde bir yon belirleyici olusturur*/
	float y = -0.2;


	glColor3f(0.9, 0.1, 0.2);
	while (y < 0.21){
		y += 0.01;
		glBegin(GL_TRIANGLES);
		glVertex3f(0.56, y, 0.26);
		glVertex3f(0.26, y, 0.56);
		glVertex3f(2.0, y, 2.0);
		glEnd();
	}
}

void drawWheel(float radiusX, float radiusY, float radiusZ){/*icinde jantlari olan halka silindir cizen fonksiyon*/
	int i, j = 0;
	float rad;
	glBegin(GL_QUAD_STRIP);
	for (i = 0, j = 0; i < 360, j < 6; i++, (i % 60 == 0) ? j++ : j = j) {/*j burada jant sayisini belirleyen faktordur. ayrica i % 60, 60 derecede bir jant cizilmesini saglar */
		rad = i*DEG2RAD;
		if (i % 2 == 0){/*QUAD_STRIP özelliginden dolayi V0,V2,V4,V6,... kenarlarini secmek gerekir*/
			glColor3f(0.0, 0.0, 0.0);
			glVertex3f(cos(rad)*(float)radiusX, radiusY, sin(rad)*(float)radiusZ);
		}
		else if (i >(j * 60 - 20) && i < (j * 60 + 20)){ /*her 60 derecede bir genisligi 40 derece olan bir jant cizilir */
			glColor3f(0.3, 0.0, 0.3);/*jantlarin renginin farkli olmasini saglar*/
			glVertex3f(0.0, radiusY + 0.01, 0.0);
		}
		else{/*QUAD_STRIP özelliginden dolayi V1,V3,V5,V7,... kenarlarini secmek gerekir*/
			glColor3f(0.0, 0.0, 0.0);
			glVertex3f(cos(rad)*((float)radiusX - 0.3), radiusY, sin(rad)*((float)radiusZ - 0.3));
		}
	}
	glEnd();
}

void drawRotor(void){/*tekerlek seklini olusturan drawWheel fonksiyonunu kullanarak 3 boyutlu bir pervane olusturur*/
	float y = 0.4;
		while (y < 0.5){
			drawWheel(1.5, y, 1.5);
			y += 0.01;
		}
}

void placeRotors(void){/*pervaneleri cubuklarin uclarina yerlestirir, boylece dortdoner olusmaya baslar*/
	glColor3f(0.0, 0.0, 0.0);
	glPushMatrix();
	glTranslatef(4.5, 0.0, 0.0);
	glRotatef(-(GLfloat)(fanAngle[LEFT_FRONT_ROTOR]), 0.0, 1.0, 0.0);/*sol on pervane*/
	drawRotor();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, 0.0, -4.5);
	glRotatef(-(GLfloat)(fanAngle[LEFT_BACK_ROTOR]), 0.0, 1.0, 0.0);/*sol arka pervane*/
	drawRotor();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-4.5, 0.0, 0.0);
	glRotatef(-(GLfloat)(fanAngle[RIGHT_BACK_ROTOR]), 0.0, 1.0, 0.0);/*sag arka pervane*/
	drawRotor();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0, 0.0, 4.5);
	glRotatef(-(GLfloat)(fanAngle[RIGHT_FRONT_ROTOR]), 0.0, 1.0, 0.0);/*sag on pervane*/
	drawRotor();
	glPopMatrix();
}

void turnRotors(int id){/*timer fonksiyonu, pervanelerin acilarini fanSpeed hizina gore degistirerek donuyormus goruntusu vermeye yarar*/
	switch (fanSpeed){
	case VERY_SLOW:
		for (int i = 0; i < 4; i++)
			fanAngle[i] = (fanAngle[i] + VERY_SLOW) % 360;
		break;
	case SLOW:
		for (int i = 0; i < 4; i++)
			fanAngle[i] = (fanAngle[i] + SLOW) % 360;
		break;
	case NORMAL:
		for (int i = 0; i < 4; i++)
			fanAngle[i] = (fanAngle[i] + NORMAL) % 360;
		break;
	case FAST:
		for (int i = 0; i < 4; i++)
			fanAngle[i] = (fanAngle[i] + FAST) % 360;
		break;
	case VERY_FAST:
		for (int i = 0; i < 4; i++)
			fanAngle[i] = (fanAngle[i] + VERY_FAST) % 360;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void setCam(int position){/*kameranin konumunu belirli yerlere set eder*/
	switch (position)
	{
	case BACK_CAM:
		camPos[X] = quadroPos[X] + -7.0;
		camPos[Y] = quadroPos[Y] + 9.0;
		camPos[Z] = quadroPos[Z] + -7.0;
		break;
	case FRONT_CAM:
		camPos[X] = quadroPos[X] + 7.0;
		camPos[Y] = quadroPos[Y] + 9.0;
		camPos[Z] = quadroPos[Z] + 7.0;
		break;
	case LEFT_CAM:
		camPos[X] = quadroPos[X] + 7.0;
		camPos[Y] = quadroPos[Y] + 9.0;
		camPos[Z] = quadroPos[Z] + -7.0;
		break;
	case RIGHT_CAM:
		camPos[X] = quadroPos[X] + -7.0;
		camPos[Y] = quadroPos[Y] + 9.0;
		camPos[Z] = quadroPos[Z] + 7.0;
	default:
		break;
	}
}

void look(void){/*kameranin bakisini belirler, ayni zamanda once setCam fonksiyonunu cagirarak kamerayi belirlenmis konuma yerlestirir*/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	setCam(camera);
	gluLookAt(camPos[X], camPos[Y], camPos[Z], quadroPos[X], quadroPos[Y], quadroPos[Z], 0.0, 1.0, 0.0);/*ýp vector +y ekseni olarak secildi*/
	glutPostRedisplay();
}

void drawCoordinates(void){
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);//X ekseni, kirmizi
	glVertex3f(0, 0, 0);
	glVertex3f(500, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(-500, 0, 0);

	glColor3f(0.0, 1.0, 0.0);//Y ekseni, yesil
	glVertex3f(0, 0, 0);
	glVertex3f(0, 500, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, -500, 0);

	glColor3f(0.0, 0.0, 1.0);//Z ekseni, mavi
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 500);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, -500);
	glEnd();
}