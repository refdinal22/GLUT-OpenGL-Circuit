/*
 *  UAS Refdinal 171511023
 *
 *  p             Toggles first person/perspective projection
 *  c             Change Day and Night
 *  arrow keys    Change view angle
 *  ESC           Exit
 *  w             Up
 *  a             Turn Left
 *  s             Back
 *  d             Turn Right
 */
#include "JTKPOLBAN.h"
#include "loadtexbmp.c"
#include "fatal.c"
#include "Errcheck.c"
#include "print.c"
#include "project.c"
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#define ROAD_WIDTH 2
#define ROAD_THICK 0.15
#define CUBE_TOP_LEFT 1
#define CUBE_TOP_RIGHT 2
#define CUBE_BOTTOM_LEFT 3
#define CUBE_BOTTOM_RIGHT 4
#define ANGLE_TURN 6
#define DAY 1
#define NIGHT 0
#define CAR_MOVE 70
#endif

int mode=0;       //  Projection mode
float thf=105;
int th=105;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1.333;  //  Aspect ratio
double dim=11;   //  Size of world

// Light values
int one       =   1;  // Unit value
int distance  =   25;  // Light distance
int local     =   0;  // Local Viewer Model
int emission  =  30;  // Emission intensity (%)
int ambient   =  80;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  0;  // Light azimuth
float ylight  = 13;  // Elevation of light
int earth = DAY; //Day and Night

int at0=100;      //  Constant  attenuation %
int at1=20;        //  Linear    attenuation %
int at2=20;        //  Quadratic attenuation %

int step= 0;
//First person camera location
double fpX = 0;
double fpY = 0.8;
double fpZ = 0;

float xPos = -6;


double xRotateTurn = 0;
double ZRotateTurn = 0;
double turnXInc = 0;
double turnZInc = 0;

//Enemy
float centerXIncrement = 0;
float centerZIncrement = 0;
float xRotate = 0.5;
float zRotate = 0;
float carRotate2 = 0;

//Person Control
float carRotateX = 0.5;
float carRotateZ = 0;
float carXIncrement = 0;
float carZIncrement = 0;
float carRotate = 0;

float temp;

//Race
int start = 0;

//x, y, z for refrence point in glLookAt() for FP mode
double refX = 0;
double refY = 0;
double refZ = 0;

//Texture Variables
int tMode = 0;
float texScale = 0.5;

//Light Vecotrs
float Ambient[]   = {0.8 ,0.8 ,0.8 ,1.0};
float Diffuse[]   = {1.0,1.0,1.0,1.0};
float Specular[]  = {0,0,0,1.0};

GLuint	_textureBasicMetal, _textureGlass, _textureWheel, _textureTire,
		_textureWoodFence, _textureGrass, _textureCinderBlock, _textureCarGrill, 
		_textureHeadLamp, _textureCarbonFiber, _textureSidewalk, 
		_textureGarageDoor, _textureWalkway, _textureHedge, _textureGreyBrick, 
		_textureWoodBeam, _textureFrontDoor2, _textureWindow1, _textureSkyboxFront, 
		_textureSkyboxBack, _textureSkyboxRight, _textureSkyboxLeft, _textureSkyboxTop, 
		_textureAsphalt, _textureBrownBrick, _textureWhiteBrick, _textureMetalRoof, 
		_textureWarehouseWindow, _textureSupport;
;

void initTexture() {       
	_textureSkyboxFront = LoadTexBMP("texture/skybox-front.bmp");
	_textureSkyboxBack = LoadTexBMP("texture/skybox-back.bmp");
	_textureSkyboxRight = LoadTexBMP("texture/skybox-right.bmp");
	_textureSkyboxLeft = LoadTexBMP("texture/skybox-left.bmp");
	_textureSkyboxTop = LoadTexBMP("texture/skybox-top.bmp");
	_textureBrownBrick = LoadTexBMP("texture/brown-brick.bmp");
	_textureSupport = LoadTexBMP("texture/support.bmp");
	_textureAsphalt = LoadTexBMP("texture/asphalt.bmp");
	_textureGarageDoor = LoadTexBMP("texture/garage-door.bmp");
	_textureSidewalk = LoadTexBMP("texture/sidewalk.bmp");	
	_textureBasicMetal = LoadTexBMP("texture/basic-metal.bmp");
	_textureGlass = LoadTexBMP("texture/glass.bmp");
    _textureWheel = LoadTexBMP("texture/car-wheel.bmp");
    _textureTire = LoadTexBMP("texture/tire-tread.bmp");
	_textureCarGrill = LoadTexBMP("texture/car-grill.bmp");
	_textureHeadLamp = LoadTexBMP("texture/headlamp.bmp");
	_textureGlass = LoadTexBMP("texture/glass.bmp");
	_textureCarbonFiber = LoadTexBMP("texture/carbon-fiber.bmp");
	_textureGreyBrick = LoadTexBMP("texture/grey-brick.bmp");
	_textureGrass = LoadTexBMP("texture/grass.bmp");
	_textureWoodBeam = LoadTexBMP("texture/wood-beam.bmp");
	_textureMetalRoof = LoadTexBMP("texture/metal-roof.bmp");
}

static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //Texture repitition values
   float texRepX = 1.0;
   float texRepY = 1.0;

   //  Cube
   glBegin(GL_QUADS);
   //  Front
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, 1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,-1, 1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,+1, 1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,+1, 1);
   //  Back
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(texRepX,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-1,+1,-1);
   glTexCoord2f(0.0,texRepY); glVertex3f(+1,+1,-1);
   //  Right
   texRepX = dz/texScale;
   texRepY = dy/texScale;
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,+1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,texRepY); glVertex3f(+1,+1,+1);
   //  Left
   texRepX = dz/texScale;
   texRepY = dy/texScale;
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(texRepX,0.0); glVertex3f(-1,-1,+1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-1,+1,+1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,+1,-1);
   //  Top
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,+1,+1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,+1,+1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,+1,-1);
   //  Bottom
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glNormal3f( 0,-one, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,-1,+1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}


//Car Start
/*
 *  Draw a section of the car body
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 *     w (1 to color windows for car body, 0 otherwise)
 */
static void body(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,
                 int w)
{
 //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   glBindTexture(GL_TEXTURE_2D,_textureBasicMetal);
   texScale = 0.1;

   glEnable(GL_POLYGON_OFFSET_FILL);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //Texture repitition values
   float texRepX = 1.0;
   float texRepY = 1.0;

   //  Cube
   glBegin(GL_QUADS);

   //  Top
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,+1,+1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,+1,+1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,+1,-1);
   //  Bottom
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glNormal3f( 0,-one, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,-1,+1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,-1,+1);

   //  Front
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, 1);
   glTexCoord2f(texRepX,0.0); glVertex3f(+1,-1, 1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(+1,+1, 1);
   glTexCoord2f(0.0,texRepY); glVertex3f(-1,+1, 1);
   //  Back
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(texRepX,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-1,+1,-1);
   glTexCoord2f(0.0,texRepY); glVertex3f(+1,+1,-1);

   //  End
   glEnd();

   glDisable(GL_POLYGON_OFFSET_FILL);
   //Color and texture to add windows
   if(w == 1) {
      glColor3f(0.8, 0.8, 1);
      glBindTexture(GL_TEXTURE_2D,_textureGlass);
      texRepX = 1.0;
      texRepY = 1.0;
      glBegin(GL_QUADS);
      //  Front
      glNormal3f(0, 0, 1);
      glTexCoord2f(0.0,0.0); glVertex3f(-0.8,-1, 1);
      glTexCoord2f(texRepX,0.0); glVertex3f(+0.8,-1, 1);
      glTexCoord2f(texRepX,texRepY); glVertex3f(+0.8,+1, 1);
      glTexCoord2f(0.0,texRepY); glVertex3f(-0.8,+1, 1);
      //  Back
      glNormal3f(0, 0,-1);
      glTexCoord2f(0.0,0.0); glVertex3f(+0.8,-1,-1);
      glTexCoord2f(texRepX,0.0); glVertex3f(-0.8,-1,-1);
      glTexCoord2f(texRepX,texRepY); glVertex3f(-0.8,+1,-1);
      glTexCoord2f(0.0,texRepY); glVertex3f(+0.8,+1,-1);
      glEnd();
   }
   glEnable(GL_POLYGON_OFFSET_FILL);
   
   //  Undo transformations
   glPopMatrix();
}

static void wheel(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,
                 int d,
                 int s)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   //Turn off shininess for the rubber tire
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   glBindTexture(GL_TEXTURE_2D,_textureWheel);
   
   glColor3f(0.8,0.8,0.8);
   //Tire
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, 0, -1);
   glTexCoord2f(0.5,0.5);
   glVertex3f(0, 0, -0.05);
   for (th=0;th<=360;th+=s)
   {
      double ph = d-90;
      glTexCoord2f(0.5*Cos(th)+0.5,0.5*Sin(th)+0.5);
      glVertex3d(Sin(th)*Cos(ph), Cos(th)*Cos(ph), -0.05);
   }
   glEnd();

   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0.5,0.5);
   glVertex3f(0, 0, 0.05);
   for (th=360;th>=0;th-=s)
   {
      double ph = d-90;
      glTexCoord2f(0.5*Cos(th)+0.5,0.5*Sin(th)+0.5);
      glVertex3d(Sin(th)*Cos(ph) , Cos(th)*Cos(ph), 0.05);
   }
   glEnd();

   glBindTexture(GL_TEXTURE_2D, _textureTire);
  
   glColor3f(0.5,0.5,0.55);
   glBegin(GL_QUAD_STRIP);

   for (th=0;th<=360;th+=s)
   {
      double ph = d-90;
      glNormal3f(Sin(th)*Cos(ph), Cos(th)*Cos(ph), 0);
      glTexCoord2f(0,0.1*th); glVertex3d(Sin(th)*Cos(ph), Cos(th)*Cos(ph), -0.05);
      glTexCoord2f(1,0.1*th); glVertex3d(Sin(th)*Cos(ph), Cos(th)*Cos(ph), 0.05);
   }
   // glNormal3f(Sin(0)*Cos(-90), Cos(0)*Cos(-90), 0);
   // glVertex3d(Sin(0)*Cos(-90), Cos(0)*Cos(-90), -0.05);
   // glVertex3d(Sin(0)*Cos(-90), Cos(0)*Cos(-90), 0.05);
   glEnd();

   //  Undo transformations
   glPopMatrix();
}

static void bumper(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,
                 int m)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //Texture repitition values
   float texRepX = 1.0;
   float texRepY = 1.0;

   glBindTexture(GL_TEXTURE_2D,_textureBasicMetal);
   texScale = 0.1;

   //Offset the bumper so that the lights and grill are drawn directly on the surface
   glEnable(GL_POLYGON_OFFSET_FILL);

   //Bumper

   //Base
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glBegin(GL_POLYGON);
   glNormal3f(0, -1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0,0,0.4);
   glTexCoord2f(texRepX,0.0); glVertex3f(0,0,-0.4);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.1,0,-0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.1,0,0.35);
   glEnd();

   //Front Panels
   glBegin(GL_QUADS);
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f(0.447214, 0, 0.894427);
   glTexCoord2f(0.0,0.0); glVertex3f(0.1, 0, 0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.1, 0.2, 0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0, 0.2, 0.4);
   glTexCoord2f(0.0,texRepY); glVertex3f(0, 0, 0.4);

   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0.1, 0, -0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.1, 0.2, -0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.1, 0.2, 0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.1, 0, 0.35);

   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f(0.447214, 0, -0.894427);
   glTexCoord2f(0.0,0.0); glVertex3f(0.1, 0, -0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0, 0, -0.4);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0, 0.2, -0.4);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.1, 0.2, -0.35);
   
   glEnd();

   //Upper Bumper
   glBegin(GL_QUADS);
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f(0.447214, 0.894427, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0, 0.25, 0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.1, 0.2, 0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.1, 0.2, -0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0, 0.25, -0.35);
   glEnd();

   glBegin(GL_TRIANGLES);
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f(0.333333, 0.666667, 0.666667);
   glTexCoord2f(0.0, 0.0); glVertex3f(0, 0.2, 0.4);
   glTexCoord2f(texRepX/2, texRepY); glVertex3f(0.1, 0.2, 0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(0, 0.25, 0.35);

   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glNormal3f(0.333333, 0.666667, -0.666667);
   glTexCoord2f(0.0, 0.0); glVertex3f(0, 0.25, -0.35);
   glTexCoord2f(texRepX/2, texRepY); glVertex3f(0.1, 0.2, -0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(0, 0.2, -0.4);
   glEnd();

   //  Disable polygon offset
   glDisable(GL_POLYGON_OFFSET_FILL);

   if (m == 1) {
      glColor3f(0.2,0.2,0.2);
      glBindTexture(GL_TEXTURE_2D,_textureCarGrill);

      //Grill
      glBegin(GL_QUADS);
      glNormal3f(1, 0, 0);
      glTexCoord2f(0.0,0.0); glVertex3f(0.1, 0.15, 0.18);
      glTexCoord2f(0.5,0.0); glVertex3f(0.1, 0.03, 0.18);
      glTexCoord2f(0.5,1.0); glVertex3f(0.1, 0.03, -0.18);
      glTexCoord2f(0.0,1.0); glVertex3f(0.1, 0.15, -0.18);
      glEnd();
   }
   
   //Lights (taillights or headlights)
   float emColor[4];
   if(m == 1) {
      emColor[0] = 1.0 * emission;
      emColor[1] = 1.0 * emission;
      emColor[2] = 0.8 * emission;
      emColor[3] = 1.0 * emission;
      glColor3f(1, 1, 0.8);
   } else {
      emColor[0] = 0.8 * emission;
      emColor[1] = 0.0 * emission;
      emColor[2] = 0.0 * emission;
      emColor[3] = 1.0 * emission;
      glColor3f(.8, 0, 0);
   }

   glMaterialf(GL_FRONT,GL_SHININESS,0);
   glMaterialfv(GL_FRONT,GL_SPECULAR,emColor);
   glMaterialfv(GL_FRONT,GL_EMISSION,emColor);

   glBindTexture(GL_TEXTURE_2D,_textureHeadLamp);

   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.5,0.5); glVertex3f(0.1, 0.13, -0.25);
   for (th=0;th<=360;th+=10)
   {
      double ph = 3-90;
      glTexCoord2f(0.5*Cos(th)+0.5,0.5*Sin(th)+0.5);
      glVertex3d(0.1, 0.13+(Cos(th)*Cos(ph)), -0.25+(Sin(th)*Cos(ph)));
   }
   glEnd();

   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(1, 0, 0);
   glTexCoord2f(0.5,0.5); glVertex3f(0.1, 0.13, 0.25);
   for (th=0;th<=360;th+=10)
   {
      double ph = 3-90;
      glTexCoord2f(0.5*Cos(th)+0.5,0.5*Sin(th)+0.5);
      glVertex3d(0.1, 0.13+(Cos(th)*Cos(ph)), 0.25+(Sin(th)*Cos(ph)));
   }
   glEnd();

   glEnable(GL_POLYGON_OFFSET_FILL);

   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //Undo transformations
   glPopMatrix();  
}

static void car(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th,
                 float cr, float cb, float cg)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
   glPolygonOffset(1,1);

   wheel(0.6,0,0.4, 1,1,1, 0, 8, 10);
   wheel(-0.6,0,-0.4, 1,1,1, 0, 8, 10);
   wheel(0.6,0,-0.4, 1,1,1, 0, 8, 10);
   wheel(-0.6,0,0.4, 1,1,1, 0, 8, 10);

   glColor3f(cr, cb, cg);

   //Lower Body
   body(0,0.1,0, 0.8,0.1,0.4, 0, 0);
   //Cabin
   body(-0.1,0.3,0, 0.3,0.1,0.35, 0, 1);
  
   texScale = 1.0;

   glColor3f(cr, cb, cg);

   //Front Bumper
   bumper(0.8,0,0, 1,1,1, 0, 1);

   glColor3f(cr, cb, cg);

   //Rear Bumper
   bumper(-0.8,0,0, 1,1,1, 180, 0);
   
   //  Set specular color to white
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
   glEnable(GL_POLYGON_OFFSET_FILL);

   glColor3f(cr, cb, cg);

   glBindTexture(GL_TEXTURE_2D,_textureBasicMetal);

   //Texture repitition values
   float texRepX = 1.0;
   float texRepY = 1.0;

   //Hood and upper side pannels
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glBegin(GL_QUADS);
   glNormal3f(0, 0.707107, 0.707107);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8, 0.25, 0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8, 0.2, 0.4);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.8, 0.2, 0.4);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.8, 0.25, 0.35);

   glNormal3f(0, 1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0.4, 0.25, 0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.8, 0.25, 0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.8, 0.25, -0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.4, 0.25, -0.35);
   
   glNormal3f(0, 0.707107, -0.707107);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8, 0.2, -0.4);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8, 0.25, -0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.8, 0.25, -0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.8, 0.2, -0.4);
   glEnd();

   //Trunk
   texRepX = dx/texScale;
   texRepY = dz/texScale;
   glBegin(GL_QUADS);
   glNormal3f(0,1,0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8, 0.25, -0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8, 0.25, 0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.6, 0.25, 0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(-0.6, 0.25, -0.35);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureGlass);

   glColor3f(0.8, 0.8, 1);

   //Windshield
   texRepX = 1.0;
   texRepY = 1.0;
   glBegin(GL_QUADS);
   glNormal3f(0.6, 0.8, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(0.4,0.25,0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(0.4,0.25,-0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(0.2,0.4,-0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(0.2,0.4,0.35);
   glEnd();

   glBegin(GL_TRIANGLES);
   glNormal3f(0,0,1);
   glTexCoord2f(0.0,0.0); glVertex3f(0.2,0.4,0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(0.2,0.25,0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(0.4,0.25,0.35);

   glNormal3f(0,0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(0.4,0.25,-0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(0.2,0.25,-0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(0.2,0.4,-0.35);
   glEnd();

   //Rear Window
   texRepX = 1.0;
   texRepY = 1.0;
   glBegin(GL_QUADS);
   glNormal3f(-0.6, 0.8, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.6,0.25,-0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.6,0.25,0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.4,0.4,0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(-0.4,0.4,-0.35);
   glEnd();

   glBegin(GL_TRIANGLES);
   glNormal3f(0,0,1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.6,0.25,0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.4,0.25,0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.4,0.4,0.35);
   
   glNormal3f(0,0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.4,0.4,-0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.4,0.25,-0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.6,0.25,-0.35);
   glEnd();


   glBindTexture(GL_TEXTURE_2D,_textureCarbonFiber);

   //Spoiler
   glColor3f(0.3,0.3,0.3);
   cube(-0.75,0.28,0.3, 0.02,0.03,0.02, 0);
   cube(-0.75,0.28,-0.3, 0.02,0.03,0.02, 0);

   texRepX = 5.0;
   texRepY = 1.0;

   glBegin(GL_QUADS);
   glNormal3f(0, -1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.7,0.31,-0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.7,0.31,0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.8,0.31,0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(-0.8,0.31,-0.35);
   
   glNormal3f(0.196116, 0.980581, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8,0.33,-0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8,0.33,0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.7,0.31,0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(-0.7,0.31,-0.35);

   texRepX = 5.0;
   texRepY = 0.2;
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.8,0.33,0.35);
   glTexCoord2f(texRepX,0.0); glVertex3f(-0.8,0.33,-0.35);
   glTexCoord2f(texRepX,texRepY); glVertex3f(-0.8,0.31,-0.35);
   glTexCoord2f(0.0,texRepY); glVertex3f(-0.8,0.31,0.35);
   
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureBasicMetal);
   glColor3f(cr, cb, cg);

   //Spoiler Fins
   texRepX = dx/texScale;
   texRepY = dy/texScale;
   glBegin(GL_TRIANGLES);
   glNormal3f(0,0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.68,0.31,-0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.82,0.31,-0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.82,0.35,-0.35);

   glNormal3f(0,0,1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.82,0.35,0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.82,0.31,0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.68,0.31,0.35);
   
   //Duplicate to draw both sides (with inverted normals) when face culling is on
   glNormal3f(0,0,1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.68,0.31,-0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.82,0.31,-0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.82,0.35,-0.35);

   glNormal3f(0,0,-1);
   glTexCoord2f(0.0,0.0); glVertex3f(-0.82,0.35,0.35);
   glTexCoord2f(texRepX, 0.0); glVertex3f(-0.82,0.31,0.35);
   glTexCoord2f(texRepX, texRepY); glVertex3f(-0.68,0.31,0.35);
   
   glEnd();
   
   //Undo transformations
   glPopMatrix();
}

//Car End
static void support(double x, double y, double z)
{
   glBindTexture(GL_TEXTURE_2D,_textureSupport );
   glPushMatrix();
      glTranslated(x,y,z);
      cube(0,-0.05,-0.5, 2,0.15,0.2, 0);
   glPopMatrix();
}

static void road(double x, double y, double z)
{
   glBindTexture(GL_TEXTURE_2D,_textureAsphalt);
   glPushMatrix();
      glTranslated(x,y,z);
      cube(0,-0.05,-0.5, 2,0.15,1, 0);
   glPopMatrix();
}

static void lightRoad(double x, double y, double z)
{
   glBindTexture(GL_TEXTURE_2D,_textureHeadLamp);
   texScale = 0.1;
   glPushMatrix();   
      // cube(-4,4,13, 0.07,0.02,0.1, 0);
      glTranslated(x,y,z);
      glRotated(5,1,0,0);
      cube(-4,4,13, 1.5,0.5,0.1, 0);
   glPopMatrix();
   
   glBindTexture(GL_TEXTURE_2D, _textureBasicMetal);
   glPushMatrix();   
      glTranslated(x,y,z);
      cube(-4,1.6,13.2, 0.1 ,1.6,0.1, 0);
   glPopMatrix();
   
}

static void pitstop(double x, double y, double z)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
           
   glBindTexture(GL_TEXTURE_2D,_textureSupport);
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
       //Building
       cube(0,1.6,0, 2,0.4,1, 0); //Top   
       glBindTexture(GL_TEXTURE_2D,_textureBrownBrick);
       cube(-1.75,0.65,0, 0.25,0.55,1, 0); //Left
       cube(1.75,0.65,0, 0.25,0.55,1, 0); //Right
       cube(0,0.65,0, 0.2,0.55,1, 0); //Middle
   glPopMatrix();   
   
   float texRepX = 1.0;
   float texRepY = 1.0;   
   //Door left
   glBindTexture(GL_TEXTURE_2D,_textureGarageDoor);
   glPushMatrix();
      glTranslated(x,y,z);
       glBegin(GL_QUADS);
           //Left
           glTexCoord2f(0.0,0.0); glVertex3f(-1.5, 0.1, 0.9);
           glTexCoord2f(texRepX,0.0); glVertex3f(-0.2, 0.1, 0.9);
           glTexCoord2f(texRepX,texRepY); glVertex3f(-0.2, 1.2, 0.9);
           glTexCoord2f(0.0,texRepY); glVertex3f(-1.5, 1.2, 0.9);
        glEnd();
   glPopMatrix();  
   //Door Right     
   glPushMatrix();
      glTranslated(x+1.7,y,z);
       glBegin(GL_QUADS);         
           //Left
           glTexCoord2f(0.0,0.0); glVertex3f(-1.5, 0.1, 0.9);
           glTexCoord2f(texRepX,0.0); glVertex3f(-0.2, 0.1, 0.9);
           glTexCoord2f(texRepX,texRepY); glVertex3f(-0.2, 1.2, 0.9);
           glTexCoord2f(0.0,texRepY); glVertex3f(-1.5, 1.2, 0.9);
        glEnd();
   glPopMatrix();
   //Floor
   glBindTexture(GL_TEXTURE_2D,_textureSidewalk );
   glPushMatrix();
      glTranslated(x,y,z);
      cube(0,-0.05,0.9, 2,0.15,0.1, 0);
   glPopMatrix();
   
   //Sidewalk
   glPushMatrix();
       glTranslated(x,0, 0);
       cube(0,-0.05,z+1.5, 2,0.15,0.5, 0); // Along Street
       glTranslated(0,0, 1);
       cube(0,-0.05,z+1.5, 2,0.15,0.5, 0); // Along Street
   glPopMatrix();
   //
   support(x,y, z+3.7);
   //Road
   road(x, y,z+4.91);
   
//      road(x, y,z+8);
}

static void skybox(float dim) {       
   glDisable(GL_POLYGON_OFFSET_FILL);

   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
   glColor3f(0.7, 0.7, 0.7);
   glBindTexture(GL_TEXTURE_2D,_textureSkyboxFront);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glBegin(GL_QUADS);
   glNormal3f(0, 0, -1);
   glTexCoord2f(1.0,0.0); glVertex3f(+dim,-dim, dim);
   glTexCoord2f(0.0,0.0); glVertex3f(-dim,-dim, dim);
   glTexCoord2f(0.0,1.0); glVertex3f(-dim,+dim, dim);
   glTexCoord2f(1.0,1.0); glVertex3f(+dim,+dim, dim);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureSkyboxBack);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glBegin(GL_QUADS);
   glNormal3f(0, 0, 1);
   glTexCoord2f(1.0,0.0); glVertex3f(-dim,-dim, -dim);
   glTexCoord2f(0.0,0.0); glVertex3f(+dim,-dim, -dim);
   glTexCoord2f(0.0,1.0); glVertex3f(+dim,+dim, -dim);
   glTexCoord2f(1.0,1.0); glVertex3f(-dim,+dim, -dim);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureSkyboxRight);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(1.0,0.0); glVertex3f(dim,-dim, -dim);
   glTexCoord2f(0.0,0.0); glVertex3f(dim,-dim, +dim);
   glTexCoord2f(0.0,1.0); glVertex3f(dim,+dim, +dim);
   glTexCoord2f(1.0,1.0); glVertex3f(dim,+dim, -dim);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureSkyboxLeft);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glBegin(GL_QUADS);
   glNormal3f(1, 0, 0);
   glTexCoord2f(1.0,0.0); glVertex3f(-dim,-dim, +dim);
   glTexCoord2f(0.0,0.0); glVertex3f(-dim,-dim, -dim);
   glTexCoord2f(0.0,1.0); glVertex3f(-dim,+dim, -dim);
   glTexCoord2f(1.0,1.0); glVertex3f(-dim,+dim, +dim);
   glEnd();

   glBindTexture(GL_TEXTURE_2D,_textureSkyboxTop);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glBegin(GL_QUADS);
   glNormal3f(0, -1, 0);
   glTexCoord2f(1.0,0.0); glVertex3f(+dim,dim, +dim);
   glTexCoord2f(0.0,0.0); glVertex3f(-dim,dim, +dim);
   glTexCoord2f(0.0,1.0); glVertex3f(-dim,dim, -dim);
   glTexCoord2f(1.0,1.0); glVertex3f(+dim,dim, -dim);
   glEnd();
}

static void straightRoad(double tx, double ty, double tz, double roadLong, double degree, int edge)
{
    int xt, yt, zt;   
   
//   glPushMatrix();
    switch (edge) {
      case CUBE_TOP_LEFT :
        xt = tx + ROAD_WIDTH;
        yt = ty + ROAD_THICK;
        zt = tz - roadLong;   
        break;
        
      case CUBE_TOP_RIGHT :
        xt = tx + ROAD_WIDTH;
        yt = ty + ROAD_THICK;
        zt = tz + roadLong;   
        break;
        
      case CUBE_BOTTOM_LEFT :
        xt = tx - roadLong;
        yt = ty + ROAD_THICK;
        zt = tz - ROAD_WIDTH; 
        break;
        
      case CUBE_BOTTOM_RIGHT :
        xt = tx - ROAD_WIDTH;
        yt = ty + ROAD_THICK;
        zt = tz + roadLong; 
        break;
        
      default:
        xt = 0;
        yt = 0;
        zt = 0; 
        break;
    }

   glBindTexture(GL_TEXTURE_2D,_textureAsphalt);       
   glPushMatrix();      
      glTranslated(xt, yt, zt);
      glRotated(degree,0,1,0);
      glTranslated(-xt, -yt, -zt);
      cube(tx, ty, tz, roadLong, ROAD_THICK, ROAD_WIDTH, 0);
   glPopMatrix();
}

static void turnRoad(double tx, double ty, double tz, double roadLong){
     straightRoad(tx, ty, tz, 1, -15, CUBE_BOTTOM_LEFT);
     straightRoad(tx+1.5, ty, tz+0.15, 1, -30, CUBE_BOTTOM_LEFT);
     straightRoad(tx+3.8, ty, tz+1, 1, -45, CUBE_BOTTOM_LEFT);
     straightRoad(tx+5, ty, tz+3, 1, -60, CUBE_BOTTOM_LEFT);
     straightRoad(tx+6, ty, tz+4.2, 1, -75, CUBE_BOTTOM_LEFT);
}

static void circuit(){
       
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
   glColor3f(0.7, 0.7, 0.7);       
   //Main Road
   int i;
   int inc=2;
   for(i=0;i<6;i++){
      straightRoad(-11+inc, -0.05, -2, 2, 0, 0);
      inc+=4;
   }
   //Turn 1
   turnRoad(14, -0.05, -2, 1);
   //Straight 2
   glPushMatrix();
      glTranslated(19.3,0,7.8);
      glRotated(90,0,1,0);
      for(i=0;i<9;i++){
        straightRoad(-3+i, -0.05, -2, 1, 0, 0);
      }
   glPopMatrix();
   //Turn 2
   glPushMatrix();
      glTranslated(15.25,0,-1.5);
      glRotated(-90,0,1,0);
      turnRoad(14, -0.05, -2, 1);
   glPopMatrix();
   //Straight 3
   glPushMatrix();
      glTranslated(0.5,0,17.8);
       inc=2;
       for(i=0;i<6;i++){
         straightRoad(-11+inc, -0.05, -2, 2, 0, 0);
          inc+=4;
       }
   glPopMatrix();
   
   //Turn 3
   glPushMatrix();
      glTranslated(3,0,13.8);
      glRotated(-180,0,1,0);
      turnRoad(14, -0.05, -2, 1);
   glPopMatrix();
   //Straight 3
   glPushMatrix();
      glTranslated(-12.3,0,7.8);
      glRotated(90,0,1,0);
      for(i=0;i<9;i++){
        straightRoad(-3+i, -0.05, -2, 1, 0, 0);
      }
   glPopMatrix();
   //Turn 4
   glPushMatrix();
      glTranslated(-12.5,0,15.2);
      glRotated(-270,0,1,0);
      turnRoad(14, -0.05, -2, 1);
   glPopMatrix();    
}

void stand(double x, double y, double z, double height, double width, double lz){

     double initialStandz = 0.5;
     double initialStandy = 1.2;
     
     glBindTexture(GL_TEXTURE_2D,_textureGreyBrick);    
          
     glPushMatrix();
        cube(x, y+0.2, z, width, height-0.2, lz, 0);        
        texScale = 0.5;
        glBindTexture(GL_TEXTURE_2D,_textureWoodBeam);
        
        //Sit
        int i;
        for (i=0;i<5;i++){
            cube(x, y+initialStandy, z+initialStandz, width, 0.2, lz-initialStandz, 0); 
            
            initialStandz += 0.2;
            initialStandy += 0.2;   
        }
        
        cube(x, y+2.6, z+2, width, 0.2, lz-2, 0);
     glPopMatrix();
     
     glPushMatrix();
     glBindTexture(GL_TEXTURE_2D, _textureMetalRoof);
       glRotated(10, 1,0,0);
       glTranslated(0, 0.4,-0.65);
       cube(x, y+3, z, width, 0.1, lz, 0);                     
     glPopMatrix();
     
     
}

void grass(double x, double y, double z, double width){
        float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,1);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
   glColor3f(1, 1, 1);       
     glBindTexture(GL_TEXTURE_2D, _textureGrass);    
     glPushMatrix();
        cube(x, y, z, width, 0.17, width, 0);
     glPopMatrix();
}

void setLighting(){
     if(earth == DAY){
        Ambient[0] = 0.8;           Ambient[1] = 0.8;          Ambient[2] = 0.8;
        Diffuse[0] = 1;          Diffuse[1] = 1;          Diffuse[2] = 1;
        //Disable Lighting
        glDisable(GL_LIGHT1);     
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT3);    
        glDisable(GL_LIGHT4);
        glDisable(GL_LIGHT5);
        glDisable(GL_LIGHT6);
     }     
     else{
          //Setting For Night
          Ambient[0] = (20 / 255) * 0.8;           Ambient[1] = (60 / 255) * 0.8;          Ambient[2] = (180 / 255) * 0.8;
          Diffuse[0] = 0;          Diffuse[1] = 0;          Diffuse[2] = 0;
          
          //Car Light          
          glEnable(GL_LIGHT1);
          glEnable(GL_LIGHT2);
          glEnable(GL_LIGHT3);    
          glEnable(GL_LIGHT4);
          glEnable(GL_LIGHT5);
          glEnable(GL_LIGHT6);
          
          float amb[4] = {0,0,0,0};
          float dif[4] = {1,1,1,1}; //White
          float spec[4] = {0,0,0,1};
         
//Light lampu mobil
//float carPosition[4] = {-1+centerXIncrement,0.8,-2.7+centerZIncrement,1.0};

         float ligthStand1[4] = {-4,6,-2.7, 1.0};
         //White Light
         glLightfv(GL_LIGHT1,GL_AMBIENT ,amb);              glLightfv(GL_LIGHT1,GL_DIFFUSE ,dif);
         glLightfv(GL_LIGHT1,GL_SPECULAR,spec);              glLightfv(GL_LIGHT1,GL_POSITION,ligthStand1);
        
         glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION ,at0/100.0);              glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION   ,at1/100.0);
         glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,at2/100.0);
  
          float ligthStand2[4] = {5.5,6,-2.7, 1.0};
          //White Light
          glLightfv(GL_LIGHT2,GL_AMBIENT ,amb);          glLightfv(GL_LIGHT2,GL_DIFFUSE ,dif);
          glLightfv(GL_LIGHT2,GL_SPECULAR,spec);          glLightfv(GL_LIGHT2,GL_POSITION,ligthStand2);
        
          glLightf(GL_LIGHT2,GL_CONSTANT_ATTENUATION ,at0/100.0);          glLightf(GL_LIGHT2,GL_LINEAR_ATTENUATION   ,at1/100.0);
          glLightf(GL_LIGHT2,GL_QUADRATIC_ATTENUATION,at2/100.0);
          
          float ligthStand3[4] = {-4,6,15.7, 1.0};
          //White Light
          glLightfv(GL_LIGHT3,GL_AMBIENT ,amb);          glLightfv(GL_LIGHT3,GL_DIFFUSE ,dif);
          glLightfv(GL_LIGHT3,GL_SPECULAR,spec);          glLightfv(GL_LIGHT3,GL_POSITION,ligthStand3);
        
          glLightf(GL_LIGHT3,GL_CONSTANT_ATTENUATION ,at0/100.0);          glLightf(GL_LIGHT3,GL_LINEAR_ATTENUATION   ,at1/100.0);
          glLightf(GL_LIGHT3,GL_QUADRATIC_ATTENUATION,at2/100.0);
          
          float ligthStand4[4] = {5.5,6,15.7, 1.0};
          //White Light
          glLightfv(GL_LIGHT4,GL_AMBIENT ,amb);          glLightfv(GL_LIGHT4,GL_DIFFUSE ,dif);
          glLightfv(GL_LIGHT4,GL_SPECULAR,spec);          glLightfv(GL_LIGHT4,GL_POSITION,ligthStand4);
        
          glLightf(GL_LIGHT4,GL_CONSTANT_ATTENUATION ,at0/100.0);          glLightf(GL_LIGHT4,GL_LINEAR_ATTENUATION   ,at1/100.0);
          glLightf(GL_LIGHT4,GL_QUADRATIC_ATTENUATION,at2/100.0);
          
          float ligthStand5[4] = {-14.7,6,6.5, 1.0};
          //White Light
          glLightfv(GL_LIGHT5,GL_AMBIENT ,amb);          glLightfv(GL_LIGHT5,GL_DIFFUSE ,dif);
          glLightfv(GL_LIGHT5,GL_SPECULAR,spec);          glLightfv(GL_LIGHT5,GL_POSITION,ligthStand5);
        
          glLightf(GL_LIGHT5,GL_CONSTANT_ATTENUATION ,at0/100.0);          glLightf(GL_LIGHT5,GL_LINEAR_ATTENUATION   ,at1/100.0);
          glLightf(GL_LIGHT5,GL_QUADRATIC_ATTENUATION,at2/100.0);
          
          float ligthStand6[4] = {17.7,6,6.5, 1.0};
          //White Light
          glLightfv(GL_LIGHT6,GL_AMBIENT ,amb);          glLightfv(GL_LIGHT6,GL_DIFFUSE ,dif);
          glLightfv(GL_LIGHT6,GL_SPECULAR,spec);          glLightfv(GL_LIGHT6,GL_POSITION,ligthStand6);
        
          glLightf(GL_LIGHT6,GL_CONSTANT_ATTENUATION ,at0/100.0);          glLightf(GL_LIGHT6,GL_LINEAR_ATTENUATION   ,at1/100.0);
          glLightf(GL_LIGHT6,GL_QUADRATIC_ATTENUATION,at2/100.0);
     }
}

void control(int direction){
     int degrees ,rotates, turn;
     
     switch(direction){
        case 1:
             degrees = 0;             rotates = 1;             turn = 0;      
             step++;  
             break;                  
             
        case 2:
             degrees = 0;             rotates = -1;             turn = 0;             
             break;
             
        case 3:
             degrees = -ANGLE_TURN;    rotates = 1;              turn = -ANGLE_TURN;
             step++;
             break;
             
        default :
             degrees = ANGLE_TURN;    rotates = 1;              turn = ANGLE_TURN;
             break;   
     }
     
        temp = xRotate;
        xRotate = xRotate*Cos(degrees) + zRotate * Sin(degrees);
        zRotate = -temp*Sin(degrees)+zRotate*Cos(degrees);
        centerXIncrement += (xRotate * rotates);
        centerZIncrement += (zRotate * rotates);
        carRotate2 += turn;  
}

void personControl(int direction){
     int degree ,rotate, turn;
     
     switch(direction){
        case 1:
             degree = 0;             rotate = 1;             turn = 0;                  
             break;                  
             
        case 2:
             degree = 0;             rotate = -1;             turn = 0;             
             break;
             
        case 3:
             degree = -ANGLE_TURN;    rotate = 1;              turn = -ANGLE_TURN;
             break;
             
        default :
             degree = ANGLE_TURN;    rotate = 1;              turn = ANGLE_TURN;
             break;   
     }  
        temp = carRotateX;
        carRotateX = carRotateX*Cos(degree) + carRotateZ * Sin(degree);
        carRotateZ = -temp*Sin(degree)+carRotateZ*Cos(degree);
        carXIncrement += (carRotateX * rotate);
        carZIncrement += (carRotateZ * rotate);
        
        carRotate += turn;  
}

void carEnemy(){
     int i;

     if(step >= 25 && step < 40){
             control(3);                  
     }  
     else if(step >= 53 && step < 68){
             control(3);                  
     }
     else if(step >= 108 && step < 123){
             control(3);                  
     }
     else if(step >= 136 && step < 151){
             control(3);                  
     }
     else if(step == 166)
            step = 0;
     else
        control(1);
   
     printf("%d\n", step);      

}

void timer(int miliseconds) {
     
   	if(start)
        carEnemy();
	
	glutPostRedisplay();
	glutTimerFunc(CAR_MOVE, timer, 0);
}
/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //Enable Textures
   glEnable(GL_TEXTURE_2D);

   //  Undo previous transformations
   glLoadIdentity();
   
   // Light Position (Sun)
   float Position[]  = {0, 30, 0, 1.0};
   
   //  Enable lighting
   glEnable(GL_LIGHTING);
   //  Location of viewer for specular calculations
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
   //  glColor sets ambient and diffuse color materials
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
    //  Enable light 0 - Sun
   glEnable(GL_LIGHT0);
   
   //  Set ambient, diffuse, specular components and position of light 0 (Sun)
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);

	
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  First Person
   else
   {
      refX = ((dim * Sin(thf)) + fpX ) + carXIncrement;     
      refY = (dim * Sin(ph))+ fpY;
      refZ = (dim * -Cos(thf)) + fpZ + carZIncrement;
      printf("%f\n", refY);
   
      glRotated(-carRotate,0,1,0);      
         gluLookAt(-1+carXIncrement, 0.9 ,-2.7+carZIncrement , 8.210370+carXIncrement,refY,-3.058857+carZIncrement, 0,1,0);  

   }
   //Draw scene
   //Grass
   grass(1.5,-0.13,4,20);
   //Skybox
   skybox(64);
   
   //PitStop
   pitstop(1, 0, -10);   
   pitstop(5, 0, -10);
   pitstop(-3, 0, -10);
   
   //pit top to road
   glPushMatrix();
      glRotated(-40,0,1,0);
      glTranslated(-11.8,0,-7.8);
      road(15, 0, -0.09);
   glPopMatrix();
   
   glPushMatrix();
      glRotated(40,0,1,0);
      glTranslated(-16.5,0,-6.5);
      road(15, 0, -0.09);
   glPopMatrix();
   
   //Draw Circuit  
   circuit();
   
   //Light
   float redEm[4] = {1,1,1,1};
   glMaterialf(GL_FRONT,GL_SHININESS,0);
   glMaterialfv(GL_FRONT,GL_SPECULAR,redEm);
   glMaterialfv(GL_FRONT,GL_EMISSION,redEm);
   
   glColor3f(0.5, 0, 0);
   
   glPushMatrix();
       glBindTexture(GL_TEXTURE_2D,_textureHeadLamp);   
           cube(-4,4,0, 0.07,0.02,0.1, 0);
           cube(5.5,4,0, 0.07,0.02,0.1, 0);
   glPopMatrix();
   
   lightRoad(0,0,0);
   lightRoad(9.5,0,0);
   
   glPushMatrix();
      glRotated(90,0,1,0);
      lightRoad(-2.5,0,1.5);   
   glPopMatrix();
   
   glPushMatrix();
      glRotated(-90,0,1,0);
      lightRoad(10.5,0,-1.5); 
   glPopMatrix();
   
   //Stand
   texScale = 1.5;
   stand(1, 0.5, 2, 1, 8, 2);
   
   //Support
   texScale = 0.3;
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   
   glColor3f(0.7, 0.7, 0.7);
   glBindTexture(GL_TEXTURE_2D,_textureSupport);   
   cube(1, 0.5, -4, 6,0.3,0.1, 0);
   
      
                         /* Controlled Car */
   glPushMatrix();
      car(-1+carXIncrement,0.3,-2.7+carZIncrement, 1,1,1, carRotate, 0,0,0.8);
   glPopMatrix();
   
                                      /* Opponent's Car */
   glPushMatrix();
      car(-1+centerXIncrement,0.3,-1+centerZIncrement, 1,1,1, carRotate2, 0,0.8,0);
   glPopMatrix();
      
   texScale = 0.5;
   setLighting();

   glutSwapBuffers();
   
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{  
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;      
      
   
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
      
   th %= 360;   
   //  Update projection
   Project(fov,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Switch projection mode
   else if (ch == 'p' || ch == 'P')
      mode = 1-mode;
      
   // Switch Day   
   else if (ch == 'c' || ch == 'C')
      earth = 1-earth;
      
   else if(ch == 'w' || ch == 'W')
   {      
       if(!start)   
           start = 1;
           
       personControl(1);
   }
   
   else if(ch == 's' || ch == 'S')
   {      
       personControl(2);  
   }
      
   else if(ch == 'd' || ch == 'D')
      {
        personControl(3);          
      }
   else if(ch == 'a' || ch == 'A')
   {
         personControl(4);
   }   
         
   th %= 360;        
   //  Reproject
   Project(fov,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(fov,asp,dim);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(800,600);
   glutCreateWindow("UAS 171511023");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutTimerFunc(CAR_MOVE, timer, 0);
   
   initTexture();
   
   glutIdleFunc(idle);
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}

