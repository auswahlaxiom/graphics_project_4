#version 420


/* ------------- UNIFORM VARIABLES -------------- */
/* This is `global state' that every invocation   */
/* of the shader has access to.                   */
/* Note that these variables can also be declared */
/* in the fragment shader if necessary.           */
/* If the names are the same, the same value will */
/* be seen in both shaders.                       */
/* ---------------------------------------------- */

uniform float angle;

/* --------------- SAMPLER UNIFORMS ------------- */
/* The binding layout parameter needs to be the   */
/* texture attachment point (TAP) the texture     */
/* is attached to in the host code                */
/* ---------------------------------------------- */

layout (binding=1) uniform sampler2D tex;

/* --------------- INPUT VARIABLES -------------- */
/* In a fragment shader, attributes sent out with */
/* processed vertices in the vertex shader        */
/* and interpolated on the rasterization stage    */
/* ---------------------------------------------- */

noperspective in vec2 param;
flat in vec2 wdir;
flat in vec3 col;
flat in int instance;

/* ----------- OUTPUT VARIABLES ----------------- */
/* For `simple' rendering we do here, there is    */ 
/* just one: RGB value for the fragment           */
/* ---------------------------------------------- */

out vec3 fragcolor;

/* ---------------------------------------------- */
/* ----------- MAIN FUNCTION -------------------- */
/* goal: compute the color of the fragment        */
/*  [put it into the only output variable]        */
/* ---------------------------------------------- */

void main()
{
  vec2 dir = wdir;

  // rotate the direction vector only for instance #2

  if (instance==2)
  {
    dir = vec2(cos(angle)*wdir.x - sin(angle)*wdir.y, cos(angle)*wdir.y + sin(angle)*wdir.x);
  }

  // alter depth depending on where you are (param) and 
  // dir (which is an instanced attribute value passed here
  // from the vertex shader)

  gl_FragDepth = 0.5*(1+sin(10/(0.3+abs(dot(param,dir)))));

  // col is also an instanced attribute - color each instance
  // using a constant color

  fragcolor = col;
}
