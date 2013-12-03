#version 420

/* --------------- INPUT VARIABLES -------------- */
/* In a vertex shader, they are vertex attributes */
/* location=... informs the shader of the index   */
/* of the attribute - it has to be in agreement   */
/* with the first argument of attachAttribute()   */
/* in the CPU code                                */
/* ---------------------------------------------- */

layout (location=0) in vec2 model_coord;
layout (location=1) in vec2 wave_dir;
layout (location=2) in vec3 color;

/* -------------- OUTPUT VARIABLES -------------- */
/* Attributes of the processed vertices           */
/* Interpolated by the rasterizer and sent with   */
/* fragments to the fragment shader               */
/* ---------------------------------------------- */

// x- and y- coordinates scaled to 0...1
// since we are using parallel projection, linear
// interpolation in screen space is the same
// as perspectively correct interpolation (and cheaper),
// so this is what we are requesting here

noperspective out vec2 param;

// this will be set to wave_dir;
// all vertices of any triangle have the same value
// of this attribute, so flat interpolation can
// be used to save some time

flat out vec2 wdir;

// same for color

flat out vec3 col;

// if = 2 we will rotate the wave in the fragment shader

flat out int instance;

/* ---------------------------------------------- */
/* ----------- MAIN FUNCTION -------------------- */
/* goal: set gl_Position (location of the         */
/* projected vertex in homogenous coordinates)    */
/* and values of the output variables             */
/* ---------------------------------------------- */

void main()
{
  // vertices of the square scaled to 0...1

  param = model_coord;
  wdir = wave_dir;
  col = color;

  // gl_InstanceID is a built-in input variable that tells
  //  which instance the vertex belongs to
  // In your project, you probably won't need it, but it's
  //  a nice thing to know that it exists :)
  // see GLSL specification for other built-in variables

  instance = gl_InstanceID;

  // just add 0 z-coordinate and 1 homogenous coordinate

  gl_Position = vec4(model_coord,0,1);
}
