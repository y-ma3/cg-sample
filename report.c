#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>

#define	imageWidth 512
#define	imageHeight 256

unsigned char texImage[imageHeight][imageWidth][3];

int samplingTime = 50;
static int direction[10][2];
static float position[10][2];

unsigned char	mouseFlag = GL_FALSE;
int				xStart, yStart;
double			xAngle = 0.0, yAngle = 0.0;
int counter = 1;

void initializeDirectionAndPosition() {
    srand(time(NULL));

    for (int i = 0; i < 10; i++) {
        direction[i][0] = rand() % 3 - 1;
        direction[i][1] = rand() % 3 - 1;
    }

    for (int i = 0; i < 10; i++) {
        position[i][0] = (float)(rand() % 20 - 10);
        position[i][1] = (float)(rand() % 20 - 10);
    }
}

void mySetlight()
{
    float light_position[] = { 0.0, 3.0, -10.0, 1.0 };
    float light_ambient[] = { 0.3, 0.4, 0.2, 1.0 };
    float light_diffuse[] = { 0.6, 0.5, 0.3, 1.0 };
    float light_specular[] = { 1.0, 0.0, 0.5, 1.0 };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHT0);
}

void myTimer(int value) {
    if (value == 1 && counter == 1) {
        glutTimerFunc(samplingTime, myTimer, 1);

        for (int i = 0; i < 10; i++) {
            position[i][0] += direction[i][0] * 0.1;
            position[i][1] += direction[i][1] * 0.1;
            if (position[i][0] > 10.0 || position[i][0] < -10.0 || position[i][1] > 10.0 || position[i][1] < -10.0) {
                direction[i][0] = rand() % 3 - 1;
                direction[i][1] = rand() % 3 - 1;
            }
        }

        glutPostRedisplay();
    }
}

void readPPMImage(char* filename)
{
    FILE* fp;
    int  ch, i;

    if (fopen_s(&fp, filename, "r") != 0) {
        fprintf(stderr, "Cannot open ppm file %s\n", filename);
        exit(1);
    }
    fread(texImage, 1, imageWidth * imageHeight * 3, fp);
    fclose(fp);
}

void setUpTexture1(void)
{
    readPPMImage("basket-court.ppm");
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
        0, GL_RGB, GL_UNSIGNED_BYTE, texImage);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
}
void setUpTexture2(void)
{
    readPPMImage("uniform.ppm");
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
        0, GL_RGB, GL_UNSIGNED_BYTE, texImage);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
}

void basketCourtDisplay() {
    setUpTexture1();
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
        glScaled(28, 0, 20);
        glutSolidCube(1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void humanDisplay() {
    for (int i = 0; i < 10; i++) {
        glPushMatrix();
        glTranslated(position[i][0], 2.0, position[i][1]);
        glScaled(0.6, 0.5, 0.7);
        GLfloat skinColor[] = { 1.0, 0.8, 0.6, 1.0 };
        glMaterialfv(GL_FRONT, GL_DIFFUSE, skinColor);
        glutSolidSphere(1.0, 20, 20);
        setUpTexture2();
        glEnable(GL_TEXTURE_2D);
        glTranslated(0.0, -3.0, 0.0);
        glRotated(-90.0, 1.0, 0.0, 0.0);
        GLfloat coneColor[] = { 0.6, 0.8, 1.0, 1.0 };
        glMaterialfv(GL_FRONT, GL_DIFFUSE, coneColor);
        glutSolidCone(1.0, 3.0, 20, 20);
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
    }
}

void myDisplay(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    basketCourtDisplay();
    humanDisplay();

    glutSwapBuffers();
}

void myReshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)width / (double)height, 0.1, 50.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 10.0, -25.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void myMouseMotion(int x, int y)
{
    int		xdis, ydis;
    double	a = 0.5;

    if (mouseFlag == GL_FALSE) return;
    xdis = x - xStart;
    ydis = y - yStart;
    xAngle += (double)ydis * a;
    yAngle += (double)xdis * a;

    xStart = x;
    yStart = y;
    glutPostRedisplay();
}

void myMouseFunc(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        xStart = x;
        yStart = y;
        mouseFlag = GL_TRUE;
    }
    else {
        mouseFlag = GL_FALSE;
    }
}

void getValueFromMenu(int value)
{
    switch (value) {
    case 1:
        counter = 1;
        myTimer(1);
        break;
    case 2:
        counter = 2;
        break;
    default:
        break;
    }
}

void mySetMenu()
{
    glutCreateMenu(getValueFromMenu);
    glutAddMenuEntry("SPEED UP", 1);
    glutAddMenuEntry("STOP", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void myInit(char* progname) {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1000, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow(progname);
    glClearColor(0.6, 0.8, 1.0, 1.0);
    initializeDirectionAndPosition();
    glEnable(GL_LIGHTING);
    mySetlight();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    myInit(argv[0]);
    mySetMenu();
    mySetlight();
    glutMouseFunc(myMouseFunc);
    glutMotionFunc(myMouseMotion);
    glutReshapeFunc(myReshape);
    glutTimerFunc(samplingTime, myTimer, 1);
    glutDisplayFunc(myDisplay);
    glutMainLoop();
    return 0;
}
