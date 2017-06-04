/*
 *	asteroids.c
 *
 *	skeleton code for an OpenGL implementation of the Asteroids video game
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define RAD2DEG 180.0/M_PI
#define DEG2RAD M_PI/180.0

#define myTranslate2D(x,y) glTranslated(x, y, 0.0)
#define myScale2D(x,y) glScalef(x, y, 1.0)
#define myRotate2D(angle) glRotatef(RAD2DEG*angle, 0.0, 0.0, 1.0)


#define MAX_PHOTONS	8
#define MAX_ASTEROIDS	8
#define MAX_VERTICES	16


#define drawCircle() glCallList(circle)

#define SHIP_HEIGHT 3
#define SHIP_WIDTH 2
#define SHIP_MAX_SPEED 3
#define SHIP_MAX_ROTATION 0.3
#define SHIP_ACCELERATION 0.05

/* -- display list for drawing a circle ------------------------------------- */

static GLuint	circle;

void
buildCircle() {
    GLint   i;

    circle = glGenLists(1);
    glNewList(circle, GL_COMPILE);
      glBegin(GL_POLYGON);
        for(i=0; i<40; i++)
            glVertex2d(cos(i*M_PI/20.0), sin(i*M_PI/20.0));
      glEnd();
    glEndList();
}


/* -- type definitions ------------------------------------------------------ */

typedef struct Coords {
	double		x, y;
} Coords;

typedef struct {
	Coords	pos; 
	double phi, dx, dy;
} Ship;

typedef struct {
	int	active;
	Coords pos;
	double dx, dy;
} Photon;

typedef struct {
	int	active, nVertices;
	double	phi, dx, dy, dphi;
	Coords	coords[MAX_VERTICES],pos;
} Asteroid;


/* -- function prototypes --------------------------------------------------- */

static void	myDisplay(void);
static void	myTimer(int value);
static void	myKey(unsigned char key, int x, int y);
static void	keyPress(int key, int x, int y);
static void	keyRelease(int key, int x, int y);
static void	myReshape(int w, int h);

static void	init(void);
static void	initAsteroid(Asteroid *a, double x, double y, double size);
static void	drawShip(Ship *s);
static void	drawPhoton(Photon *p);
static void	drawAsteroid(Asteroid *a);

static double	myRandom(double min, double max);
static int screenWrap(Coords* pos, int border);

/* -- global variables ------------------------------------------------------ */

static int	up=0, down=0, left=0, right=0;	/* state of cursor keys */
static double	xMax, yMax;
static Ship	ship;
static Photon	photons[MAX_PHOTONS];
static Asteroid	asteroids[MAX_ASTEROIDS];
static int activeAsteroids = 0, activePhotons = 0;
/*Vector to draw ship*/
static Coords ship_coords[] = {
			{0, SHIP_HEIGHT},              
		    {-SHIP_WIDTH, -SHIP_HEIGHT},
		    {0,-SHIP_HEIGHT/2},
		    {SHIP_WIDTH,-SHIP_HEIGHT}
     	};

/* -- main ------------------------------------------------------------------ */

int
main(int argc, char *argv[])
{
    srand((unsigned int) time(NULL));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Asteroids");
    buildCircle();
    glutDisplayFunc(myDisplay);
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(myKey);
    glutSpecialFunc(keyPress);
    glutSpecialUpFunc(keyRelease);
    glutReshapeFunc(myReshape);
    glutTimerFunc(33, myTimer, 0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    init();
    
    glutMainLoop();
    
    return 0;
}


/* -- callback functions ---------------------------------------------------- */

void
myDisplay()
{
    /*
     *	display callback function
     */

    int	i;

    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    drawShip(&ship);

    for (i=0; i<MAX_PHOTONS; i++){
    	if (photons[i].active){
            drawPhoton(&photons[i]);
    		
    	}
    }
    
    

    for (i=0; i<MAX_ASTEROIDS; i++){
    	if (asteroids[i].active)
            drawAsteroid(&asteroids[i]);
    }
    
    glutSwapBuffers();
}
void shipMovement(){
	/* advance the ship */
	
	double velocity, da;
	// Input and set rotation.
	ship.phi += (left*SHIP_MAX_ROTATION) - (right*SHIP_MAX_ROTATION);
	da = (up * SHIP_ACCELERATION) - (down*SHIP_ACCELERATION);
	// Update dx and dy
	ship.dx += da * -sin(ship.phi);
	ship.dy += da * cos(ship.phi);
	velocity = sqrt(ship.dx*ship.dx + ship.dy*ship.dy);
	
	
	if( velocity > SHIP_MAX_SPEED)
	{
		ship.dx = (ship.dx / velocity) / SHIP_MAX_SPEED;
		ship.dy = (ship.dy / velocity) / SHIP_MAX_SPEED;
	}
	else
	{
		ship.pos.x += ship.dx;
		ship.pos.y += ship.dy;
	}
	screenWrap(&ship.pos,SHIP_HEIGHT);
}

void asteroidMovement(Asteroid* a){
	a->pos.x += a->dx;
	a->pos.y += a->dy;
	a->phi += a->dphi;
	screenWrap(&a->pos,2);
}

void photonMovement(Photon* p){
	//printf("\nBefore if, Photon Coords: %f, %f",p->pos.x,p->pos.y);
	//fflush(stdout);
	
	//Set initial coordinates for shot, then fire the shot in the next if statement. 
	//This is so that the shots don't rotate after being fired.
	if(p->active == 1){
		p->active = 2;
		p->pos.x = ship.pos.x + SHIP_HEIGHT * -sin(ship.phi);
		p->pos.y = ship.pos.y + SHIP_HEIGHT *  cos(ship.phi);
		p->dx = 4 * -sin(ship.phi) + ship.dx;
		p->dy = 4 * cos(ship.phi) + ship.dy;
		
		
	}
	else if(p->active == 2){
		p->pos.x += p->dx;
		p->pos.y += p->dy;
		
		printf("\nPhoton Coords: %f, %f",p->pos.x,p->pos.y);
		fflush(stdout);
		
		if(screenWrap(&p->pos,0) == 1)
			p->active = 0;
	}
		
	
}
double randfrom(double min, double max) 
{
    double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

void
myTimer(int value)
{
    
    if(activeAsteroids < MAX_ASTEROIDS){
    	for(int i =0; i < MAX_ASTEROIDS || activeAsteroids > MAX_ASTEROIDS; i++, activeAsteroids++){
    		initAsteroid(&asteroids[i], (rand()%2)*xMax, myRandom(0,yMax), myRandom(2,5));
    	}
    }
    /*
     *	timer callback function
     */
	
	//Handles how the ship moves
	shipMovement();
    
    //Handles individual asteroid movement
    for(int i=0; i < MAX_ASTEROIDS && activeAsteroids > 0; i++){
    	if(asteroids[i].active){
    		asteroidMovement(&asteroids[i]);
    	}
    }
    
    //Handles individual photon movement
    for(int i=0; i < MAX_PHOTONS; i++){
    	if(photons[i].active){
    		photonMovement(&photons[i]);
    	}
    }
	
	
    /* advance photon laser shots, eliminating those that have gone past
      the window boundaries */

    /* advance asteroids */

    /* test for and handle collisions */

    glutPostRedisplay();
    
    glutTimerFunc(33, myTimer, value);		/* 30 frames per second */
}

void
myKey(unsigned char key, int x, int y)
{
    /*
     *	keyboard callback function; add code here for firing the laser,
     *	starting and/or pausing the game, etc.
     */
     switch(key){
     	case ' ':
     		for(int i=0;i<MAX_PHOTONS;i++){
     			if(!photons[i].active){
     				photons[i].active = 1;
     				printf("\nI'VE ACTIVATED!\n");
     				break;
     			}
     		}
     		break;
     
     }
}

void
keyPress(int key, int x, int y)
{
    /*
     *	this function is called when a special key is pressed; we are
     *	interested in the cursor keys only
     */

    switch (key)
    {
        case 100:
            left = 1; break;
        case 101:
            up = 1; break;
		case 102:
            right = 1; break;
        case 103:
            down = 1; break;
    }
}

void
keyRelease(int key, int x, int y)
{
    /*
     *	this function is called when a special key is released; we are
     *	interested in the cursor keys only
     */

    switch (key)
    {
        case 100:
            left = 0; break;
        case 101:
            up = 0; break;
		case 102:
            right = 0; break;
        case 103:
            down = 0; break;
    }
}

void
myReshape(int w, int h)
{
    /*
     *	reshape callback function; the upper and lower boundaries of the
     *	window are at 100.0 and 0.0, respectively; the aspect ratio is
     *  determined by the aspect ratio of the viewport
     */

    xMax = 100.0*w/h;
    yMax = 100.0;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, xMax, 0.0, yMax, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
}


/* -- other functions ------------------------------------------------------- */

void
init()
{
    /*
     * set parameters including the numbers of asteroids and photons present,
     * the maximum velocity of the ship, the velocity of the laser shots, the
     * ship's coordinates and velocity, etc.
     */
    
    ship.pos.x = 50;
	ship.pos.y = 50;
	//printf("\nInit x: %f",ship.x);
	//printf("\nInit y: %f",ship.y);
      
}

void
initAsteroid(
	Asteroid *a,
	double x, double y, double size)
{
    /*
     *	generate an asteroid at the given position; velocity, rotational
     *	velocity, and shape are generated randomly; size serves as a scale
     *	parameter that allows generating asteroids of different sizes; feel
     *	free to adjust the parameters according to your needs
     */

    double	theta, r;
    int		i;
        
    a->pos.x = x;
    a->pos.y = y;
    a->phi = 0.0;
    a->dx = myRandom(-0.8, 0.8);
    a->dy = myRandom(-0.8, 0.8);
    a->dphi = myRandom(-0.2, 0.2);
    
    a->nVertices = 6+rand()%(MAX_VERTICES-6);
    for (i=0; i<a->nVertices; i++)
    {
	theta = 2.0*M_PI*i/a->nVertices;
	r = size*myRandom(2.0, 3.0);
	a->coords[i].x = -r*sin(theta);
	a->coords[i].y = r*cos(theta);
    }
    
    a->active = 1;
}

void
drawShip(Ship *s)
{    
    //glLoadIdentity();
    glPushMatrix();
    myTranslate2D(s->pos.x,s->pos.y);
    myRotate2D(s->phi);
    
    
    glBegin(GL_LINE_LOOP);
    
		for(int i=0;i<4;i++){
			//glVertex2f(2*sin(2*i*M_PI/3), 2*cos(2*i*M_PI/3));
			glVertex2f(ship_coords[i].x, ship_coords[i].y);			
		}    
    //printf("\nVertex: (%f,%f)\tShip origin: (%f,%f)", ship_coords[0].x,ship_coords[0].y,ship.pos.x,ship.pos.y);
	//printf("\nVertex Y: %f", ship_coords[0].y);
	//fflush(stdout);
    glEnd();
    
    glPopMatrix();
}

void
drawPhoton(Photon *p)
{
	glPushMatrix();
	//myTranslate2D(p->pos.x,p->pos.y);
	glBegin(GL_LINE_LOOP);
		glVertex2f(p->pos.x+0.5, p->pos.y+0.5);
		glVertex2f(p->pos.x+0.5, p->pos.y-0.5);
		glVertex2f(p->pos.x-0.5, p->pos.y-0.5);
		glVertex2f(p->pos.x-0.5,p->pos.y+0.5);
	glEnd();
	glPopMatrix();
}

void
drawAsteroid(Asteroid *a){
	
	//a->active = 1;
	glPushMatrix();
	myTranslate2D(a->pos.x,a->pos.y);
	myRotate2D(a->phi);
	
	
	
	glBegin(GL_LINE_LOOP);
		for(int i=0;i < a->nVertices;i++){
			glVertex2f(a->coords[i].x,a->coords[i].y);
		}	
	glEnd();
	
	glPopMatrix();
		
}

int screenWrap(Coords* pos, int border){
	int wrapped = 0;
	if(pos->x < -border) {
    	pos->x = xMax+border;
    	wrapped = 1;
	} 
	else if(pos->x > xMax+border) {
		pos->x = -border;
		wrapped = 1;
	}

	if(pos->y < -border) {
		pos->y = yMax+border;
		wrapped = 1;
	}
	else if (pos->y > yMax+border){
		pos->y = -border;
		wrapped = 1;
	}
	return wrapped;
}

/* -- helper function ------------------------------------------------------- */

double
myRandom(double min, double max)
{
	double	d;
	
	/* return a random number uniformly draw from [min,max] */
	d = min+(max-min)*(rand()%0x7fff)/32767.0;
	
	return d;
}
