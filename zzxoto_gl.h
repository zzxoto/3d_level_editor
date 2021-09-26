#ifndef ZZXOTO_GL_H
#define ZZXOTO_GL_H

//----------------------------------------
// OpenGL    GLSL      GLSL
// version   version   string
//----------------------------------------
//  2.0       110       "#version 110"
//  2.1       120       "#version 120"
//  3.0       130       "#version 130"
//  3.1       140       "#version 140"
//  3.2       150       "#version 150"
//  3.3       330       "#version 330 core"
//  4.0       400       "#version 400 core"
//  4.1       410       "#version 410 core"
//  4.2       420       "#version 410 core"
//  4.3       430       "#version 430 core"
//  ES 2.0    100       "#version 100"      = WebGL 1.0
//  ES 3.0    300       "#version 300 es"   = WebGL 2.0
//----------------------------------------

//these definitions are burrowed from imgui_impl_opengl3_loader.h
typedef          float         khronos_float_t;
typedef signed   char          khronos_int8_t;
typedef unsigned char          khronos_uint8_t;
typedef signed   short int     khronos_int16_t;
typedef unsigned short int     khronos_uint16_t;
typedef signed   long long int khronos_intptr_t;
typedef signed   long long int khronos_ssize_t;

typedef void GLvoid;
typedef unsigned int GLenum;
typedef khronos_float_t GLfloat;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLbitfield;
typedef double GLdouble;
typedef unsigned int GLuint;
typedef unsigned char GLboolean;
typedef khronos_uint8_t GLubyte;
typedef char GLchar;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_intptr_t GLintptr;
typedef float GLclampf;

#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01

//window specific context creation flags
#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_NUMBER_PIXEL_FORMATS_ARB            0x2000
#define WGL_DRAW_TO_WINDOW_ARB                  0x2001
#define WGL_DRAW_TO_BITMAP_ARB                  0x2002
#define WGL_ACCELERATION_ARB                    0x2003
#define WGL_NEED_PALETTE_ARB                    0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB             0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB              0x2006
#define WGL_SWAP_METHOD_ARB                     0x2007
#define WGL_NUMBER_OVERLAYS_ARB                 0x2008
#define WGL_NUMBER_UNDERLAYS_ARB                0x2009
#define WGL_TRANSPARENT_ARB                     0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB           0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB         0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB          0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB         0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB         0x203B
#define WGL_SHARE_DEPTH_ARB                     0x200C
#define WGL_SHARE_STENCIL_ARB                   0x200D
#define WGL_SHARE_ACCUM_ARB                     0x200E
#define WGL_SUPPORT_GDI_ARB                     0x200F
#define WGL_SUPPORT_OPENGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_STEREO_ARB                          0x2012
#define WGL_PIXEL_TYPE_ARB                      0x2013
#define WGL_COLOR_BITS_ARB                      0x2014
#define WGL_RED_BITS_ARB                        0x2015
#define WGL_RED_SHIFT_ARB                       0x2016
#define WGL_GREEN_BITS_ARB                      0x2017
#define WGL_GREEN_SHIFT_ARB                     0x2018
#define WGL_BLUE_BITS_ARB                       0x2019
#define WGL_BLUE_SHIFT_ARB                      0x201A
#define WGL_ALPHA_BITS_ARB                      0x201B
#define WGL_ALPHA_SHIFT_ARB                     0x201C
#define WGL_ACCUM_BITS_ARB                      0x201D
#define WGL_ACCUM_RED_BITS_ARB                  0x201E
#define WGL_ACCUM_GREEN_BITS_ARB                0x201F
#define WGL_ACCUM_BLUE_BITS_ARB                 0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB                0x2021
#define WGL_DEPTH_BITS_ARB                      0x2022
#define WGL_STENCIL_BITS_ARB                    0x2023
#define WGL_AUX_BUFFERS_ARB                     0x2024
#define WGL_TYPE_RGBA_ARB                       0x202B
#define WGL_SAMPLE_BUFFERS_ARB		              0x2041
#define WGL_SAMPLES_ARB			                    0x2042


//blending
#define GL_BLEND                          0x0BE2
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305
#define GL_BLEND_SRC_RGB          0x80C9
#define GL_BLEND_DST_RGB          0x80C8  
#define GL_BLEND_SRC_ALPHA        0x80CB
#define GL_BLEND_DST_ALPHA        0x80CA
#define GL_BLEND_EQUATION_RGB     0x8009
#define GL_BLEND_EQUATION_ALPHA   0x883D

//tests
#define GL_CULL_FACE                      0x0B44
#define GL_CW                             0x0900
#define GL_CCW                            0x0901
#define GL_DEPTH_TEST                     0x0B71
#define GL_STENCIL_TEST                   0x0B90
#define GL_SCISSOR_TEST                   0x0C11
#define GL_VIEWPORT                       0x0BA2

//texture
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE_BINDING_1D             0x8068
#define GL_TEXTURE_BINDING_2D             0x8069
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_TEXTURE_1D                     0x0DE0
#define GL_TEXTURE_2D                     0x0DE1
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7

#define GL_SCISSOR_BOX                    0x0C10

//draw primitives
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006
#define GL_QUADS                          0x0007
#define GL_QUAD_STRIP                     0x0008
#define GL_POLYGON                        0x0009

//glClear masks
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_ENABLE_BIT                     0x00002000
#define GL_COLOR_BUFFER_BIT               0x00004000

//types
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_TRUE                           1
#define GL_FALSE                          0
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT                            0x1404
#define GL_UNSIGNED_INT                   0x1405
#define GL_FLOAT                          0x1406

#define GL_MAX_COLOR_ATTACHMENTS          0x8CDF
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D

#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_TYPE_MARKER              0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269
#define GL_DEBUG_TYPE_POP_GROUP           0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242

#define GL_ARRAY_BUFFER                   0x8892

//draw hint
#define GL_STREAM_DRAW                    0x88E0
#define GL_STREAM_READ                    0x88E1
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA

#define GL_FRAMEBUFFER_SRGB               0x8DB9
#define GL_SRGB8_ALPHA8                   0x8C43

#define GL_SHADING_LANGUAGE_VERSION       0x8B8C
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS                0x8B83

#define GL_FRAMEBUFFER                    0x8D40
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_COLOR_ATTACHMENT4              0x8CE4
#define GL_COLOR_ATTACHMENT5              0x8CE5
#define GL_COLOR_ATTACHMENT6              0x8CE6
#define GL_COLOR_ATTACHMENT7              0x8CE7
#define GL_COLOR_ATTACHMENT8              0x8CE8
#define GL_COLOR_ATTACHMENT9              0x8CE9
#define GL_COLOR_ATTACHMENT10             0x8CEA
#define GL_COLOR_ATTACHMENT11             0x8CEB
#define GL_COLOR_ATTACHMENT12             0x8CEC
#define GL_COLOR_ATTACHMENT13             0x8CED
#define GL_COLOR_ATTACHMENT14             0x8CEE
#define GL_COLOR_ATTACHMENT15             0x8CEF
#define GL_COLOR_ATTACHMENT16             0x8CF0
#define GL_COLOR_ATTACHMENT17             0x8CF1
#define GL_COLOR_ATTACHMENT18             0x8CF2
#define GL_COLOR_ATTACHMENT19             0x8CF3
#define GL_COLOR_ATTACHMENT20             0x8CF4
#define GL_COLOR_ATTACHMENT21             0x8CF5
#define GL_COLOR_ATTACHMENT22             0x8CF6
#define GL_COLOR_ATTACHMENT23             0x8CF7
#define GL_COLOR_ATTACHMENT24             0x8CF8
#define GL_COLOR_ATTACHMENT25             0x8CF9
#define GL_COLOR_ATTACHMENT26             0x8CFA
#define GL_COLOR_ATTACHMENT27             0x8CFB
#define GL_COLOR_ATTACHMENT28             0x8CFC
#define GL_COLOR_ATTACHMENT29             0x8CFD
#define GL_COLOR_ATTACHMENT30             0x8CFE
#define GL_COLOR_ATTACHMENT31             0x8CFF
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5

#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_DEPTH_COMPONENT32F             0x8CAC

//pixel format
#define GL_COLOR_INDEX                    0x1900
#define GL_STENCIL_INDEX                  0x1901
#define GL_DEPTH_COMPONENT                0x1902
#define GL_RED                            0x1903
#define GL_GREEN                          0x1904
#define GL_BLUE                           0x1905
#define GL_ALPHA                          0x1906
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_LUMINANCE                      0x1909
#define GL_LUMINANCE_ALPHA                0x190A

#define GL_RGBA32F                        0x8814
#define GL_RGB32F                         0x8815
#define GL_RGBA16F                        0x881A
#define GL_RGB16F                         0x881B

#define GL_MULTISAMPLE                    0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE       0x809E
#define GL_SAMPLE_ALPHA_TO_ONE            0x809F
#define GL_SAMPLE_COVERAGE                0x80A0
#define GL_SAMPLE_BUFFERS                 0x80A8
#define GL_SAMPLES                        0x80A9
#define GL_SAMPLE_COVERAGE_VALUE          0x80AA
#define GL_SAMPLE_COVERAGE_INVERT         0x80AB
#define GL_TEXTURE_2D_MULTISAMPLE         0x9100
#define GL_MAX_SAMPLES                    0x8D57
#define GL_MAX_COLOR_TEXTURE_SAMPLES      0x910E
#define GL_MAX_DEPTH_TEXTURE_SAMPLES      0x910F

#define GL_MAJOR_VERSION          0x821B 
#define GL_MINOR_VERSION          0x821C
#define GL_NUM_EXTENSIONS         0x821D
#define GL_FUNC_ADD               0x8006
#define GL_ACTIVE_TEXTURE         0x84E0
#define GL_CURRENT_PROGRAM        0x8B8D
#define GL_ELEMENT_ARRAY_BUFFER   0x8893
#define GL_INFO_LOG_LENGTH        0x8B84 
#define GL_ARRAY_BUFFER_BINDING   0x8894
#define GL_VERTEX_ARRAY_BINDING   0x8896

typedef void type_glTexImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void type_glBindFramebuffer(GLenum target, GLuint framebuffer);
typedef void type_glGenFramebuffers(GLsizei n, GLuint *framebuffers);
typedef void type_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef GLenum type_glCheckFramebufferStatus(GLenum target);
typedef void type_glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void type_glAttachShader(GLuint program, GLuint shader);
typedef void type_glCompileShader(GLuint shader);
typedef GLuint type_glCreateProgram(void);
typedef GLuint type_glCreateShader(GLenum type);
typedef void type_glLinkProgram(GLuint program);
typedef void type_glShaderSource(GLuint shader, GLsizei count, const GLchar * const*string, GLint *length);
typedef void type_glUseProgram(GLuint program);
typedef void type_glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void type_glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void type_glValidateProgram(GLuint program);
typedef void type_glGetProgramiv(GLuint program, GLenum pname, GLint *params);
typedef GLint type_glGetUniformLocation (GLuint program, const GLchar *name);
typedef void type_glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
typedef void type_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void type_glUniform1i(GLint location, GLint v0);
typedef void type_glUniform1f(GLint location, GLfloat v0);
typedef void type_glUniform2fv(GLint location, GLsizei count, const GLfloat *value);
typedef void type_glUniform3fv(GLint location, GLsizei count, const GLfloat *value);
typedef void type_glEnableVertexAttribArray(GLuint index);
typedef void type_glDisableVertexAttribArray(GLuint index);
typedef GLint type_glGetAttribLocation(GLuint program, const GLchar *name);
typedef void type_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void type_glBindVertexArray(GLuint array);
typedef void type_glGenVertexArrays(GLsizei n, GLuint *arrays);
typedef void type_glBindBuffer (GLenum target, GLuint buffer);
typedef void type_glGenBuffers (GLsizei n, GLuint *buffers);
typedef void type_glBufferData (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void type_glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
typedef void type_glActiveTexture (GLenum texture);
typedef void type_glDeleteProgram (GLuint program);
typedef void type_glDeleteShader (GLuint shader);
typedef void type_glDeleteFramebuffers (GLsizei n, const GLuint *framebuffers);
typedef void type_glDrawBuffers (GLsizei n, const GLenum *bufs);
typedef const GLubyte *type_glGetStringi(GLenum name, GLuint index);
typedef void type_glDeleteBuffers(GLsizei n, const GLuint * buffers);
typedef void type_glDetachShader(GLuint program,GLuint shader);
typedef void type_glDeleteVertexArrays(GLsizei n, const GLuint *arrays);
typedef void type_glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
typedef void type_glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
typedef void type_glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
typedef void type_glBlendEquation(GLenum mode);
typedef void type_glGetIntegerv (GLenum pname, GLint *params);
typedef const GLubyte *type_glGetString (GLenum name);
typedef void type_glEnable(GLenum cap);
typedef void type_glDisable (GLenum cap);
typedef void type_glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
typedef GLboolean type_glIsEnabled (GLenum cap);
typedef void type_glTexParameteri (GLenum target, GLenum pname, GLint param);
typedef void type_glBindTexture (GLenum target, GLuint texture);
typedef void type_glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
typedef void type_glDeleteTextures (GLsizei n, const GLuint *textures);
typedef void type_glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
typedef void type_glClear(GLbitfield mask);
typedef void type_glGenTextures (GLsizei n, GLuint *textures);
typedef void type_glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void type_glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void type_glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
typedef void type_glFrontFace(GLenum mode);

extern type_glTexImage2DMultisample *glTexImage2DMultisample;
extern type_glBindFramebuffer *glBindFramebuffer;
extern type_glGenFramebuffers *glGenFramebuffers;
extern type_glFramebufferTexture2D *glFramebufferTexture2D;
extern type_glCheckFramebufferStatus *glCheckFramebufferStatus;
extern type_glBlitFramebuffer *glBlitFramebuffer;
extern type_glAttachShader *glAttachShader;
extern type_glCompileShader *glCompileShader;
extern type_glCreateProgram *glCreateProgram;
extern type_glCreateShader *glCreateShader;
extern type_glLinkProgram *glLinkProgram;
extern type_glShaderSource *glShaderSource;
extern type_glUseProgram *glUseProgram;
extern type_glGetProgramInfoLog *glGetProgramInfoLog;
extern type_glGetShaderInfoLog *glGetShaderInfoLog;
extern type_glValidateProgram *glValidateProgram;
extern type_glGetProgramiv *glGetProgramiv;
extern type_glGetUniformLocation *glGetUniformLocation;
extern type_glUniform4fv *glUniform4fv;
extern type_glUniformMatrix4fv *glUniformMatrix4fv;
extern type_glUniform1i *glUniform1i;
extern type_glUniform1f *glUniform1f;
extern type_glUniform2fv *glUniform2fv;
extern type_glUniform3fv *glUniform3fv;
extern type_glEnableVertexAttribArray *glEnableVertexAttribArray;
extern type_glDisableVertexAttribArray *glDisableVertexAttribArray;
extern type_glGetAttribLocation *glGetAttribLocation;
extern type_glVertexAttribPointer *glVertexAttribPointer;
extern type_glBindVertexArray *glBindVertexArray;
extern type_glGenVertexArrays *glGenVertexArrays;
extern type_glBindBuffer *glBindBuffer;
extern type_glGenBuffers *glGenBuffers;
extern type_glBufferData *glBufferData;
extern type_glBufferSubData *glBufferSubData;
extern type_glActiveTexture *glActiveTexture;
extern type_glDeleteProgram *glDeleteProgram;
extern type_glDeleteShader *glDeleteShader;
extern type_glDeleteFramebuffers *glDeleteFramebuffers;
extern type_glDrawBuffers *glDrawBuffers;
extern type_glGetStringi *glGetStringi;
extern type_glDetachShader *glDetachShader;
extern type_glDeleteBuffers *glDeleteBuffers;
extern type_glDeleteVertexArrays *glDeleteVertexArrays;
extern type_glBlendEquationSeparate *glBlendEquationSeparate;
extern type_glBlendFuncSeparate *glBlendFuncSeparate;
extern type_glGetShaderiv *glGetShaderiv;
extern type_glBlendEquation *glBlendEquation;
extern type_glGetString *glGetString;
extern type_glGetIntegerv *glGetIntegerv;
extern type_glEnable *glEnable;
extern type_glDisable *glDisable;
extern type_glViewport *glViewport;
extern type_glIsEnabled *glIsEnabled;
extern type_glTexParameteri *glTexParameteri;
extern type_glBindTexture *glBindTexture;
extern type_glScissor *glScissor;
extern type_glDeleteTextures *glDeleteTextures;
extern type_glDrawElements *glDrawElements;
extern type_glClear *glClear;
extern type_glGenTextures *glGenTextures;
extern type_glTexImage1D *glTexImage1D;
extern type_glTexImage2D *glTexImage2D;
extern type_glClearColor *glClearColor;
extern type_glFrontFace *glFrontFace;

struct zzxoto_GLInfo
{
  int version;

  char *vendorString;
  char *rendererString;
  char *versionString;
  char *shadingLanguageVersionString;
};
extern zzxoto_GLInfo zzxoto_getGLInfo();

extern void zzxoto_glLoadFunctions();
extern GLuint zzxoto_createShader(const char *shaderCode, int shaderType);

//fragment shader is optional
//send 0 if not present
extern GLuint zzxoto_createProgram(GLuint vertexShader, GLuint fragmentShader);

#endif