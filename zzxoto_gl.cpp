#include "zzxoto_gl.h"
#include <stdio.h>
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#endif

type_glTexImage2DMultisample *glTexImage2DMultisample;
type_glBindFramebuffer *glBindFramebuffer;
type_glGenFramebuffers *glGenFramebuffers;
type_glFramebufferTexture2D *glFramebufferTexture2D;
type_glCheckFramebufferStatus *glCheckFramebufferStatus;
type_glBlitFramebuffer *glBlitFramebuffer;
type_glAttachShader *glAttachShader;
type_glCompileShader *glCompileShader;
type_glCreateProgram *glCreateProgram;
type_glCreateShader *glCreateShader;
type_glLinkProgram *glLinkProgram;
type_glShaderSource *glShaderSource;
type_glUseProgram *glUseProgram;
type_glGetProgramInfoLog *glGetProgramInfoLog;
type_glGetShaderInfoLog *glGetShaderInfoLog;
type_glValidateProgram *glValidateProgram;
type_glGetProgramiv *glGetProgramiv;
type_glGetUniformLocation *glGetUniformLocation;
type_glUniform4fv *glUniform4fv;
type_glUniformMatrix4fv *glUniformMatrix4fv;
type_glUniform1i *glUniform1i;
type_glUniform1f *glUniform1f;
type_glUniform2fv *glUniform2fv;
type_glUniform3fv *glUniform3fv;
type_glEnableVertexAttribArray *glEnableVertexAttribArray;
type_glDisableVertexAttribArray *glDisableVertexAttribArray;
type_glGetAttribLocation *glGetAttribLocation;
type_glVertexAttribPointer *glVertexAttribPointer;
type_glBindVertexArray *glBindVertexArray;
type_glGenVertexArrays *glGenVertexArrays;
type_glBindBuffer *glBindBuffer;
type_glGenBuffers *glGenBuffers;
type_glBufferData *glBufferData;
type_glBufferSubData *glBufferSubData;
type_glActiveTexture *glActiveTexture;
type_glDeleteProgram *glDeleteProgram;
type_glDeleteShader *glDeleteShader;
type_glDeleteFramebuffers *glDeleteFramebuffers;
type_glDrawBuffers *glDrawBuffers;
type_glGetStringi *glGetStringi;
type_glDetachShader *glDetachShader;
type_glDeleteBuffers *glDeleteBuffers;
type_glDeleteVertexArrays *glDeleteVertexArrays;
type_glBlendEquationSeparate *glBlendEquationSeparate;
type_glBlendFuncSeparate *glBlendFuncSeparate;
type_glGetShaderiv *glGetShaderiv;
type_glBlendEquation *glBlendEquation;
type_glGetString *glGetString;
type_glGetIntegerv *glGetIntegerv;
type_glEnable *glEnable;
type_glDisable *glDisable;
type_glViewport *glViewport;
type_glIsEnabled *glIsEnabled;
type_glTexParameteri *glTexParameteri;
type_glBindTexture *glBindTexture;
type_glScissor *glScissor;
type_glDeleteTextures *glDeleteTextures;
type_glDrawElements *glDrawElements;
type_glClear *glClear;
type_glGenTextures *glGenTextures;
type_glTexImage1D *glTexImage1D;
type_glTexImage2D *glTexImage2D;
type_glClearColor *glClearColor;
type_glFrontFace *glFrontFace;

#ifdef _WIN32
static HMODULE libgl;
static void *loadFunction(const char *funcName)
{
  void *result = wglGetProcAddress(funcName);
  if (!result)
  {
    result = GetProcAddress(libgl, funcName);
  }

  return result;
}
#endif

typedef unsigned long long int u64;

#define GET_PROC_LOAD(name) \
  name = (type_##name *) loadFunction(#name);

void zzxoto_glLoadFunctions()
{ 
#ifdef _WIN32
  libgl = LoadLibraryA("opengl32.dll");
#endif

  GET_PROC_LOAD(glTexImage2DMultisample)
  GET_PROC_LOAD(glBindFramebuffer)
  GET_PROC_LOAD(glGenFramebuffers)
  GET_PROC_LOAD(glFramebufferTexture2D)
  GET_PROC_LOAD(glCheckFramebufferStatus)
  GET_PROC_LOAD(glBlitFramebuffer)
  GET_PROC_LOAD(glAttachShader)
  GET_PROC_LOAD(glCompileShader)
  GET_PROC_LOAD(glCreateProgram)
  GET_PROC_LOAD(glCreateShader)
  GET_PROC_LOAD(glLinkProgram)
  GET_PROC_LOAD(glShaderSource)
  GET_PROC_LOAD(glUseProgram)
  GET_PROC_LOAD(glGetProgramInfoLog)
  GET_PROC_LOAD(glGetShaderInfoLog)
  GET_PROC_LOAD(glValidateProgram)
  GET_PROC_LOAD(glGetProgramiv)
  GET_PROC_LOAD(glGetUniformLocation)
  GET_PROC_LOAD(glUniform4fv)
  GET_PROC_LOAD(glUniformMatrix4fv)
  GET_PROC_LOAD(glUniform1i)
  GET_PROC_LOAD(glUniform1f)
  GET_PROC_LOAD(glUniform2fv)
  GET_PROC_LOAD(glUniform3fv)
  GET_PROC_LOAD(glEnableVertexAttribArray)
  GET_PROC_LOAD(glDisableVertexAttribArray)
  GET_PROC_LOAD(glGetAttribLocation)
  GET_PROC_LOAD(glVertexAttribPointer)
  GET_PROC_LOAD(glBindVertexArray)
  GET_PROC_LOAD(glGenVertexArrays)
  GET_PROC_LOAD(glBindBuffer)
  GET_PROC_LOAD(glGenBuffers)
  GET_PROC_LOAD(glBufferData)
  GET_PROC_LOAD(glBufferSubData)
  GET_PROC_LOAD(glActiveTexture)
  GET_PROC_LOAD(glDeleteProgram)
  GET_PROC_LOAD(glDeleteShader)
  GET_PROC_LOAD(glDeleteFramebuffers)
  GET_PROC_LOAD(glDrawBuffers)
  GET_PROC_LOAD(glGetStringi)
  GET_PROC_LOAD(glDetachShader)
  GET_PROC_LOAD(glDeleteBuffers)
  GET_PROC_LOAD(glDeleteVertexArrays)
  GET_PROC_LOAD(glBlendEquationSeparate)
  GET_PROC_LOAD(glBlendFuncSeparate)
  GET_PROC_LOAD(glGetShaderiv)
  GET_PROC_LOAD(glBlendEquation)
  GET_PROC_LOAD(glGetString)
  GET_PROC_LOAD(glGetIntegerv)
  GET_PROC_LOAD(glEnable)
  GET_PROC_LOAD(glDisable)
  GET_PROC_LOAD(glViewport)
  GET_PROC_LOAD(glIsEnabled)
  GET_PROC_LOAD(glTexParameteri)
  GET_PROC_LOAD(glBindTexture)
  GET_PROC_LOAD(glScissor)
  GET_PROC_LOAD(glDeleteTextures)
  GET_PROC_LOAD(glDrawElements)
  GET_PROC_LOAD(glClear)
  GET_PROC_LOAD(glGenTextures)
  GET_PROC_LOAD(glTexImage1D)
  GET_PROC_LOAD(glTexImage2D)
  GET_PROC_LOAD(glClearColor)
  GET_PROC_LOAD(glFrontFace)
}

zzxoto_GLInfo zzxoto_getGLInfo()
{
  zzxoto_GLInfo result = {};

  result.vendorString   = (char *) glGetString(GL_VENDOR);
  result.rendererString = (char *) glGetString(GL_RENDERER);
  result.versionString  = (char *) glGetString(GL_VERSION);
  result.shadingLanguageVersionString = (char *) glGetString(GL_SHADING_LANGUAGE_VERSION);

  //3.3 -> 330
  int majorVersion; int minorVersion;
  sscanf(result.versionString, "%d.%d", &majorVersion, &minorVersion);  
  result.version = majorVersion * 100 + minorVersion * 10;

  return result;
}

GLuint zzxoto_createProgram(GLuint vertexShader, GLuint fragmentShader)
{
  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  if (fragmentShader)
  {
    glAttachShader(program, fragmentShader);
  }
  glLinkProgram(program);

  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  assert(success);

  return program;
}

static void myPrint(char *str)
{
  #ifdef _WIN32
  OutputDebugStringA(str);
  #endif
}

GLuint zzxoto_createShader(const char *shaderCode, int shaderType)
{
  GLuint shader;
  
  shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderCode, NULL);
  glCompileShader(shader);

  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    myPrint(infoLog);
    assert(false);
  }

  return shader;
}