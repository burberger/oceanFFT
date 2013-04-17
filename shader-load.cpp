//
// shader-load.cpp
// Bryan Clair 2013
// Based pretty closely on the simple GLSL demo from www.lighthouse3d.com
//

#include <iostream>
#include <fstream>
#include <string>

#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//
// Read file into a char array
//
char *read_file(std::string filename) {
  std::ifstream is (filename.c_str(), std::ifstream::binary);
  if (is) {
    // get length of file:
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);
    
    char * buffer = new char [length+1];
    // read data as a block:
    is.read (buffer,length);
    
    if (!is)
      std::cerr << "error reading file " << filename << std::endl;
    is.close();
    buffer[length] = '\0'; //null terminate
    return buffer;
  }
  return NULL;
}

//
// Utility function checks the info log for a shader and prints it if nonempty.
// Returns true if errors found.
//
bool reportShaderErrors(GLuint shader)
{
  int infologLength = 0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH,&infologLength);
  if (infologLength > 1) {
    char *infoLog = new char[infologLength];
    glGetShaderInfoLog(shader, infologLength, NULL, infoLog);

    GLint shadertype;
    glGetShaderiv(shader,GL_SHADER_TYPE,&shadertype);
    std::cerr << "Error compiling ";
    switch (shadertype) {
    case GL_VERTEX_SHADER:
      std::cerr << "vertex";
      break;
    case GL_FRAGMENT_SHADER:
      std::cerr << "fragment";
      break;
    default:
      std::cerr << "unknown type";
    }
    std::cerr << " shader:\n";
    std::cerr << infoLog << std::endl;

    delete[] infoLog;
    return true;
  }
  return false;
}

//
// Utility function checks the info log for a program and prints if nonempty.
// Returns true if errors found.
//
bool reportProgramErrors(GLuint program)
{
  int infologLength = 0;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH,&infologLength);
  if (infologLength > 1) {
    char *infoLog = new char[infologLength];
    std::cerr << "Error linking shader program:\n";
    glGetProgramInfoLog(program, infologLength, NULL, infoLog);
    std::cerr << infoLog << std::endl;
    delete[] infoLog;
    return true;
  }
  return false;
}

//
// Load shaders, compile them, attach to a program.
//   basename is the basename of the shader files to load.
//   This will look for and load a vertex shader file with extension .vert
//   and look for and load a fragment shader file with extension .frag
//
//   Returns a GLuint which is the compiled program.
//
GLuint loadShader(const char *basename) {
  const char *code;
  std::string filename;

  GLuint vertex_shader;
  bool vertex = false;
  filename = std::string(basename)+".vert";
  code = read_file(filename);
  if (code) {
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &code, NULL);
    glCompileShader(vertex_shader);
    if (!reportShaderErrors(vertex_shader)) vertex = true;
    delete[] code;
  } else
    std::cerr << "File " << filename << " not read, using default vertex shader.\n";

  GLuint fragment_shader;
  bool fragment = false;
  filename = std::string(basename)+".frag";
  code = read_file(filename);
  if (code) {
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &code, NULL);
    glCompileShader(fragment_shader);
    if (!reportShaderErrors(fragment_shader)) fragment = true;
    delete[] code;
  } else
    std::cerr << "File " << filename << " not read, using default fragment shader.\n";

  GLuint program = glCreateProgram();
  if (vertex) glAttachShader(program,vertex_shader);
  if (fragment) glAttachShader(program,fragment_shader);
  glLinkProgram(program);
  reportProgramErrors(program);

  return program;
}
