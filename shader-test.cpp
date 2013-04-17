/*
Usage: shader-test [basename]
        where [basename] is the name of the shader program, without .frag/.vert suffix.

The program will load and enable fragment shader basename.frag (if it exists) and
load and enable vertex shader basename.vert (if it exists).

When running, the s key toggles the shader on and off.

It displays a teapot, which can be rotated with the ijkl keys.
The teapot is given a color and material.  LIGHT_0 is set up.

Finally, there are two variables set up for access by the shader:

uniform float time
   Initially 0, increments by a small amount each frame (0.05)

uniform vec2 resolution
   The (width,height) of the viewport in pixels

Bryan Clair 2013
*/
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "shader-load.h"

// Rotation angles
GLint phi = 0;
GLint theta = 0;

// Shader
GLuint program;
bool program_on = true;
GLint time_var, resolution_var;  // handles to the "time" and "resolution" shader variables
float time_count = 0;   // value of the "time" shader variable

// Display function - draw a teapot
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Modeling transformation
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -3);
  glRotated(phi,1,0,0);
  glRotated(theta,0,1,0);
  
  // Provide both material and color so shaders can use either
  GLfloat color[4] = {.8,.4,.1,0};
  GLfloat specular[4] = {1,1,1,0};
  glColor4fv(color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,20);
  
  // Send the time variable to the shader
  glUniform1f(time_var,time_count);
  
  glutSolidTeapot(1);
  
  glFlush();
}

void keyboard (unsigned char key, int x, int y)
{
  switch (key) {
  case 'j':
    theta -= 2;
    break;
  case 'l':
    theta += 2;
    break;
  case 'i':
    phi -= 2;
    break;
  case 'k':
    phi += 2;
    break;
  case 's':
    if (program_on)
      glUseProgram(0);
    else
      glUseProgram(program);
    program_on = !program_on;
    break;
  case 'q':
    exit(0);
  default:
    return;
  }
  glutPostRedisplay();
}

// Window resize
void reshape(int w, int h)
{
  // Inform shader program of new resolution
  // by setting "resolution" variable
  glUniform2f(resolution_var,w,h);

  // Set up view and projection
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, 1.0, 1.0, 30.0);
  glMatrixMode(GL_MODELVIEW);
}

// Advance time
void idle(void)
{
  time_count += .02;
  glutPostRedisplay();
}

// Main code - all initialization is here
int main(int argc, char** argv)
{
  // Initialize OpenGL
  glutInit(&argc, argv);
  
  // Check args
  char *shaderfile;
  if (argc != 2) {
    std::cerr << "usage: shader-test shadername\n";
    exit(1);
  }
  shaderfile = argv[1];

  // Make window
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Shader Tester");

  // Initialize extensions manager and set up shaders
  glewInit();
  program = loadShader(shaderfile);  // defined in the shader-load module
  glUseProgram(program);

  // Uniform shader variables to pass UI information to the shaders
  resolution_var = glGetUniformLocation(program,"resolution");
  time_var = glGetUniformLocation(program,"time");

  // Depth test
  glEnable(GL_DEPTH_TEST);

  // Simple Lighting
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  
  // Attach event handlers
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0; 
}
