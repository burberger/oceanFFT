/* Bob Urberger
 * Computer Graphics - Dr. Clair
 * Final Project: Realistic Water by use of FFT method
 *
 * Using the methods described in the 2002 Tessendorf paper
 * "Simulating Ocean Water", this program generates a height map
 * on the CPU which is then transferred to the GPU as a texture
 * for each frame.  This simplifies the implementation, but could be
 * made significantly faster by operating the FFT on the GPU as a
 * OpenCL or CUDA kernel. 
 * All equation and section refrences refer to locations in the Tessendorf paper
 */
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <ctime>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include "shader-load.h"
#include "vector.h"
#include "complex.h"

using namespace std;

// Rotation angles
GLint phi = 0;
GLint theta = 0;

// Constants for ocean field
int size = 1024; // Number of verticies for each side of the field, determines square resolution
float g = 9.81; // Gravity
float fixsize = 500.0f; // Fixed quad map length and width, regardless of square resolution
vector2 w(1.0, 1.0); // Wind speed
float A = 1.0; // Spectrum parameter, affects output height
int N = 32; // Frequency map size, has to be some multiple of two

struct height_norm {
    complex height;
    vector3 normal;
};

GLfloat* vertices;
GLuint* indicies;

int zoom = 0;
int height = 0;

// Shader
GLuint program;
bool program_on = true;
GLint time_var, resolution_var;  // handles to the "time" and "resolution" shader variables
float time_count = 0;   // value of the "time" shader variable
GLfloat lightPos[] = { 1.0, 0.0, -2.0, 1.0 };
GLfloat lightKa[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat lightKd[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat lightKs[] = { 1.0, 1.0, 1.0, 1.0 };

// RNG seed
gsl_rng * rng;

// Card buffer IDs
GLuint bufferIds[2];

// Display function - draw a teapot
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Modeling transformation
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 0.0 + height, 20.0 - zoom, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glRotated(phi,1,0,0);
  glRotated(theta,0,1,0);
  
  // Provide both material and color so shaders can use either
  GLfloat color[4] = {.8,.4,.1,0};
  GLfloat specular[4] = {1,1,1,0};
  glColor4fv(color);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE,color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,specular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,20);


  
  // Send the time variable to the shader
  glUniform1f(time_var,time_count);

  // Bind buffers, inform opengl of structure of verticies
  glBindBuffer(GL_ARRAY_BUFFER, bufferIds[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIds[1]);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, 0);

  // Draw triangle strips by index
  glDrawElements(GL_TRIANGLE_STRIP, (2*size+1)*(size - 1) - 1, GL_UNSIGNED_INT, 0);

  glDisableClientState(GL_VERTEX_ARRAY);

  // Release buffers
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glutSolidTeapot(1.0);

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
  case '+':
    zoom++;
    break;
  case '-':
    zoom--;
    break;
  case 'u':
    height++;
    break;
  case 'o':
    height--;
    break;
  case 'q':
    exit(0);
  default:
    return;
  }
  x = y; // shuts up the automatic warning system, clean me up before submitting
  glutPostRedisplay();
}

// Window resize
void reshape(int w, int h) {
  // Inform shader program of new resolution
  // by setting "resolution" variable
  glUniform2f(resolution_var,w,h);

  // Set up view and projection
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (GLfloat) w / (GLfloat) h, 0.1, 200.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// Advance time
void timerUpdate(int value) {
    time_count += 0.02;
}

// As wrong as this seems, this gives relatively smooth time updates, where
// re-registering the timer from within the callback results in noticeable stuttering
void idle(void) {
    glutTimerFunc(20, timerUpdate, 0);
    glutPostRedisplay();
}


// Generates a square vertex map for the water and copies it to the graphics card
void buildWater(int size) {
    // Total number of vertex coordinate values
    int vertSize = size*size*3;
    // Number of vertex refrences, including geometry restart indicators
    int indSize = (2*size+1)*(size - 1) - 1;

    vertices = new GLfloat[vertSize];
    indicies = new GLuint[indSize];

    // Calculate vertex positions from defined size limits
    // This will fit specified square count in defined size
    int curVert = 0;
    float dist = float (fixsize/(size-1));
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            vertices[curVert++] = -fixsize/2 + j*dist;
            vertices[curVert++] = -5;
            vertices[curVert++] = fixsize/2 - i*dist;
        }
    }

    // Calculate index refrences to actually draw the triangles
    int indCount = 0; 
    for (int row = 0; row < size - 1; row++) {
        for (int col = 0; col < size; col++) {
            indicies[indCount++] = row*size + col;
            indicies[indCount++] = row*size + size + col;
        }
        // If end of row, and not last row, mark it with identifier
        if (row != size-2) {
            indicies[indCount++] = vertSize;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, bufferIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertSize, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indSize, indicies, GL_STATIC_DRAW);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndexNV((GLuint)vertSize);

    delete[] indicies;
}

// Model the periodic dispersion relation, Section 3.2
float dispersion(int n, int m) {
    // Define base frequency at which to loop the dispersion
    float w0 = 2 * M_PI / 200.0f;
    // Calculate K vector from current location in 2D frequency grid
    float kx = 2 * M_PI * n / fixsize;
    float kz = 2 * M_PI * m / fixsize;
    // Equation 18
    return floor(sqrt(g * sqrt(kx * kx + kz * kz)) / w0) * w0;
}

// Phillips Spectrum modulated by wind speed and direction
// Section 3.3, equation 23
float phillips(int n, int m) {
    vector2 k(2 * M_PI * n / fixsize, 2 * M_PI * m / fixsize);
    float k_len = k.length();
    // Do nothing if frequency is excessively small, saves time
    if (k_len < 0.000001) {
        return 0.0;
    }

    // |k|^4
    float k_len2 = k_len * k_len;
    float k_len4 = k_len2 * k_len2;
    
    // |k dot w|^2
    float kw = k.normalize() * w.normalize();
    kw = kw * kw;

    // L, L^2
    float w_len = w.length();
    float Lsq = w_len * w_len / g;
    Lsq = Lsq * Lsq;

    // Damping term for eqation 24
    float damping = 0.001;
    float l2 = Lsq * damping * damping;
    
    // Equation 23, damped by eq. 24
    return A * exp(-1.0f / (k_len2 * Lsq)) / k_len4 * kw * exp(-k_len2 * l2);
}

// Generate the fourier amplitude of the height field at specified fequency vector k
// Produces results in the complex frequency domain
// Section 3.4
complex ht_0(int n, int m) {
    // produces gaussian random draws with mean 0 and std dev 1
    double a = gsl_ran_gaussian(rng, 1.0);
    double b = gsl_ran_gaussian(rng, 1.0);
    complex r(a, b);
    // Equation 25
    return r * sqrt(phillips(n, m) / 2.0);
}

// Generate the fourier amplitudes of the wave field
// Equation 26
complex ht(float t, int n, int m) {
    complex ht0 = ht_0(n, m);
    complex ht0conj = ht_0(-n, -m).conj();
    
    // complex exponential is calculated by euler's identity
    // for faster computation
    float omegat = dispersion(n, m) * t;
    float real = cos(omegat);
    float cmp = sin(omegat);

    complex c0(real, cmp);
    complex c1(real, -cmp);
    return ht0 * c0 + ht0conj * c1;
}

// Computes a height and a normal value for an individual coordinate, and returns them
// packed into a struct.  These are evaluated by Equation 19 for height, and
// Equation 20 for normal
height_norm heightAndNormal(vector2 x, float t) {
    complex height(0.0f, 0.0f);
    vector3 normal(0.0f, 0.0f, 0.0f);
    
    float kx, kz, kDx;
    vector2 k;
    complex c, res, ht_c;
    // Discrete fourier transform over frequency plane described by vector k
    // Equation 19
    for (int m = -N/2; m < N/2; ++m) {
        kz = 2.0f * M_PI * m / fixsize;
        for (int n = -N/2; n < N/2; ++n) {
            // K vector is generated for this frequency point
            kx = 2.0f * M_PI * n / fixsize;
            k = vector2(kx, kz);

            // frequency dot location, then compute exponential
            kDx = k * x;
            // Euler identity resolution of exponential
            c = complex(cos(kDx), sin(kDx));
            // Solve full equation for this iteration of the frequency plane
            ht_c = ht(t, n, m) * c;

            // Sum the height value
            height = height + ht_c;
            // Sum the normal value, converted into a proper vector
            // Equation 20
            normal = normal + vector3(-kx * ht_c.b, 0.0f, -kz * ht_c.b);
        }
    }
    height_norm hn;
    hn.height = height;
    hn.normal = normal;
    return hn;
}

void init() {
    // Depth test
    glEnable(GL_DEPTH_TEST);
    // Simple Lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glGenBuffersARB(2, bufferIds);
}

// Main code - all initialization is here
int main(int argc, char** argv)
{
    // Initialize OpenGL
    glutInit(&argc, argv);

    // Make window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Ocean View");

    // Initialize extensions manager and set up shaders
    glewInit();
    program = loadShader("ocean");  // defined in the shader-load module
    glUseProgram(program);

    // Uniform shader variables to pass UI information to the shaders
    resolution_var = glGetUniformLocation(program,"resolution");
    time_var = glGetUniformLocation(program,"time");

    rng = gsl_rng_alloc(gsl_rng_taus);
    gsl_rng_set(rng, time(0));

    init();
    buildWater(size);

    // Attach event handlers
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    glutMainLoop();
    glDeleteBuffersARB(2, bufferIds);
    gsl_rng_free(rng);
    return 0; 
}
