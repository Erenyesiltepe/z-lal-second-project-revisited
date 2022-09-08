/*
   CTIS164 - Template Source Program
----------
STUDENT : AYŞE ZÜLAL EKİCİ
SECTION : 002
HOMEWORK: II
----------
PROBLEMS: The man is not professional, he sometimes misses the target and loses his spear :(
          Jokes aside there is a problem with angle that I could not fix.
          + He sometimes gets stuck.
----------
ADDITIONAL FEATURES:
*/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  400
#define WINDOW_HEIGHT 400

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON        1 // 0:disable timer, 1:enable timer
#define TIMERSECS 100

#define D2R 0.0174532

typedef struct {
    int x, y;
} point_t;

typedef struct {
    point_t pos;
    double angle;
    double radius; // circular collision boundary
    bool hit;      // collision state
} player_t;

typedef struct {
    point_t pos;
    float angle;
    bool active;
} fire_t;

typedef struct {
    point_t center;
    float radius;
    float speed;
} target_t;

#define MAX_FIRE 20 // 20 fires at a time.
#define FIRE_RATE 8 // after 8 frames you can throw another one.

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, spacebar = false;
int  winWidth, winHeight;
int dx, dy; //player
int dir = 0;
int ax = -200000, ay = -2000000; //target
bool targetOn = false;
int aax = -165, aay = -65; //arms
int timeCounter = 60000;
fire_t fr;
int fire_rate = 0;
player_t p = { {dx - 175,dy - 65}, 0, 2 };
target_t t = { {ax, ay}, 34, 3.0 * (rand() % 100) / 100.0 + 0.5 };

int checkstop = 1;

/* Stars */
bool bgOn = false;
int bgCheck = 1;
int bgCounter = 0;
int n = 0;
typedef struct {
    point_t basic[3500];
    int bright[3500];
}stars_t;

stars_t stars;
//the stars are Kamila's humble gift to me 


// current Window width and height

//
// to draw circle, center at (x,y)
// radius r
//



void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}
void animate(int value) {
    glutTimerFunc(TIMERSECS, animate, 1);
    if (dx > 0.5) {
        dx = -0.5;
    }
    else {
        dx += 0.1;
    }
    glutPostRedisplay();
}
void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}
void vertex(point_t P, point_t Tr, double angle) {
    double xp = ((P.x + 165) * cos(angle) - (P.y + 65) * sin(angle)) + Tr.x - 165;
    double yp = ((P.x + 165) * sin(angle) + (P.y + 65) * cos(angle)) + Tr.y - 65;
    glVertex2d(xp, yp);
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

double angle() {
    double Dy = aay - ay;
    double Dx = aax - ax;
    double ang2 = atan2(Dy, Dx) / D2R;
    if (ang2 < 0)
        return ang2 += 180;
    else if (ang2 == 360)
        return 0;
    return ang2 += 180;




}
void drawTarget()
{
    glColor3f(0.4, 0.2, 0);
    circle(ax, ay, 34);

    glColor3f(1, 0, 0);
    circle(ax, ay, 30);

    glColor3f(1, 1, 1);
    circle(ax, ay, 25);

    glColor3f(1, 0, 0);
    circle(ax, ay, 20);

    glColor3f(1, 1, 1);
    circle(ax, ay, 15);

    glColor3f(1, 0, 0);
    circle(ax, ay, 10);

    glColor3f(1, 1, 1);
    circle(ax, ay, 5);
}
//tecnically drawFires
void drawPolearm()
{
    double angle = (p.angle) * D2R;
    for (int i = 0; i < MAX_FIRE; i++) {
        glColor3f(1, 1, 1);

        //the polearm/spear/lance I'm not an expert on weapon names
        glColor3f(0.4, 0.4, 0.4);
        glBegin(GL_QUADS);
        vertex({ -165, -55 }, { dx + fr.pos.x, dy + fr.pos.y }, angle);
        vertex({ -165, -51 }, { dx + fr.pos.x, dy + fr.pos.y }, angle);
        vertex({ -85, -51 }, { dx + fr.pos.x, dy + fr.pos.y }, angle);
        vertex({ -85, -55 }, { dx + fr.pos.x, dy + fr.pos.y }, angle);
        glEnd();
        //mızrak ucu
        glColor3f(1, 1, 1);
        glBegin(GL_TRIANGLES);
        vertex({ -85, -55 }, { dx + fr.pos.x, dy + fr.pos.y }, angle);
        vertex({ -85, -51 }, { dx + fr.pos.x, dy + fr.pos.y }, angle);
        vertex({ -70, -53 }, { dx + fr.pos.x, dy + fr.pos.y }, angle);
        glEnd();
    }

}
void mansMovingArms()
{
    /* p->angle += n;
     if (p->angle < 0)
         p->angle += 360;
     if (p->angle > 360)
         p->angle -= 360;*/
    double angle = (p.angle) * D2R;
    glColor3ub(90, 50, 138);
    glBegin(GL_QUADS);
    vertex({ -175, -65 }, { dx, dy }, angle);
    vertex({ -175, -55 }, { dx, dy }, angle);
    vertex({ -145, -55 }, { dx, dy }, angle);
    vertex({ -145, -65 }, { dx, dy }, angle);
    glEnd();

    glColor3f(1.11, 0.80, 0.55);
    glBegin(GL_QUADS);
    vertex({ -145, -65 }, { dx, dy }, angle);
    vertex({ -145, -55 }, { dx, dy }, angle);
    vertex({ -135, -55 }, { dx, dy }, angle);
    vertex({ -135, -65 }, { dx, dy }, angle);
    glEnd();

}

void drawHorseAndMan() {

    //horse torso
    glColor3f(0.6, 0.3, 0);
    glRectf(dx - 180, dy - 100, dx - 100, dy - 140);

    //horse legs(+toynak)
    glColor3f(0.6, 0.3, 0);
    glRectf(dx - 180, dy - 180, dx - 190, dy - 100);
    glColor3f(0, 0, 0);
    glRectf(dx - 180, dy - 190, dx - 190, dy - 180);

    glColor3f(0.6, 0.3, 0);
    glRectf(dx - 110, dy - 180, dx - 100, dy - 100);
    glColor3f(0, 0, 0);
    glRectf(dx - 110, dy - 190, dx - 100, dy - 180);

    //horse tail
    glColor3f(0, 0, 0);
    glRectf(dx - 200, dy - 175, dx - 190, dy - 110);

    //horse neck
    glColor3f(0.6, 0.3, 0);
    glRectf(dx - 125, dy - 140, dx - 100, dy - 60);

    //horse head
    glColor3f(0.6, 0.3, 0);
    glRectf(dx - 125, dy - 60, dx - 100, dy - 40);

    glColor3f(0.6, 0.3, 0);
    glBegin(GL_QUADS);
    glVertex2f(dx - 100, dy - 65);
    glVertex2f(dx - 100, dy - 40);
    glVertex2f(dx - 70, dy - 55);
    glVertex2f(dx - 70, dy - 65);
    glEnd();

    //yele
    glColor3f(0, 0, 0);
    glRectf(dx - 143, dy - 100, dx - 125, dy - 48);

    //horse ear
    glColor3f(0.6, 0.3, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(dx - 125, dy - 40);
    glVertex2f(dx - 110, dy - 20);
    glVertex2f(dx - 110, dy - 40);
    glEnd();

    //eye
    glColor3f(0, 0, 0);
    circle(dx - 100, dy - 50, 5);
    glColor3f(1, 1, 1);
    circle(dx - 98, dy - 50, 2);

    //man butt
    glColor3f(0.3, 0.3, 0.8);
    glRectf(dx - 180, dy - 100, dx - 145, dy - 90);

    //man leg
    glColor3f(0.3, 0.3, 0.8);
    glRectf(dx - 155, dy - 120, dx - 145, dy - 100);

    //man torso
    glColor3ub(90, 50, 138);
    glRectf(dx - 180, dy - 90, dx - 160, dy - 50);

    //boot
    glColor3f(0, 0, 0);
    glRectf(dx - 155, dy - 130, dx - 145, dy - 120);
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(dx - 145, dy - 130);
    glVertex2f(dx - 145, dy - 120);
    glVertex2f(dx - 135, dy - 125);
    glVertex2f(dx - 135, dy - 130);
    glEnd();

    //neck & head
    glColor3f(1.11, 0.80, 0.55);
    glRectf(dx - 175, dy - 50, dx - 165, dy - 40);
    glColor3f(1.11, 0.80, 0.55);
    circle(dx - 170, dy - 30, 15);
    glColor3f(1.11, 0.80, 0.55);
    glBegin(GL_TRIANGLES);
    glVertex2f(dx - 158, dy - 35);
    glVertex2f(dx - 158, dy - 20);
    glVertex2f(dx - 150, dy - 35);
    glEnd();
    glColor3f(1, 1, 1);
    circle(dx - 165, dy - 27, 5);
    glColor3f(0, 0, 0);
    circle(dx - 162, dy - 27, 3);

    //hat
    glColor3ub(90, 50, 138);
    glBegin(GL_TRIANGLES);
    glVertex2f(dx - 190, dy - 23);
    glVertex2f(dx - 175, dy - 3);
    glVertex2f(dx - 150, dy - 23);
    glEnd();

    //hat feather because it looks fancy
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLES);
    glVertex2f(dx - 188, dy - 15);
    glVertex2f(dx - 184, dy + 10);
    glVertex2f(dx - 180, dy - 20);
    glEnd();


}

//
// To display onto window using OpenGL commands
//

void createStars() {
    bgOn = true;
    int i;
    int maxX = winWidth / 2 - 5, maxY = winHeight / 2 - 5;
    int dens = winWidth * winHeight * 0.0003125;

    srand(time(NULL));
    for (i = 0; i < dens; i++)
    {
        stars.basic[i].x = rand() % (winWidth + 1 - 5) - maxX;
        stars.basic[i].y = rand() % (winHeight + 1 - 5) - maxY;
        stars.bright[i] = rand() % 4 + 1;
    }

}

void displayStars() {

    if (!bgOn)
        createStars();
    int i;
    int dens = winWidth * winHeight * 0.0003125;
    for (i = 0; i < dens; i++)
    {
        glColor3f(1, 1, 1);
        glRectf(stars.basic[i].x, stars.basic[i].y, stars.basic[i].x + stars.bright[i], stars.basic[i].y + stars.bright[i]);
    }
}

int i = 0;
void display() {
    //
    // clear window to white
    //
    int n = 0;
    player_t p{};
    glClearColor(0.7f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
    glColor3f(1, 0.5, 0);
    glVertex2f(-1000, -500);
    glVertex2f(1000, -500);
    glColor3f(0.3, 0, 1.0);
    glVertex2f(1000, 1000);
    glVertex2f(-1000, 1000);
    glEnd();

    displayStars();

    //SUN agresif
    glColor3ub(255, 107, 38);
    circle(0, -200, 150);

    
    printf("try %d\n",i++);

    //ÇİMEN bu da agresif
    glBegin(GL_QUADS);
    glColor3f(0.2, 0.2, 0.2);
    glVertex2f(-1000, -500);
    glVertex2f(1000, -500);

    glColor3f(0.21, 0.55, 0.05);
    glVertex2f(1000, -185);
    glVertex2f(-1000, -185);
    glEnd();



    glColor3f(1, 1, 0);
    vprint(-620, 300, GLUT_BITMAP_8_BY_13, "Ayse Zulal Ekici");
    vprint(-610, 280, GLUT_BITMAP_8_BY_13, "22103565");
    vprint(-590, 65, GLUT_BITMAP_8_BY_13, "Use arrow keys to manage your position and the man will handle the rest. Click to summon the target. If it gets stuck summon the target again.");
    

    drawHorseAndMan();
    drawPolearm();
    mansMovingArms();
    drawTarget();



    /*glColor3b(0, 0, 1);
    circle(ax, ay, 3);

    glColor3b(0, 1, 1);
    circle(dx, dy, 3);
    */

    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void resetTarget() {
    float yPos = rand() % 2 ? 225 : -225;
    t.center = { ax, ay };
    t.radius = 34;
    t.speed = 3.0 * (rand() % 100) / 100.0 + 0.5;
}
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ')
        spacebar = true;
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    if (key == ' ')
        spacebar = false;
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true;
        if (dy < -20) {
            aay += 20;
            dy += 20;
            dir = 3;
        }
        break;
    case GLUT_KEY_DOWN: down = true;
        aay -= 20;
        dy -= 20;
        dir = 4;
        break;
    case GLUT_KEY_LEFT: left = true;
        aax -= 20;
        dx -= 20;
        dir = 2;
        break;
    case GLUT_KEY_RIGHT: right = true;
        aax += 20;
        dx += 20;
        dir = 1;
        break;
    }


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP:
        up = false;
        break;
    case GLUT_KEY_DOWN:
        down = false;
        break;
    case GLUT_KEY_LEFT:
        left = false;
        break;
    case GLUT_KEY_RIGHT:
        right = false;
        break;
    }


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.
    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
        //map from GLUT coordinate system to OpenGL Coordinate System
        targetOn = true;
        ax = x - winWidth / 2;
        ay = winHeight / 2 - y;
        drawTarget();
    }
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}
bool testCollision(fire_t fr, target_t t) {
    float dx = t.center.x - fr.pos.x;
    float dy = t.center.y - fr.pos.y;
    float d = sqrt(dx * dx + dy * dy);
    return d <= t.radius;
}

void turn(player_t* p, double turn) {
    p->angle += turn;
}

void fireInit() {
    fr.pos = { dx, dy };
    fr.angle = p.angle + 90;
}

#if TIMER_ON == 1
void onTimer(int v) {

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.


    if (((winWidth > 400 || winHeight > 400) && bgCheck == 1) || bgCounter == 126)
    {
        bgCounter = 0;
        bgOn = false;
        bgCheck = 0;
    }
    bgCounter++;
    if (timeCounter > 0) {
        if (targetOn) {
            double ang2 = angle();


            //printf("%d %.2f\n", int(p.angle) % 360 + 360, ang2);
        
            if (int(p.angle) % 360 + 360 != int(ang2))
            {
                if (ang2 > 180)
                    turn(&p, -1);
            }
            if (int(p.angle) % 360 != int(ang2))
            {
                if (ang2 <= 180)
                    turn(&p, 1);
            }

            else {
                if (!fr.active && checkstop)
                {
                    fireInit();
                    fr.active = true;
                    checkstop = 0;
                }
                if (fr.active)
                {
                    fr.pos.x += 4 * cos(ang2 * D2R);
                    fr.pos.y += 4 * sin(ang2 * D2R);


                    if (dx + fr.pos.x - 165 < ax + 34 && dx + fr.pos.x - 165 > ax - 34 && dy + fr.pos.y - 65 < ay + 34 && dy + fr.pos.y - 65 > ay - 34)
                    {
                        fr.pos.x = 0;
                        fr.pos.y = 0;
                        fr.active = false;
                        checkstop = 1;

                        ax = rand() % 800 - 400;
                        ay = rand() % 800 - 400;

                    }
                }
            }
        }
        // to refresh the window it calls display() function
        glutPostRedisplay(); // display()


    }
}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    resetTarget();
}

void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("The Flying Karadenizli!");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);

#endif

    Init();

    glutMainLoop();
}