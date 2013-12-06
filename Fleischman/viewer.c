
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <string>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>

#include "program.h"
#include "vertexarray.h"
#include "shader.h"
#include "texture.h"

using namespace gl_CSCI441;
using namespace std;
using namespace glm;

/* ----------------------------------------------------- */

/* --------------------------------------------- */
/* ----- GLOBAL VARIABLES  --------------------- */
/* --------------------------------------------- */

static const GLint VPD_DEFAULT = 800;
static GLint wid;               /* GLUT window id; value asigned in main() and should stay constant */
static GLint vpw = VPD_DEFAULT; /* viewport dimensions; changed when window is resized (resize callback) */
static GLint vph = VPD_DEFAULT;

static GLint number_of_sites = 32;
static const GLint max_number_of_sites = 65536;

static bool rand_colors = true;

static bool show_sites = true;

/* ----------------------------------------------------- */

// vertics of the square extending from -1 to 1 in x and y
// order suitable for rendering as a triangle strip

GLfloat square_vertices[] = {
  -1, -1,
  -1,  1,
   1, -1,
   1,  1
};


/* ----------------------------------------------------- */

// all buffer and program objects used 

VertexArray *va_square = NULL;
Buffer *buf_square_vertices = NULL;
Buffer *buf_rand_color = NULL;
Buffer *buf_site_loc = NULL;
Buffer *buf_angle_velocity = NULL;

Program *square_program = NULL;

// texture objects - you'll need to have one for the image lookup mode
// shaders provided here don't use it - see project 3 sample code to
// see what to do

RGBTexture2D *t2D = NULL;

/* ----------------------------------------------------- */

void setup_textures()
{
  // reads a PPM image from file and create a texture object
  //  last three values are border color (in the project, there is no need to use borders)
  t2D = createRGBTexture2D("input.ppm",0.5,0.25,0.25);
  t2D->linear();        // request bilinear interpolation
  t2D->attach(1);       // attach to TAP #1; must match the binding parameter in the shaders
                        // see `layout (binding=1) uniform sampler2D tex;' in fsh_square.glsl
  t2D->on();            // turn on the texture; has to be called after any parameter change - 
                        //  otherwise the texture will be off
}

/* ----------------------------------------------------- */

void setup_buffers()
{
  // Fhe first argument to the Buffer constructor is the number of 
  // components per vertex (basically, numbers per vertex)
  // For square, vertices are 2D - they have 2 coordinates; hence 
  //  the number of components is 2, and there are 4 vertices.
  // The last argument is a pointer to the actual vertex data.
  buf_square_vertices = new Buffer(2,4,square_vertices);

  // randomly generate maximum possible site locations
  GLfloat *site_locs = new GLfloat[2*max_number_of_sites];

  // random multipliers for angle of rotation
  GLfloat *rand_angle_velocities = new GLfloat[max_number_of_sites];

  // random rgb colors per instance (cone)
  GLfloat *rand_colors = new GLfloat[max_number_of_sites*3];

  for (int i = 0; i < max_number_of_sites; i++) {
    site_locs[2*i+0] = 1-2*drand48();
    site_locs[2*i+1] = 1-2*drand48();

    rand_colors[3*i+0] = drand48();
    rand_colors[3*i+1] = drand48();
    rand_colors[3*i+2] = drand48();

    rand_angle_velocities[i] = drand48()*1.5 + 0.1;
  }

  buf_site_loc = new Buffer(2, max_number_of_sites, site_locs);
  buf_rand_color = new Buffer(3, max_number_of_sites, rand_colors);
  buf_angle_velocity = new Buffer(1, max_number_of_sites, rand_angle_velocities);

  
  // construct the square VA
  va_square = new VertexArray;

  // vertices are attribute #0
  va_square->attachAttribute(0,buf_square_vertices);

  // the wave direction attribute is an instanced attribute
  //  hence we use version of attachAttribute with 3 arguments
  //  the last one is the number of instances that needs to pass
  //  between updates of the attribute. Here, we use 1 which
  //  means all vertices in ith instance will use index i
  //  to look up the attribute value; if the value is d,
  //  vertices in the ith instance would use index i/d 
  //  (integer division is used here)
  
  va_square->attachAttribute(1, buf_site_loc, 1);

  // color is similar
  va_square->attachAttribute(2, buf_rand_color, 1);

  // as is angle velocities
  va_square->attachAttribute(3, buf_angle_velocity, 1);
}

/* ----------------------------------------------------- */

void setup_programs()
{
  cout << "Creating the square program..." << endl;
  square_program = createProgram("shaders/vsh_square.glsl","shaders/fsh_square.glsl");
}

/* ----------------------------------------------------- */

bool animate = true;    // animate or not
float multiplier = 0.02; // controls rotation speed
int dcounter = 1;       // used to increment the frame counter (set to zero to freeze)

/* ----------------------------------------------------- */

void draw()
{
  // ensure we're drawing to the correct GLUT window 
  glutSetWindow(wid);

  // frame counter for animation
  static int counter = 0;
  counter += dcounter;

  // rotation angle
  static GLfloat angle = 0.0;
  angle += multiplier;

  // clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // No need to do culling in this project - we want to use all 
  // fragments anyway to ensure correctness
  glDisable(GL_CULL_FACE);

  // want to use depth test to get visibility right
  glEnable(GL_DEPTH_TEST);

  square_program->setUniform("angle",angle);
  square_program->setUniform("show_sites",show_sites);
  square_program->setUniform("rand_colors",rand_colors);

  // turn on the square program...
  square_program->on();

  // Send vertices 0...3 to pipeline; this is an instanced variant
  // The first argument instructs the pipeline how to set up triangles
  // last one is the number of instances (number of sites)
  va_square->sendToPipeline(GL_TRIANGLE_STRIP,0,4,number_of_sites);

  // turn the program off
  square_program->off();

  // make sure all the stuff is drawn
  glFlush();

  // this exchanges the invisible back buffer with the visible buffer to 
  //  avoid refresh artifacts
  glutSwapBuffers();

  // Request another call to draw() in the "animation" mode.
  // Note that in the viewer you'll be implementing you may not need this - 
  //  all you have to do is to make sure the image is redrawn when rendering parameters change,
  //  e.g. as a result of mouse events.
  if (animate) 
    glutPostRedisplay();
}


/* ----------------------------------------------------- */

/* --------------------------------------------- */
/* --- INTERACTION ----------------------------- */
/* --------------------------------------------- */

/* menu callback */

// you'll need to change this one as well...

static const int MENU_MORE_POINTS = 1;
static const int MENU_TOGGLE_SITES = 2;
static const int MENU_RESET = 3;
static const int MENU_MOVE_POINTS = 4;
static const int MENU_TOGGLE_COLORING= 5;

void menu ( int value )
{
  static GLfloat stored_multiplier = 0;
  static int stored_dcounter;

  switch(value)
    {
    case MENU_MORE_POINTS:
      number_of_sites = glm::min(number_of_sites * 2, max_number_of_sites);
      break;
    case MENU_TOGGLE_SITES:
      show_sites = !show_sites;
      break;
    case MENU_RESET:
      number_of_sites = 32;
      show_sites = true;
      if(animate) {
        menu(MENU_MOVE_POINTS);
      }
      rand_colors = true;
      break;
    case MENU_MOVE_POINTS:
      animate = !animate;
      // this is to make sure that when animation is off, things don't move even if
      //  glutPostRedisplay comes up somewhere (e.g. in a mouse event)
      swap(dcounter,stored_dcounter);
      swap(multiplier,stored_multiplier);
      break;
    case MENU_TOGGLE_COLORING:
      rand_colors = !rand_colors;
      break;
    }

  // and again, in case any rendering paramters changed, redraw
  glutPostRedisplay();
}

/* --------------------------------------------- */

/* handle keyboard events; here, just exit if ESC is hit */

void keyboard(GLubyte key, GLint x, GLint y)
{
  switch(key) {
  case 27:  /* ESC */
    
    // clean up and exit
    // you may remove these deletes and let the OS do the work

    delete va_square;
    delete square_program;
    delete buf_square_vertices;
    delete buf_site_loc;
    delete buf_rand_color;
    delete t2D;

    exit(0);

  default:  break;
  }
}

/* --------------------------------------------- */

/* handle resizing the glut window */

GLvoid reshape(GLint sizex, GLint sizey)
{
  glutSetWindow(wid);

  // keep the viewport square...
  vpw = sizex < sizey ? sizex : sizey;
  vph = vpw;

  glViewport(0, 0, vpw, vph);
  glutReshapeWindow(vpw, vph);

  glutPostRedisplay();
}

/* --------------------------------------------- */
/* -------- SET UP GLUT  ----------------------- */
/* --------------------------------------------- */

// initialize glut, callbacks etc.

GLint init_glut(GLint *argc, char **argv)
{
  GLint id;

  glutInit(argc,argv);

  /* size and placement hints to the window system */
  glutInitWindowSize(vpw, vph);
  glutInitWindowPosition(10,10);

  /* double buffered, RGB color mode */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  /* create a GLUT window (not drawn until glutMainLoop() is entered) */
  id = glutCreateWindow("Zach Fleischman Graphics Project 4");    

  /* register callbacks */

  /* window size changes */
  glutReshapeFunc(reshape);

  /* keypress handling when the current window has input focus */
  glutKeyboardFunc(keyboard);

  /* mouse event handling - no need for it here */
  glutMouseFunc(NULL);          
  glutMotionFunc(NULL);      

  /* window obscured/revealed event handler */
  glutVisibilityFunc(NULL);

  /* handling of keyboard SHIFT, ALT, CTRL keys */
  glutSpecialFunc(NULL);

  /* what to do when mouse cursor enters/exits the current window */
  glutEntryFunc(NULL);

  /* what to do on each display loop iteration */
  glutDisplayFunc(draw);

  /* create menu */
  // you'll need to change this to build your menu
  GLint menuID = glutCreateMenu(menu);
  glutAddMenuEntry("Spray more points",MENU_MORE_POINTS);
  glutAddMenuEntry("Show/hide sites",MENU_TOGGLE_SITES);
  glutAddMenuEntry("Reset",MENU_RESET);
  glutAddMenuEntry("Toggle moving sites",MENU_MOVE_POINTS);
  glutAddMenuEntry("Toggle coloring method",MENU_TOGGLE_COLORING);
  glutSetMenu(menuID);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  return id;
}

/* --------------------------------------------- */
/* --------------------------------------------- */
/* --------------------------------------------- */

GLint main(GLint argc, char **argv)
{
  /* initialize GLUT: register callbacks, etc */
  wid = init_glut(&argc, argv);

  // initialize glew and check for OpenGL 4.2 support
  glewInit();
  if (glewIsSupported("GL_VERSION_4_0"))
    cout << "Ready for OpenGL 4.0" << endl;
  else 
    {
      cout << "OpenGL 4.0 not supported" << endl;;
      return 1;
    }

  // initialize programs and buffers
  setup_programs();
  setup_buffers();

  // setup textures
  setup_textures();

  menu(MENU_MOVE_POINTS);

  // Main loop: keep processing events.
  // This is actually an indefinite loop - you can only exit it using 
  // brutal means like the exit() function in one of the event handlers.
  // In this code, the exit point is in the keyboard event handler.
  glutMainLoop();

  return 0;
}


/* --------------------------------------------- */
