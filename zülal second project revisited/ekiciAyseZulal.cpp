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
- a game have born
-level,health systems added
-start,game,end page added
-we can enter our name and see previous players' scores on start page(printed in a txt file)
-targets can fire multiple fires ...
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

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON        1 // 0:disable timer, 1:enable timer
//#define TIMERSECS 100

#define D2R 0.0174532

int movex = 170, movey = 60;//sets correct origin for the vertex function

typedef struct {
    double x, y;
} point_t;

typedef struct {
    point_t pos;
    double angle;
} player_t;

typedef struct {
    point_t pos;
    double angle;
    int active;
    float speed;
} fire_t;

typedef struct {
    point_t center;
    int timer;
    float radius;
} target_t;

typedef struct {
    int bulx, buly;
    bool set;
    double angle;
}bullet_t;

typedef struct {
    point_t start,end;
    bool active;
}hook_t;

typedef struct {
    char name[30];
    int score;
}gamer_t;

gamer_t gamer[20];
int gamerNum = 0;

gamer_t currentGamer = {"\0",0};
bool printFlag = true; //prevent from printing multiple records

FILE* fr;
FILE* fa = fopen("champions.txt", "a");

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false, spacebar = false;
int  winWidth, winHeight;

#define MAX_FIRE 20 // 20 fires at a time.

#define FIRE_SPEED 4
#define POLEARM_SPEED 16
#define POLEARM_SPEED_RETURN -12

int aax = -165, aay = -65; //arms

player_t p = { { - 175,- 65}, 0 };
fire_t polearm = { p.pos,p.angle,0,POLEARM_SPEED };
hook_t hook = { p.pos, polearm.pos,false };

target_t tars[20];
int activeNum = 2;// num of targetss to be displayed
bool levelCheck = false;

int gameMode = 0;//entrance page:0, game page:1, end page 2

bullet_t bullets[20];

bool hookVis = false;

int health = 100,score=0;


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

int findAvailableFire() {
    for (int i = 0; i < MAX_FIRE; i++) {
        if (bullets[i].set == false) return i;
    }
    return -1;
}

void targetfire(target_t *tar) {
    int bul=findAvailableFire();
    bullets[bul].bulx = tar->center.x;
    bullets[bul].buly = tar->center.y;
    bullets[bul].set = true;
    bullets[bul].angle = atan2(tar->center.y - (p.pos.y-60), tar->center.x - (p.pos.x-185));
    tar->timer = rand() % 240 + 60;
}

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
    
    double xp = ((P.x + movex) * cos(angle) - (P.y + movey) * sin(angle)) + Tr.x - movex;
    double yp = ((P.x + movex) * sin(angle) + (P.y + movey) * cos(angle)) + Tr.y - movey;
    glVertex2d(xp, yp);
}


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


void drawTarget(target_t tar)
{

    int ax = tar.center.x;
    int ay = tar.center.y;

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


        glColor3f(1, 1, 1);
        double armup = -7;
        //the polearm/spear/lance I'm not an expert on weapon names
        glColor3f(0.4, 0.4, 0.4);
        glBegin(GL_QUADS);
        vertex({ -165, -55+ armup }, {polearm.pos.x,polearm.pos.y }, polearm.angle);
        vertex({ -165, -51 + armup }, {  polearm.pos.x,  polearm.pos.y }, polearm.angle);
        vertex({ -85, -51 + armup }, {  polearm.pos.x, polearm.pos.y }, polearm.angle);
        vertex({ -85, -55 + armup }, { polearm.pos.x,  polearm.pos.y }, polearm.angle);
        glEnd();
        //mızrak ucu
        glColor3f(1, 1, 1);
        glBegin(GL_TRIANGLES);
        vertex({ -85, -55 + armup }, { polearm.pos.x, polearm.pos.y }, polearm.angle);
        vertex({ -85, -51 + armup }, { polearm.pos.x, polearm.pos.y }, polearm.angle);
        vertex({ -70, -53 + armup }, { polearm.pos.x, polearm.pos.y }, polearm.angle);
        glEnd();


}
void mansMovingArms()
{

    double angle = (p.angle);
    glColor3ub(90, 50, 138);
    glBegin(GL_QUADS);
    vertex({ -175, -65 }, { p.pos.x, p.pos.y }, angle);
    vertex({ -175, -55 }, { p.pos.x, p.pos.y }, angle);
    vertex({ -145, -55 }, { p.pos.x, p.pos.y }, angle);
    vertex({ -145, -65 }, { p.pos.x, p.pos.y }, angle);
    glEnd();

    glColor3f(1.11, 0.80, 0.55);
    glBegin(GL_QUADS);
    vertex({ -145, -65 }, { p.pos.x, p.pos.y }, angle);
    vertex({ -145, -55 }, { p.pos.x, p.pos.y }, angle);
    vertex({ -135, -55 }, { p.pos.x, p.pos.y }, angle);
    vertex({ -135, -65 }, { p.pos.x, p.pos.y }, angle);
    glEnd();

}

void drawHorseAndMan() {

    //horse torso
    glColor3f(0.6, 0.3, 0);
    glRectf(p.pos.x - 180, p.pos.y - 100, p.pos.x - 100, p.pos.y - 140);

    //horse legs(+toynak)
    glColor3f(0.6, 0.3, 0);
    glRectf(p.pos.x - 180, p.pos.y - 180, p.pos.x - 190, p.pos.y - 100);
    glColor3f(0, 0, 0);
    glRectf(p.pos.x - 180, p.pos.y - 190, p.pos.x - 190, p.pos.y - 180);

    glColor3f(0.6, 0.3, 0);
    glRectf(p.pos.x - 110, p.pos.y - 180, p.pos.x - 100, p.pos.y - 100);
    glColor3f(0, 0, 0);
    glRectf(p.pos.x - 110, p.pos.y - 190, p.pos.x - 100, p.pos.y - 180);

    //horse tail
    glColor3f(0, 0, 0);
    glRectf(p.pos.x - 200, p.pos.y - 175, p.pos.x - 190, p.pos.y - 110);

    //horse neck
    glColor3f(0.6, 0.3, 0);
    glRectf(p.pos.x - 125, p.pos.y - 140, p.pos.x - 100, p.pos.y - 60);

    //horse head
    glColor3f(0.6, 0.3, 0);
    glRectf(p.pos.x - 125, p.pos.y - 60, p.pos.x - 100, p.pos.y - 40);

    glColor3f(0.6, 0.3, 0);
    glBegin(GL_QUADS);
    glVertex2f(p.pos.x - 100, p.pos.y - 65);
    glVertex2f(p.pos.x - 100, p.pos.y - 40);
    glVertex2f(p.pos.x - 70, p.pos.y - 55);
    glVertex2f(p.pos.x - 70, p.pos.y - 65);
    glEnd();

    //yele *mane*
    glColor3f(0, 0, 0);
    glRectf(p.pos.x - 143, p.pos.y - 100, p.pos.x - 125, p.pos.y - 48);

    //horse ear
    glColor3f(0.6, 0.3, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(p.pos.x - 125, p.pos.y - 40);
    glVertex2f(p.pos.x - 110, p.pos.y - 20);
    glVertex2f(p.pos.x - 110, p.pos.y - 40);
    glEnd();

    //eye
    glColor3f(0, 0, 0);
    circle(p.pos.x - 100, p.pos.y - 50, 5);
    glColor3f(1, 1, 1);
    circle(p.pos.x - 98, p.pos.y - 50, 2);

    //man butt
    glColor3f(0.3, 0.3, 0.8);
    glRectf(p.pos.x - 180, p.pos.y - 100, p.pos.x - 145, p.pos.y - 90);

    //man leg
    glColor3f(0.3, 0.3, 0.8);
    glRectf(p.pos.x - 155, p.pos.y - 120, p.pos.x - 145, p.pos.y - 100);

    //man torso
    glColor3ub(90, 50, 138);
    glRectf(p.pos.x - 180, p.pos.y - 90, p.pos.x - 160, p.pos.y - 50);

    //boot
    glColor3f(0, 0, 0);
    glRectf(p.pos.x - 155, p.pos.y - 130, p.pos.x - 145, p.pos.y - 120);
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(p.pos.x - 145, p.pos.y - 130);
    glVertex2f(p.pos.x - 145, p.pos.y - 120);
    glVertex2f(p.pos.x - 135, p.pos.y - 125);
    glVertex2f(p.pos.x - 135, p.pos.y - 130);
    glEnd();

    //neck & head
    glColor3f(1.11, 0.80, 0.55);
    glRectf(p.pos.x - 175, p.pos.y - 50, p.pos.x - 165, p.pos.y - 40);
    glColor3f(1.11, 0.80, 0.55);
    circle(p.pos.x - 170, p.pos.y - 30, 15);
    glColor3f(1.11, 0.80, 0.55);
    glBegin(GL_TRIANGLES);
    glVertex2f(p.pos.x - 158, p.pos.y - 35);
    glVertex2f(p.pos.x - 158, p.pos.y - 20);
    glVertex2f(p.pos.x - 150, p.pos.y - 35);
    glEnd();
    glColor3f(1, 1, 1);
    circle(p.pos.x - 165, p.pos.y - 27, 5);
    glColor3f(0, 0, 0);
    circle(p.pos.x - 162, p.pos.y - 27, 3);

    //hat
    glColor3ub(90, 50, 138);
    glBegin(GL_TRIANGLES);
    glVertex2f(p.pos.x - 190, p.pos.y - 23);
    glVertex2f(p.pos.x - 175, p.pos.y - 3);
    glVertex2f(p.pos.x - 150, p.pos.y - 23);
    glEnd();

    //hat feather because it looks fancy
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLES);
    glVertex2f(p.pos.x - 188, p.pos.y - 15);
    glVertex2f(p.pos.x - 184, p.pos.y + 10);
    glVertex2f(p.pos.x - 180, p.pos.y - 20);
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

void displayGame() {
    

    drawHorseAndMan();


    drawPolearm();

    if (hookVis) {
        glLineWidth(2);
        glBegin(GL_LINES);
        glVertex2f(p.pos.x - 170, p.pos.y - 60);
        glVertex2f(polearm.pos.x - 170, polearm.pos.y - 60);
        glEnd();
        glLineWidth(1);
    }


    mansMovingArms();

    for (int i = 0; i < activeNum; i++)
        drawTarget(tars[i]);

    glColor3f(0, 0, 0);
    for (int i = 0; i < 20; i++)
        if (bullets[i].set)
            circle(bullets[i].bulx, bullets[i].buly, 4);

    glColor3f(0, 1, 0);
    glRectf(winWidth / 2 - 210, winHeight / 2 - 30, winWidth / 2 - 210 + health * 2, winHeight / 2 - 45);
    vprint2(winWidth / 2 - 240, winHeight / 2 - 43, 0.1, "%d", health);

    glColor3f(0, 0, 1);
    glRectf(winWidth / 2 - 210, winHeight / 2 - 55, winWidth / 2 - 210 + score, winHeight / 2 - 70);
    vprint2(winWidth / 2 - 240, winHeight / 2 - 68, 0.1, "%d", score);
}

void dispEntrence() {
    vprint2(-240, 250, 0.15, "Please don't give a break in your nickname ");
    vprint2(-240, 220, 0.15, "Press enter to start");
    vprint2( - 320, 80, 0.3, "Please enter your name: %s ",currentGamer.name);
    if(gamerNum>0)
        for(int i=0;i<gamerNum;i++)
        vprint2(-240, -10-35*i, 0.2, "%-20s %d", gamer[i].name,gamer[i].score);
}

void dispEnd() {
    vprint2(-240, 120, 0.4, "Congratuations %s ", currentGamer.name);
    vprint2(-240, 50, 0.4, "Your score is %d ", score);
    
    bool winner = true;
    for (int i = 0; i < gamerNum; i++)
        if (gamer[i].score > score)
            winner = false;

    if(winner)//is it a record
        vprint2(-140, -10, 0.25, "NEW RECORD ");

    if (printFlag) {//prevent from printing multiple records
        fprintf(fa, "%s %d\n", currentGamer.name,score);
        printFlag = false;
    }
    
}

void display() {
    //
    // clear window to white
    //
    int n = 0;

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
    //vprint(-620, 300, GLUT_BITMAP_8_BY_13, "Ayse Zulal Ekici");
    //vprint(-610, 280, GLUT_BITMAP_8_BY_13, "22103565");
    //vprint(-590, 65, GLUT_BITMAP_8_BY_13, "Use arrow keys to manage your position and the man will handle the rest. Click to summon the target. If it gets stuck summon the target again.");

    if (gameMode == 0)
        dispEntrence();
    else if (gameMode == 1)
        displayGame();
    else
        dispEnd();
   
    glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//

void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    
    static int index = 0;
    if (gameMode == 0) {
        
        printf("%d\n", key);

        if (key == 8 && index > 0) {//8 is ascii of backspace
            currentGamer.name[index - 1] = '\0';
            index--;
            printf("delete");
        }
        else if ((int)key == 13)// ascii key of enter
        {
            gameMode = 1;
            printf("gamemode %d",gameMode);
         }
          
        else {
            currentGamer.name[index] = (char)key;
            currentGamer.name[index + 1] = '\0';
            index++;
        }

        //printf("%d\n", index);
    }
    
    


    // to refresh the window it calls display() function

    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
   // if (key == ' ')
     //   spacebar = false;
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
        if (p.pos.y < -20) {
            aay += 20;
            p.pos.y += 20;
        }
        break;
    case GLUT_KEY_DOWN: down = true;
        aay -= 20;
        p.pos.y -= 20;
        break;
    case GLUT_KEY_LEFT: left = true;
        aax -= 20;
        p.pos.x -= 20;
        break;
    case GLUT_KEY_RIGHT: right = true;
        aax += 20;
        p.pos.x += 20;
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
        if(gameMode==1)
        polearm.active = true;

       
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
    int x2 = x - winWidth / 2;
    int y2 = winHeight / 2 - y;
     

    p.angle = atan2(y2 - (p.pos.y-movey), x2 - (p.pos.x-movex));


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void resetTarget(target_t* t) {
    srand(time(NULL));
    t->center.y = rand() % (winHeight / 2);
    t->center.x = rand() % winWidth - winWidth / 2;
    t->radius = 34;
}

bool testCollision(fire_t fr, target_t t) {
    float dx = t.center.x - (fr.pos.x-170);
    float dy = t.center.y - (fr.pos.y-58);
    float d = sqrtf(dx * dx + dy * dy);
    return d <= t.radius;
}

bool manHitCheck(bullet_t bul) {
    if (bul.bulx > p.pos.x - 190 && bul.bulx < p.pos.x - 150 && bul.buly>p.pos.y - 120 && bul.buly < p.pos.y - 10)
        return true;
    return false;
}



void turn(player_t* p, double turn) {
    p->angle += turn;
}


#if TIMER_ON == 1
void onTimer(int v) 
{

    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    // Write your codes here.

    //animate stars
    if (((winWidth > 400 || winHeight > 400) && bgCheck == 1) || bgCounter == 126)
    {
        bgCounter = 0;
        bgOn = false;
        bgCheck = 0;
    }
    bgCounter++;

    if (gameMode == 1) {

        if (polearm.active == 0) {
            polearm.pos = p.pos;
            polearm.angle = p.angle;
        }
        else if (polearm.active == 1) {
            polearm.pos.x += POLEARM_SPEED * cos(polearm.angle);
            polearm.pos.y += POLEARM_SPEED * sin(polearm.angle);
            //printf("go on\n");
        }
        else if (polearm.active == -1) {
            polearm.angle = atan2(polearm.pos.y - p.pos.y, polearm.pos.x - p.pos.x);
            polearm.pos.x += POLEARM_SPEED_RETURN * cos(polearm.angle);
            polearm.pos.y += POLEARM_SPEED_RETURN * sin(polearm.angle);
            //printf("return\n");
        }


        if (fabs(polearm.pos.x - p.pos.x) < 8 && fabs(polearm.pos.y - p.pos.y) < 8 && polearm.active) {
            polearm.active = 0;
            polearm.pos = p.pos;
            polearm.angle = p.angle;
            hookVis = false;
        }


        for (int i = 0; i < activeNum; i++) {
            tars[i].timer--;
            if (tars[i].timer == 0)
                targetfire(tars + i);

            if (testCollision(polearm, tars[i])) {
                resetTarget(tars + i);
                score += 2;
                polearm.active = -1;
                hookVis = true;
                levelCheck = true;
            }
        }

        if (activeNum < 20 && score % 10 == 0 && levelCheck) {
            activeNum++;
            levelCheck = false;
        }

        if (polearm.pos.x<-winWidth / 2 - 120 || polearm.pos.x > winWidth / 2 + 120 || polearm.pos.y > winHeight / 2 + 120 || polearm.pos.y < -winHeight / 2 - 120) {
            polearm.active = -1;
            hookVis = true;
        }

        for (int i = 0; i < 20; i++) {
            if (bullets[i].set) {
                bullets[i].bulx -= FIRE_SPEED * cos(bullets[i].angle);
                bullets[i].buly -= FIRE_SPEED * sin(bullets[i].angle);
            }

            if (manHitCheck(bullets[i])) {

                if (health > 0 && bullets[i].set)
                    health -= 5;
                bullets[i].set = false;
                //printf("%d\n", health);
            }
            else if (bullets[i].bulx > winWidth / 2 || bullets[i].bulx < -winWidth / 2 || bullets[i].buly < -winHeight / 2)
                bullets[i].set = false;
        }


        if (health == 0)
            gameMode = 2;
    }
    

    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()
}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void main(int argc, char* argv[]) {

    fr = fopen("champions.txt", "r");


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

   
    char tmpname[20]="\0";
    int sc;
    while (fscanf(fr, "%s %d", tmpname, &sc) != EOF) {
        strcpy(gamer[gamerNum].name, tmpname);
        gamer[gamerNum].score = sc;
        gamerNum++;
    }

    for (int i = 0; i < 20; i++) {
         tars[i].center.y = rand() % (WINDOW_HEIGHT / 2);
         tars[i].center.x = rand() % WINDOW_WIDTH - WINDOW_WIDTH / 2;
         tars[i].radius = 34;
    }

    for (int i = 0; i < 20; i++)
        bullets[i].set = false;
    for (int i = 0; i < 20; i++)
        tars[i].timer = rand() % 240 + 60;

    glutMainLoop();
}