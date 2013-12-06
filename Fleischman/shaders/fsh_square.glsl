#version 420


/* ------------- UNIFORM VARIABLES -------------- */
/* This is `global state' that every invocation   */
/* of the shader has access to.                   */
/* Note that these variables can also be declared */
/* in the fragment shader if necessary.           */
/* If the names are the same, the same value will */
/* be seen in both shaders.                       */
/* ---------------------------------------------- */

uniform int show_sites;
uniform int rand_colors;

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

noperspective in vec2 coord;
flat in vec2 site;
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
  

  // alter depth depending on where you are (coord) and site location
  float dist = distance(site, coord) / 3.0;
  gl_FragDepth = dist;

  // col is also an instanced attribute - color each instance
  // using a constant color
  // scale texture coordinates to [-1,1][-1,1]
  vec2 tex_coord = 0.5 * (site + vec2(1.0,1.0));
  tex_coord = vec2(tex_coord.x, 1-tex_coord.y);

  if ((dist < 0.002) && (show_sites == 1)) {
    fragcolor = vec3(0.f,0.f,0.f);
  } else {
    if (rand_colors == 1) {
      fragcolor = col;
    } else {
      vec3 texture_vals = texture(tex,tex_coord).rgb;
      fragcolor = vec3(
        texture_vals.x,
        texture_vals.y, 
        texture_vals.z);
    }
  }
}
