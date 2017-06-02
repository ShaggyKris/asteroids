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

#define SHIP_COORDS(ship)                                                    \
    struct Coords ship_coords[] =                                          \
    {                                                                        \
        {ship->pos.x, ship->pos.y + SHIP_HEIGHT},              \
        {ship->pos.x - SHIP_WIDTH, ship->pos.y - SHIP_HEIGHT}, \
        {ship->pos.x,ship->pos.y - SHIP_HEIGHT/2}, \
        {ship->pos.x + SHIP_WIDTH, ship->pos.y - SHIP_HEIGHT}  \
    }; \

#define SHIP_HEIGHT 3
#define SHIP_WIDTH 2
#define SHIP_MAX_SPEED 3
#define SHIP_MAX_ROTATION 0.3
#define SHIP_ACCELERATION 0.2

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
	double	x, y, dx, dy;
} Photon;

typedef struct {
	int	active, nVertices;
	double	x, y, phi, dx, dy, dphi;
	Coords	coords[MAX_VERTICES];
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


/* -- global variables ------------------------------------------------------ */

static int	up=0, down=0, left=0, right=0;	/* state of cursor keys */
static double	xMax, yMax;
static Ship	ship;
static Photon	photons[MAX_PHOTONS];
static Asteroid	asteroids[MAX_ASTEROIDS];

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

    for (i=0; i<MAX_PHOTONS; i++)
    	if (photons[i].active)
            drawPhoton(&photons[i]);

    for (i=0; i<MAX_ASTEROIDS; i++)
    	if (asteroids[i].active)
            drawAsteroid(&asteroids[i]);
    
    glutSwapBuffers();
}

void
myTimer(int value)
{
    /*
     *	timer callback function
     */

    /* advance the ship */
	
	double speed, da;
	// Input and set rotation.
	ship.phi += (left*SHIP_MAX_ROTATION) - (right*SHIP_MAX_ROTATION);
	da = (up * SHIP_ACCELERATION) - (down*SHIP_ACCELERATION);
	// Update dx and dy
	ship.dx += da * -sin(ship.phi);
	ship.dy += da * cos(ship.phi);
	speed = sqrt(ship.dx*ship.dx + ship.dy*ship.dy);
	
	if( speed > SHIP_MAX_SPEED)
	{
		ship.dx = (ship.dx / speed) * SHIP_MAX_SPEED;
		ship.dy = (ship.dy / speed) * SHIP_MAX_SPEED;
	}
	else
	{
		ship.pos.x += ship.dx;
		ship.pos.y += ship.dy;
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
    
    ship.pos.x = 25;
	ship.pos.y = 25;
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
        
    a->x = x;
    a->y = y;
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
	
	SHIP_COORDS(s);
    
    glLoadIdentity();
    //COLOR_GL_LINE;
    myTranslate2D(s->pos.x,s->pos.y);
    myRotate2D(s->phi);
    
    glPushMatrix();
    
    glBegin(GL_LINE_LOOP);
    
		for(int i=0;i<4;i++){
			//glVertex2f(2*sin(2*i*M_PI/3), 2*cos(2*i*M_PI/3));
			glVertex2f(ship_coords[i].x, ship_coords[i].y);
			
			
			
			
		}    
    printf("\nVertex X: %f", ship_coords[0].x);
	printf("\nVertex Y: %f", ship_coords[0].y);
	fflush(stdout);
    glEnd();
    //glLineWidth(2);
    glPopMatrix();
}

void
drawPhoton(Photon *p)
{
}

void
drawAsteroid(Asteroid *a)
{
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