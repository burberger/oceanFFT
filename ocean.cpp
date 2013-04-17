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

int size = 64;

// Shader
GLuint program;
bool program_on = true;
GLint time_var, resolution_var;  // handles to the "time" and "resolution" shader variables
float time_count = 0;   // value of the "time" shader variable

// Card buffer IDs
GLuint bufferIds[2];

// Display function - draw a teapot
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Modeling transformation
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, -20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
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

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, bufferIds[0]);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, bufferIds[1]);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, 0);

  glDrawElements(GL_TRIANGLES, size*size*6, GL_UNSIGNED_BYTE, 0);

  glDisableClientState(GL_VERTEX_ARRAY);

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

  //glBegin(GL_POLYGON);
    //glNormal3f(0.0, 1.0, 0.0);
    //for (int i = -64; i < 64; i++) {
        //for (int j = -64; j < 64; j++) {
            //glVertex3f(-1+i, -5.0, -1+j);
            //glVertex3f(-1+i, -5.0, 1+j);
            //glVertex3f(1+i, -5.0, 1+j);
            //glVertex3f(1+i, -5.0, -1+j);
        //}
    //}
  //glEnd();
  
  glutSwapBuffers();
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
  gluPerspective(60.0, (GLfloat) w / (GLfloat) h, 1.0, 200.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// Advance time
void idle(void)
{
  time_count += .02;
  glutPostRedisplay();
}

// Generates a square vertex map for the water and copies it to the graphics card
void buildWater(int size) {

    int vertSize = size*size*3;
    int indSize = vertSize*2;
    GLfloat* verticies = new GLfloat[vertSize];
    GLuint* indicies = new GLuint[indSize];
    int vertCount = 0;
    for (int i = -size/2; i <= size/2; i++) {
        for (int j = -size/2; j <= size/2; j++) {
            verticies[vertCount]   = i;
            verticies[vertCount+1] = 0;
            verticies[vertCount+2] = j;
            vertCount += 3;
        }
    }

    int squareCount = 0;
    for (int k = 0; k < indSize; k += 6) {
        // Calculate vertex indexes for current square
        int v[4];
        v[0] = squareCount + squareCount / size;
        v[1] = v[0] + 1;
        v[2] = v[1] + size;
        v[3] = v[2] + 1;

        // place vertex indicies in array for lookup
        // triangles in ccw winding
        indicies[k]   = v[1];
        indicies[k+1] = v[0];
        indicies[k+2] = v[2];
        indicies[k+3] = v[2];
        indicies[k+4] = v[3];
        indicies[k+5] = v[1];

        squareCount++;
    }

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, bufferIds[0]);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertSize, verticies, GL_STATIC_DRAW_ARB);

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, bufferIds[1]);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indSize, indicies, GL_STATIC_DRAW_ARB);
}

void init() {
  // Depth test
  glEnable(GL_DEPTH_TEST);
  // Simple Lighting
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glGenBuffersARB(2, bufferIds);
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
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Shader Tester");

  // Initialize extensions manager and set up shaders
  glewInit();
  program = loadShader(shaderfile);  // defined in the shader-load module
  glUseProgram(program);

  // Uniform shader variables to pass UI information to the shaders
  resolution_var = glGetUniformLocation(program,"resolution");
  time_var = glGetUniformLocation(program,"time");
  init();
  buildWater(size);

  
  // Attach event handlers
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutReshapeFunc(reshape);
  glutMainLoop();
  glDeleteBuffersARB(2, bufferIds);
  return 0; 
}
