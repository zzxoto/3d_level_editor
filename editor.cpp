#include "zzxoto_platform.h"
#include "dearimgui/imgui.h"
#include "zzxoto_gl.h"
#include "zzxoto_vector.h"
#include "zzxoto_camera.h"
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include "zzxoto_constants.h"

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

static void debugMatrix(const Mat4 &mat)
{
  char buffer[1 << 7];
  debugMatrix(buffer, mat);
  platform_debugString(buffer);
}

static void debugVector(const Vec3 &v)
{
  char buffer[1 << 7];
  sprintf(buffer, "<%.2f, %.2f, %.2f>\n", v.x, v.y, v.z);
  platform_debugString(buffer);
}

void debugVector(const Vec4 &v)
{
  char buffer[1 << 7];
  sprintf(buffer, "<%.2f, %.2f, %.2f, %.2f>\n", v.x, v.y, v.z, v.w);
  platform_debugString(buffer);
}

struct ProgramData
{
  GLuint program;
  GLuint modelToWorldMatrixUnif;
  GLuint worldToCameraMatrixUnif;
  GLuint cameraToClipMatrixUnif;  
  GLuint surfaceDiffuseColorUnif;
  GLuint ambientLightIntensityUnif;
  GLuint dirToLightUnif;
  GLuint dirLightIntensityUnif;
  GLuint renderType;
};

//lighting in world space
static const char *vertexShaderCode = R"FOO(
  #version 330 core
  layout(location = 0) in vec3 pos;
  layout(location = 1) in vec3 normal;
  uniform mat4 modelToWorldMatrix;
  uniform mat4 worldToCameraMatrix;
  uniform mat4 cameraToClipMatrix;
  uniform vec3 surfaceDiffuseColor;
  uniform vec3 dirToLight;
  uniform vec3 dirLightIntensity;
  uniform vec3 ambientLightIntensity;
  uniform int renderType;

  smooth out vec3 interpColor;

  void main()
  {
    if (renderType == 1)
    {
      float cosAngIncidence = clamp(dot(normal, dirToLight), 0, 1);
      interpColor = ambientLightIntensity * surfaceDiffuseColor + dirLightIntensity * surfaceDiffuseColor * cosAngIncidence;
    }
    else if (renderType == 2)
    {
      interpColor = surfaceDiffuseColor;
    }

    gl_Position = cameraToClipMatrix * worldToCameraMatrix * modelToWorldMatrix * vec4(pos.x, pos.y, pos.z, 1.f);
  }
)FOO";

static const char *fragmentShaderCode =  R"FOO(
  #version 330 core
  in vec3 interpColor;

  out vec4 outColor;
  void main()
  {
    outColor = vec4(interpColor, 1.0f);
  }
)FOO";

struct Mesh
{
  int indicesCount;
  short *indices;
  Vec3 *positions;
  int positionsCount;
  Vec3 *normals;
  int normalsCount;

  Vec3 color;

  Mat4 modelToWorldTransform;

  Mesh *cloneMesh(void *memory)
  {
    Mesh *clone_;
    if (memory)
    {
      clone_ = (Mesh *) memory;
    }
    else
    {
      clone_ = (Mesh *) malloc(sizeof(Mesh));
    }

    clone_->positionsCount = this->positionsCount;
    clone_->positions = (Vec3 *) malloc(sizeof(Vec3) * this->positionsCount);
    clone_->normalsCount = this->normalsCount;
    clone_->normals = (Vec3 *) malloc(sizeof(Vec3) * this->normalsCount);
    clone_->indicesCount = this->indicesCount;
    clone_->indices = (short *) malloc(sizeof(short) * this->indicesCount);
    clone_->color = this->color;
    clone_->modelToWorldTransform = this->modelToWorldTransform;

    for (int i = 0; i < this->positionsCount; i++) clone_->positions[i] = this->positions[i];
    for (int i = 0; i < this->normalsCount; i++) clone_->normals[i] = this->normals[i];
    for (int i = 0; i < this->indicesCount; i++) clone_->indices[i] = this->indices[i];

    return clone_;
  }
};

struct Ray
{
  Vec3 startPos;
  Vec3 direction;
  Vec3 color;

  bool checkRayPlaneIntersection(const Vec3 &p0, const Vec3 &normalPlane, Vec4 *intersectionInfo)
  {
    const Vec3 &pointOnPlane = p0;
    const Vec3 &d     = this->direction;
    const Vec3 &s     = this->startPos;
    
    float n_dot_d = dot(normalPlane, d);
    float n_dot_p_minus_s = dot(normalPlane, (pointOnPlane - s));
    //1. if n_dot_d is < 0, then ray intersects 
    //the plane and the plane is facing the direction of the ray
    //
    //2. if n_dot_d is 0 then ray runs parallel to the plane
    //i.e. no intersection with the plane
    //
    //3. if n_dot_d is > 0, then ray intersects the plane and the plane
    //is facing away from the direction of the ray
    //
    //we are interested in case 1 and 3.

    bool result = false;
    if (fabsf(n_dot_d) > .001f)
    {
      float distanceFromRay = n_dot_p_minus_s / n_dot_d;

      //object is in front of the ray
      if (distanceFromRay > 0)
      {
        //point of intersection on plane
        Vec3 x = s + d * distanceFromRay;

        result = true;

        if (intersectionInfo)
        {
          intersectionInfo->x = x.x;
          intersectionInfo->y = x.y;
          intersectionInfo->z = x.z;
          intersectionInfo->w = distanceFromRay;
        }

        result = true;
      }
    }

    return result;
  }

  bool checkRayTriangleIntersection(const Vec3 &p0, const Vec3 &p1, const Vec3 &p2, const Vec3 &normalPlane, Vec4 *intersectionInfo)
  {
    //point of intersection on plane
    Vec4 x_;

    bool result = false;
    if (checkRayPlaneIntersection(p0, normalPlane, &x_))
    {
      Vec3 x = { x_.x, x_.y, x_.z };

      bool isInsideTriangle = dot(cross((p1 - p0), (x - p0)), normalPlane) >= 0;
      isInsideTriangle = isInsideTriangle && dot(cross((p2 - p1), (x - p1)), normalPlane) >= 0;
      isInsideTriangle = isInsideTriangle && dot(cross((p0 - p2), (x - p2)), normalPlane) >= 0;
      result = isInsideTriangle;
      if (isInsideTriangle && intersectionInfo)
      {
        *intersectionInfo = x_;
      }
    }

    return result;
  }
};

struct Box;
static Box *g_boxPrototype;

struct Box: Mesh
{
  bool isKilled;

  //note that the vertices are scaled directly, as opposed
  //to encoding scale information in the transform matrix;
  //this scale vector is for imgui editor
  Vec3 scale_;

  void kill()
  {
    if (!this->isKilled)
    {
      free(this->positions);
      free(this->normals);
      free(this->indices);
      this->isKilled = true;
    }
  }

  void scaleXYZ(const Vec3 &s)
  {
    this->scale_ = s;

    Vec3 normUp = { .0f, 1.0f, .0f };

    //The initial prototype cube was 1 X 1 X 1, centered
    //about the origin, thus these magic .5f, .5f, .5f numbers.
    //See createBoxMeshPrototype.
    Vec3 t = {0.f, .5f, 0.f};
    Mat4 transform = translate({-t.x, -t.y, -t.z }) * (scale(s) * translate({ t.x, t.y, t.z }));

    for (int i = 0; i < this->indicesCount; i++)
    {
      int idx = this->indices[i];
      this->positions[idx] = transform * g_boxPrototype->positions[idx];
    }
  }

  Box *cloneBox(void *memory)
  {
    Box *clone_;
    if (memory)
    {
      clone_ = (Box *) memory;
    }
    else
    {
      clone_ = (Box *) malloc(sizeof(Box));
    }
    if (!this->isKilled)
    {
      this->cloneMesh(clone_);
      clone_->scale_ = this->scale_;
    }

    clone_->isKilled = this->isKilled;

    return clone_;
  }

  bool checkRayIntersection(Ray ray, Vec4 *rayHitInfo)
  {
    bool foundOne = false;
    Vec4 rayHitInfo_;

    for (int i = 0; i < this->indicesCount; i+= 3)
    {
      int i0 = this->indices[i];
      int i1 = this->indices[i + 1];
      int i2 = this->indices[i + 2];

      Vec3 p0 = this->modelToWorldTransform * this->positions[i0];
      Vec3 p1 = this->modelToWorldTransform * this->positions[i1];
      Vec3 p2 = this->modelToWorldTransform * this->positions[i2];

      Vec3 normal = this->normals[i0];

      Vec4 rayHitInfo__;
      if (ray.checkRayTriangleIntersection(p0, p1, p2, normal, &rayHitInfo__))
      {
        if (!foundOne || rayHitInfo__.w < rayHitInfo_.w)
        {
          rayHitInfo_ = rayHitInfo__;
          foundOne = true;
        }
      }
    }

    if (foundOne && rayHitInfo)
    {
      *rayHitInfo = rayHitInfo_;
    }
    return foundOne;
  }
};

Box *createBoxMeshPrototype()
{
  #define CUBE_A -.5f, .5f, .5f
  #define CUBE_B .5f, .5f, .5f
  #define CUBE_C -.5f, -.5f, .5f
  #define CUBE_D .5f, -.5f, .5f
  #define CUBE_E -.5f, .5f, -.5f
  #define CUBE_F .5f, .5f, -.5f
  #define CUBE_G -.5f, -.5f, -.5f
  #define CUBE_H .5f, -.5f, -.5f

  #define NORM_LEFT  -1.0f, .0f, .0f
  #define NORM_RIGHT 1.0f,  .0f, .0f
  #define NORM_UP    .0f,   1.0f, .0f
  #define NORM_DOWN  .0f,  -1.0f, .0f
  #define NORM_FRONT .0f,  .0f,  1.0f
  #define NORM_BACK  .0f,  .0f,  -1.0f

  Vec3 cubePositions[] = 
  {
    CUBE_A, CUBE_C, CUBE_D, CUBE_B,
    CUBE_B, CUBE_D, CUBE_H, CUBE_F,
    CUBE_E, CUBE_A, CUBE_B, CUBE_F,
    
    CUBE_A, CUBE_E, CUBE_G, CUBE_C,
    CUBE_E, CUBE_F, CUBE_H, CUBE_G,
    CUBE_G, CUBE_H, CUBE_D, CUBE_C
  };

  Vec3 cubeNormals[] =
  {
    NORM_FRONT, NORM_FRONT, NORM_FRONT, NORM_FRONT,
    NORM_RIGHT, NORM_RIGHT, NORM_RIGHT, NORM_RIGHT,
    NORM_UP, NORM_UP, NORM_UP, NORM_UP,
    
    NORM_LEFT, NORM_LEFT, NORM_LEFT, NORM_LEFT,
    NORM_BACK, NORM_BACK, NORM_BACK, NORM_BACK,
    NORM_DOWN, NORM_DOWN, NORM_DOWN, NORM_DOWN
  };

  short cubeIndices[] =
  {
    1, 3, 0, 
    1, 2, 3,
    
    5, 7, 4, 
    5, 6, 7,
    
    9, 11, 8,
    9, 10, 11,
    
    15, 12, 13,
    15, 13, 14,
    
    19, 16, 17,
    19, 17, 18,
    
    23, 20, 21,
    23, 21, 22
  };

  Box *proto = (Box *) malloc(sizeof(Box));
  proto->positionsCount = ARRAY_LENGTH(cubePositions);
  proto->positions = (Vec3 *) malloc(sizeof(cubePositions));
  proto->normalsCount = ARRAY_LENGTH(cubeNormals);
  proto->normals = (Vec3 *) malloc(sizeof(cubeNormals));
  proto->indicesCount = ARRAY_LENGTH(cubeIndices);
  proto->indices = (short *) malloc(sizeof(cubeIndices));    
  proto->color = { .6, .6, .6 };
  proto->modelToWorldTransform = identity();
  proto->isKilled = false;

  for (int i = 0; i < ARRAY_LENGTH(cubePositions); i++) proto->positions[i] = cubePositions[i];
  for (int i = 0; i < ARRAY_LENGTH(cubeIndices); i++) proto->indices[i] = cubeIndices[i];
  for (int i = 0; i < ARRAY_LENGTH(cubeNormals); i++) proto->normals[i] = cubeNormals[i];
  proto->scale_ = {1.f, 1.f, 1.f};

  return proto;

  #undef CUBE_A
  #undef CUBE_B
  #undef CUBE_C
  #undef CUBE_D
  #undef CUBE_E
  #undef CUBE_F
  #undef CUBE_G
  #undef CUBE_H

  #undef NORM_LEFT
  #undef NORM_RIGHT
  #undef NORM_UP
  #undef NORM_DOWN
  #undef NORM_FRONT
  #undef NORM_BACK
}

static GLuint program;

static ProgramData triangleShaderProgram, lineShaderProgram;

static GLuint vao, vbo, ibo;
  
static Vec3 directionalLightIntensity;
static Vec3 ambientLightIntensity;
static Vec3 dirToLight;

struct DragInfo
{
  Vec3 pos0;
  Vec3 pos1;
  Vec3 deltaFrame;
};

struct UndoStack;

struct GameState
{
  Mesh *floor;
  Camera *camera;
  Ray ray;

  Box *boxes;
  int boxCount;

  Box *selectedBox;
  bool isSelectionDragActive;

  Box *copyingBox;
  bool isCopying;

  Vec3 bgColor;

  UndoStack *undoStack;

  struct {
    bool isSavePopupOpen;
    bool isLoadPopupOpen;
    char saveFilename[1 << 5];    
    char **loadFilenameOpts;
    int loadSelectionIdx;
    int loadFilenameCount;
  } uiState;
};

static GameState *gameState;

//TODO: undo stack size limit not handled
struct UndoStack
{
  void recordChange(float timestamp, GameState *gameState);
  void undoChange(float timestamp, GameState *gameState);
  bool hasChanged(GameState *gameState);
  void init()
  {
    this->stackHeadAt = -1;
    this->stackSize = 1 << 10;
    this->lastRecordedTimestamp = 0.f;
    this->lastUndidTimestamp = 0.f;
  }
  struct {
    Box *boxes;
    int boxCount;
  } boxHistory[1 << 10];
  int stackHeadAt;
  int stackSize;
  float lastRecordedTimestamp;
  float lastUndidTimestamp;
};

bool UndoStack::hasChanged(GameState *gameState)
{
  Box *currBoxes = this->boxHistory[this->stackHeadAt].boxes;
  int currBoxCount = this->boxHistory[this->stackHeadAt].boxCount;

  if (gameState->boxCount != currBoxCount)  return true;

  bool changed = false;
  for (int i = 0; i < currBoxCount; i++)
  {
    Box *box1 = currBoxes + i;
    Box *box2 = gameState->boxes + i;

    changed = box1->isKilled != box2->isKilled;
    changed = changed || box1->color != box2->color;
    changed = changed || box1->scale_ != box2->scale_;
    changed = changed || box1->modelToWorldTransform != box2->modelToWorldTransform;

    if (changed)
    {
      break;
    }
  }

  return changed;
}

void UndoStack::recordChange(float timestamp, GameState *gameState)
{
  if (this->stackHeadAt == -1 || this->hasChanged(gameState) && (timestamp - this->lastRecordedTimestamp > .5f))
  {
    this->stackHeadAt += 1;

    int boxCount = gameState->boxCount;
    Box *boxes = (Box *) malloc(sizeof(Box) * boxCount);
    for (int i = 0; i < boxCount; i++)
    {
      gameState->boxes[i].cloneBox(&boxes[i]);
    }
    this->boxHistory[this->stackHeadAt].boxes = boxes;
    this->boxHistory[this->stackHeadAt].boxCount = boxCount;
    this->lastRecordedTimestamp = timestamp;
  }
}

void UndoStack::undoChange(float timestamp, GameState *gameState)
{
  if (this->stackHeadAt >= 0 && (timestamp - this->lastUndidTimestamp) > .5f)
  {
    for (int i = 0; i < gameState->boxCount; i++)
    {
      Box *box = gameState->boxes + i;
      box->kill();
    }
    free(gameState->boxes);

    gameState->boxCount = this->boxHistory[this->stackHeadAt].boxCount;
    gameState->boxes = this->boxHistory[this->stackHeadAt].boxes;
    gameState->selectedBox = NULL;
    gameState->isSelectionDragActive = false;
    
    this->stackHeadAt--;
    this->lastUndidTimestamp = timestamp;
  }
}

static ProgramData initializeProgram()
{
  ProgramData p;
  GLuint vertexShader = zzxoto_createShader(vertexShaderCode, GL_VERTEX_SHADER);
  GLuint fragmentShader = zzxoto_createShader(fragmentShaderCode, GL_FRAGMENT_SHADER);
  p.program = zzxoto_createProgram(vertexShader, fragmentShader);
  p.modelToWorldMatrixUnif = glGetUniformLocation(p.program, "modelToWorldMatrix");
  p.worldToCameraMatrixUnif = glGetUniformLocation(p.program, "worldToCameraMatrix");
  p.cameraToClipMatrixUnif = glGetUniformLocation(p.program, "cameraToClipMatrix");
  p.surfaceDiffuseColorUnif = glGetUniformLocation(p.program, "surfaceDiffuseColor");
  p.dirToLightUnif = glGetUniformLocation(p.program, "dirToLight");
  p.dirLightIntensityUnif = glGetUniformLocation(p.program, "dirLightIntensity");
  p.ambientLightIntensityUnif = glGetUniformLocation(p.program, "ambientLightIntensity");
  p.renderType = glGetUniformLocation(p.program, "renderType");

  return p;
}

void saveLevel(const char *filename_, const GameState *gameState)
{
  #define myprintf(buffer, str, ...) offset += sprintf((buffer + offset), (str), __VA_ARGS__);

  char filename[1 << 8];
  sprintf(filename, ".\\levels\\%s", filename_);

  int offset = 0;

  size_t bufferSize = 1 << 20;
  char *buffer = (char *) malloc(bufferSize);
  memset(buffer, 0, bufferSize);

  myprintf(buffer, "#floor_color\n");
  const Vec3 &floorColor = gameState->floor->color;
  myprintf(buffer, "%.5f,%.5f,%.5f\n", floorColor.x, floorColor.y, floorColor.z);
  myprintf(buffer, "\n");
  myprintf(buffer, "#background_color\n");
  const Vec3 &bgColor = gameState->bgColor;
  myprintf(buffer, "%.5f,%.5f,%.5f\n", bgColor.x, bgColor.y, bgColor.z);
  myprintf(buffer, "\n");
  myprintf(buffer, "#boxes\n");

  int aliveBoxes = 0;
  for (int i = 0; i < gameState->boxCount; i++)
  {
    if (!gameState->boxes[i].isKilled)
    {
      aliveBoxes++;
    }
  }
  myprintf(buffer, "boxCount: %d\n", aliveBoxes);
  myprintf(buffer, "\n");

  for (int i = 0; i < gameState->boxCount; i++)
  {
    Box *b = &gameState->boxes[i];
    if (!b->isKilled)
    {
      myprintf(buffer, "scale: %.5f,%.5f,%.5f\n", b->scale_.x, b->scale_.y, b->scale_.z);
      myprintf(buffer, "color: %.5f,%.5f,%.5f\n", b->color.x, b->color.y, b->color.z);
      const Mat4 &m = b->modelToWorldTransform;
      myprintf(buffer, "modelToWorldTransform: %.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f\n", 
        m.a00, m.a10, m.a20, m.a30,
        m.a01, m.a11, m.a21, m.a31,
        m.a02, m.a12, m.a22, m.a32,
        m.a03, m.a13, m.a23, m.a33);
      myprintf(buffer, "\n");
    }    
  }

  platform_writeToFile(filename, buffer);

  #undef myprintf
}

static int readLine(char *dest, const char *src)
{
  int count = 0;
  while(*src != 0 && *src != '\n')
  {
    *dest = *src;
    src++; dest++; count++;
  }

  return count;
}

static void eatNewline(char **src)
{
  char *s = *src;
  while (*s == '\n')
  {
    s++;
  }
  *src = s;
}

GameState *createGameState()
{
  GameState *gameState = (GameState *) malloc(sizeof(GameState));

  {
    Mesh *myFloor = (Mesh *) malloc(sizeof(Mesh));

    myFloor->color = COLOR_FLOOR_INITIAL;
    myFloor->positionsCount = 4;
    myFloor->normalsCount = 4;
    myFloor->indicesCount = 6;
    myFloor->positions = (Vec3 *) malloc(sizeof(Vec3) * myFloor->positionsCount);
    myFloor->normals = (Vec3 *) malloc(sizeof(Vec3) * myFloor->normalsCount);
    myFloor->indices = (short *) malloc(sizeof(short) * myFloor->indicesCount);
    myFloor->modelToWorldTransform = identity();
    
    //x0,z0      x1,z0
    //
    //
    //x0,z1      x1,z1
    //
    float x0 = FLOOR_CENTER.x + -FLOOR_LENGTH / 2.f;
    float x1 = FLOOR_CENTER.x + FLOOR_LENGTH / 2.f;
    float z0 = FLOOR_CENTER.z + -FLOOR_WIDTH / 2.f;
    float z1 = FLOOR_CENTER.z + FLOOR_WIDTH / 2.f;
    float y = FLOOR_CENTER.y;
    myFloor->positions[0] = {x0, y, z0};
    myFloor->positions[1] = {x1, y, z0};
    myFloor->positions[2] = {x0, y, z1};
    myFloor->positions[3] = {x1, y, z1};

    myFloor->normals[0] = FLOOR_NORMAL;
    myFloor->normals[1] = FLOOR_NORMAL;
    myFloor->normals[2] = FLOOR_NORMAL;
    myFloor->normals[3] = FLOOR_NORMAL;

    myFloor->indices[0] = 0;
    myFloor->indices[1] = 2;
    myFloor->indices[2] = 3;
    myFloor->indices[3] = 0;
    myFloor->indices[4] = 3;
    myFloor->indices[5] = 1;

    gameState->floor = myFloor;
  }

  gameState->camera = (Camera *) malloc(sizeof(Camera));
  *gameState->camera = Camera(CAMERA_INITIAL_POSITION, CAMERA_INITIAL_LOOKAT, NEAR, FAR);

  gameState->boxCount = 100;
  gameState->boxes = (Box *) malloc(sizeof(Box) * gameState->boxCount);
  for (int i = 1; i < gameState->boxCount; i++) gameState->boxes[i].isKilled = true;

  gameState->selectedBox = NULL;
  gameState->isSelectionDragActive = false;

  gameState->copyingBox = (Box *) malloc(sizeof(Box));
  gameState->isCopying = false;

  gameState->bgColor = COLOR_BACKGROUND_INITIAL;

  gameState->undoStack = (UndoStack *) malloc(sizeof(UndoStack));
  gameState->undoStack->init();

  gameState->uiState.isSavePopupOpen = false;
  gameState->uiState.isLoadPopupOpen = false;
  gameState->uiState.loadFilenameCount = 0;
  gameState->uiState.loadFilenameOpts = NULL;
  gameState->uiState.loadSelectionIdx = -1;
  memset(gameState->uiState.saveFilename, 0, sizeof(gameState->uiState.saveFilename));

  return gameState;
}

//
//#floor_color
//1, 2, 3
//
//#background_color
//1, 3, 4
//
//#boxes
//boxCount: 3
//
//scale_: xx
//color: xx
//modelToWorldTransform: xx
static GameState *loadLevel(const char *filename_)
{
  char *readBuffer = (char *) malloc(1 << 20);
  char lineBuffer[1 << 10];

  char filename[1 << 8];
  sprintf(filename, ".\\levels\\%s", filename_);

  int readFileCount = platform_readFile(filename, readBuffer);
  assert(readFileCount > 0); 

  char *readHead = readBuffer;

  int readLineCount = 0;
  #define readLine_()\
    readLineCount = readLine(lineBuffer, readHead);\
    lineBuffer[readLineCount] = 0;\
    readHead += readLineCount;\
    eatNewline(&readHead)

  readLine_();
  readLine_();
  Vec3 floorColor;
  sscanf(lineBuffer, "%f,%f,%f", &floorColor.x, &floorColor.y, &floorColor.z);
  readLine_();
  readLine_();
  Vec3 bgColor;
  sscanf(lineBuffer, "%f,%f,%f", &bgColor.x, &bgColor.y, &bgColor.z);
  readLine_();
  readLine_();
  int boxCount = 0;
  sscanf(lineBuffer, "boxCount: %d", &boxCount);

  GameState *gameState = createGameState();
  gameState->floor->color = floorColor;
  gameState->bgColor = bgColor;

  Vec3 boxScale;
  Vec3 boxColor;
  Mat4 m;
  for (int i = 0; i < boxCount; i++)
  {
    readLine_();
    sscanf(lineBuffer, "scale: %f,%f,%f", &boxScale.x, &boxScale.y, &boxScale.z);
    readLine_();
    sscanf(lineBuffer, "color: %f,%f,%f", &boxColor.x, &boxColor.y, &boxColor.z);
    readLine_();
    sscanf(lineBuffer, "modelToWorldTransform: %f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
      &m.a00, &m.a10, &m.a20, &m.a30,
      &m.a01, &m.a11, &m.a21, &m.a31,
      &m.a02, &m.a12, &m.a22, &m.a32,
      &m.a03, &m.a13, &m.a23, &m.a33);

    g_boxPrototype->cloneBox(&gameState->boxes[i]);
    gameState->boxes[i].scaleXYZ(boxScale);
    gameState->boxes[i].color = boxColor;
    gameState->boxes[i].modelToWorldTransform = m;
  }

  free(readBuffer);
  
  return gameState;

  #undef readLine_
}

void freeGameState(GameState *gameState)
{
  //TODO:
}

void hook_initialize()
{
  triangleShaderProgram = initializeProgram();

  gameState = createGameState();
  g_boxPrototype = createBoxMeshPrototype();
  g_boxPrototype->cloneBox(&gameState->boxes[0]);
  gameState->boxes[0].scaleXYZ({5.f, 5.f, 5.f});

  directionalLightIntensity = { 1.f, 1.f, 1.f };
  dirToLight = normalize({ 1.f, 1.f, 1.f });
  ambientLightIntensity = { .4f, .4f, .4f };

  glGenVertexArrays(1, &vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * (1 << 9), NULL, GL_DYNAMIC_DRAW);

  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * (1 << 9), NULL, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void updateCamera(const PlatformInput &platformInput, Camera *camera)
{  
  int mouseEdgeCameraTriggerX = platformInput.screenWidth / 20;
  int mouseEdgeCameraTriggerY = platformInput.screenHeight / 20;
  bool edgeCameraLeft = (platformInput.mouse.x < mouseEdgeCameraTriggerX);
  bool edgeCameraRight = (platformInput.screenWidth - platformInput.mouse.x) < mouseEdgeCameraTriggerX;
  bool edgeCameraTop = platformInput.mouse.y < mouseEdgeCameraTriggerY;
  bool edgeCameraBot = (platformInput.screenHeight - platformInput.mouse.y) < mouseEdgeCameraTriggerY;

  if (edgeCameraLeft || edgeCameraRight || platformInput.mouse.dx != 0)
  {
    float dir = 1.f;
    if (platformInput.mouse.dx > 0 || edgeCameraRight)
    {
      //I want the camera to rotate to right i.e. clockwise
      dir = -1.f;
    }
    camera->rotateY((dir * CAMERA_ROTATE_VELOCITY) * platformInput.deltaT);
  }
  if (edgeCameraTop || edgeCameraBot || platformInput.mouse.dy != 0)
  {
    float dir = 1.f;
    //note that if dy < 0, then user is dragging his cursor 
    //to the top of the screen. I want the camera to look upwards
    //i.e. align the thumb of your right hand to the left
    //and curl your fingers
    if (edgeCameraTop || platformInput.mouse.dy < 0)
    {
      dir = -1.f;
    }
    camera->rotateX((dir * CAMERA_ROTATE_VELOCITY) * platformInput.deltaT);
  }
  
  if (platformInput.kboard.isDown_w || platformInput.kboard.isDown_s)
  {
    float distance = CAMERA_MOVE_VELOCITY * platformInput.deltaT;
    if (platformInput.kboard.isDown_s)
    {
      distance *= -1.f;
    }
    camera->moveAlongDirection(distance);    
  }
  if (platformInput.kboard.isDown_d)
  {
    float distance = CAMERA_MOVE_VELOCITY * platformInput.deltaT;
    camera->moveRight(distance);
  }
  if (platformInput.kboard.isDown_a)
  {
    float distance = CAMERA_MOVE_VELOCITY * platformInput.deltaT;
    camera->moveLeft(distance);
  }

}

static void update(const PlatformInput &platformInput)
{
  bool shouldFixateCamera = platformInput.kboard.isDown_c && !platformInput.kboard.isDown_ctrl;
  if (!shouldFixateCamera)
  {
    updateCamera(platformInput, gameState->camera);
  }
  if (platformInput.kboard.isDown_z && platformInput.kboard.isDown_ctrl)
  {
    gameState->undoStack->undoChange(platformInput.T, gameState);
  }

  gameState->ray.direction = gameState->camera->calculateWorldMouseDirection(platformInput.mouse.x, platformInput.mouse.y, platformInput.screenWidth, platformInput.screenHeight);
  gameState->ray.startPos = gameState->camera->camPos;
  gameState->ray.color = {0.f, 1.f, 0.f};  

  if (gameState->selectedBox)
  {
    if (platformInput.mouse.isLMBDown)
    {
      if (!gameState->isSelectionDragActive)
      {
        if (gameState->selectedBox->checkRayIntersection(gameState->ray, NULL))
        {
          gameState->isSelectionDragActive = true;
        }
        else
        {
          gameState->selectedBox = NULL;
          gameState->isSelectionDragActive = false;
        }
      }
    }
    else
    {
      gameState->isSelectionDragActive = false;
    }
  }
  if (gameState->isSelectionDragActive)
  {
    Vec4 intersectionInfo{};
    bool didIntersect = gameState->ray.checkRayPlaneIntersection(gameState->floor->positions[0], gameState->floor->normals[0], &intersectionInfo);

    if (didIntersect)
    {
      Vec3 x = { intersectionInfo.x, intersectionInfo.y, intersectionInfo.z };
      gameState->undoStack->recordChange(platformInput.T, gameState);
      gameState->selectedBox->modelToWorldTransform = translate(x);
    }
  }

  if (!gameState->selectedBox && platformInput.mouse.didLMBClick)
  {
    Vec4 rayHitInfo;
    Box *rayHitBox = NULL;
    for (int i = 0; i < gameState->boxCount; i++)
    {
      Box *box = &gameState->boxes[i];
      if (box->isKilled)
      {
        continue;
      }
      Vec4 rayHitInfo_;
      if (box->checkRayIntersection(gameState->ray, &rayHitInfo_))
      {
        bool isUnderGround = rayHitInfo_.y < gameState->floor->positions[0].y;
        if (!isUnderGround && (!rayHitBox || rayHitInfo_.w < rayHitInfo.w))
        {
          rayHitInfo = rayHitInfo_;
          rayHitBox = box;
        }
      }
    }
    if (rayHitBox)
    {
      gameState->selectedBox = rayHitBox;
    }
  }

  if (platformInput.kboard.isDown_c && platformInput.kboard.isDown_ctrl && gameState->selectedBox && !gameState->isCopying)
  {
    gameState->selectedBox->cloneBox(gameState->copyingBox);
    gameState->isCopying = true;
  }
  if (gameState->isCopying)
  {
    Vec4 intersectionInfo{};
    bool didIntersect = gameState->ray.checkRayPlaneIntersection(gameState->floor->positions[0], gameState->floor->normals[0], &intersectionInfo);

    if (didIntersect)
    {
      Vec3 x = { intersectionInfo.x, intersectionInfo.y, intersectionInfo.z };
      gameState->copyingBox->modelToWorldTransform = translate(x);
    }
  }
  if (platformInput.mouse.didRMBClick && gameState->isCopying)
  {
    gameState->isCopying = false;
  }
  else if (platformInput.mouse.didLMBClick && gameState->isCopying)
  {
    for (int i = 0; i < gameState->boxCount; i++)
    {
      Box *b = &gameState->boxes[i];
      if (b->isKilled)
      {
        gameState->undoStack->recordChange(platformInput.T, gameState);
        gameState->copyingBox->cloneBox(b);
        break;
      }
    }
  }
}

static void renderLines(Camera *camera, Mesh *model);
static void renderRay(const Ray &ray, float rayLength)
{
  short indices[2] = { 0, 1 };
  Vec3 startPos = ray.startPos + Vec3{.01f, .01f, .01f};
  Vec3 positions[2] = { startPos, startPos + ray.direction * rayLength };

  Mesh mesh = {};
  mesh.indices = &indices[0];
  mesh.indicesCount = 2;
  mesh.positionsCount = 2;
  mesh.positions = &positions[0];
  mesh.color = ray.color;
  mesh.normalsCount = 0;
  mesh.modelToWorldTransform = identity();

  renderLines(gameState->camera, &mesh);
}

static void renderTriangles(Camera *camera, Mesh *model)
{ 
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3) * model->positionsCount, &model->positions[0]);

  int vboNormalOffset = sizeof(Vec3) * model->positionsCount;
  glBufferSubData(GL_ARRAY_BUFFER, vboNormalOffset, sizeof(Vec3) * model->normalsCount, &model->normals[0]);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) ((size_t) vboNormalOffset));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(short) * model->indicesCount, &model->indices[0]);

  glUseProgram(triangleShaderProgram.program);
  glUniformMatrix4fv(triangleShaderProgram.modelToWorldMatrixUnif, 1, GL_FALSE, &model->modelToWorldTransform.raw[0]);
  glUniformMatrix4fv(triangleShaderProgram.worldToCameraMatrixUnif, 1, GL_FALSE, &camera->worldToCameraTransform.raw[0]);
  glUniformMatrix4fv(triangleShaderProgram.cameraToClipMatrixUnif, 1, GL_FALSE, &camera->cameraToClipTransform.raw[0]);
  glUniform3fv(triangleShaderProgram.surfaceDiffuseColorUnif, 1, &model->color.raw[0]);
  glUniform3fv(triangleShaderProgram.dirToLightUnif, 1, &dirToLight.raw[0]);
  glUniform3fv(triangleShaderProgram.dirLightIntensityUnif, 1, &directionalLightIntensity.raw[0]);
  glUniform3fv(triangleShaderProgram.ambientLightIntensityUnif, 1, &ambientLightIntensity.raw[0]);
  glUniform1i(triangleShaderProgram.renderType, 1);

  glDrawElements(GL_TRIANGLES, model->indicesCount, GL_UNSIGNED_SHORT, NULL);
}

static void renderLines(Camera *camera, Mesh *model)
{
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec3) * model->positionsCount, &model->positions[0]);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(short) * model->indicesCount, &model->indices[0]);

  glUseProgram(triangleShaderProgram.program);
  glUniformMatrix4fv(triangleShaderProgram.modelToWorldMatrixUnif, 1, GL_FALSE, &model->modelToWorldTransform.raw[0]);
  glUniformMatrix4fv(triangleShaderProgram.worldToCameraMatrixUnif, 1, GL_FALSE, &camera->worldToCameraTransform.raw[0]);
  glUniformMatrix4fv(triangleShaderProgram.cameraToClipMatrixUnif, 1, GL_FALSE, &camera->cameraToClipTransform.raw[0]);
  glUniform3fv(triangleShaderProgram.surfaceDiffuseColorUnif, 1, &model->color.raw[0]);
  glUniform1i(triangleShaderProgram.renderType, 2);

  glDrawElements(GL_LINES, model->indicesCount, GL_UNSIGNED_SHORT, NULL);
}

char imguiInputBuffer[1 << 7];
void hook_updateAndRender(PlatformInput platformInput)
{
  update(platformInput);


  Vec3 &bgColor = gameState->bgColor;
  glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_MULTISAMPLE);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  
  renderTriangles(gameState->camera, gameState->floor);
  renderRay(gameState->ray, 100.f);
  
  //highlight the bottom of selected box
  if (gameState->selectedBox)
  {
    //x0,z0      x1,z0
    //
    //
    //x0,z1      x1,z1
    //
    float x0 = -.5f;
    float x1 = .5f;
    float z0 = -.5f;
    float z1 = .5f;
    float y = 0.f;
    Vec3 positions[] = {{x0, y, z0}, {x1, y, z0}, {x0, y, z1}, {x1, y, z1}};
    Vec3 up{ 0.f, 1.f, 0.f };
    Vec3 normals[] = {up, up, up, up};
    short indices[] = {0, 2, 3, 0, 3, 1};

    Mesh highlightMesh{};
    highlightMesh.color = COLOR_HIGHLIGHT_MESH;
    highlightMesh.positionsCount = 4;
    highlightMesh.normalsCount = 4;
    highlightMesh.indicesCount = 6;

    Vec3 padding = {2.f, 0.f, 2.f};
    Mat4 scale_ = scale(gameState->selectedBox->scale_ + padding);
    Mat4 scaleTransform = scale_;
    highlightMesh.modelToWorldTransform = gameState->selectedBox->modelToWorldTransform * scaleTransform;

    highlightMesh.normals = &normals[0];
    highlightMesh.indices = &indices[0];
    highlightMesh.positions = &positions[0];   
    
    renderTriangles(gameState->camera, &highlightMesh);
  }

  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  for (int i = 0; i < gameState->boxCount; i++)
  {
    Box *b = &gameState->boxes[i];
    if (!b->isKilled)
    {
      renderTriangles(gameState->camera, b);
    }
  }

  if (gameState->isCopying)
  {
    Vec3 originalColor = gameState->copyingBox->color;
    gameState->copyingBox->color = COLOR_COPYING_BOX;
    renderTriangles(gameState->camera, gameState->copyingBox);
    gameState->copyingBox->color = originalColor;
  }

  ImGui::Begin("Editor", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
  ImGui::SetWindowPos(ImVec2(0, 0), true);  
  ImGui::SetWindowSize(ImVec2(-1.f, 500.f));

  if (ImGui::BeginMenu("File"))
  {
    if (ImGui::MenuItem("New")) 
    {
      freeGameState(gameState);
      gameState = createGameState();
      g_boxPrototype->cloneBox(&gameState->boxes[0]);
      gameState->boxes[0].scaleXYZ({5.f, 5.f, 5.f});
    }
    if (ImGui::MenuItem("Import")) 
    {
      //NOTE: memory leak
      int filesCount;
      gameState->uiState.loadFilenameOpts = platform_listFilesInfolder(".\\levels", &filesCount);
      assert(gameState->uiState.loadFilenameOpts != NULL);
      gameState->uiState.loadFilenameCount = filesCount;
      gameState->uiState.isLoadPopupOpen = true;
      gameState->uiState.loadSelectionIdx = -1;
    }
    if (ImGui::MenuItem("Export")) 
    {
      gameState->uiState.isSavePopupOpen = true;
    }
    ImGui::EndMenu();
  }
  ImGui::NewLine();

  if (gameState->uiState.isSavePopupOpen)
  {
    if (!ImGui::IsPopupOpen("Save?")) ImGui::OpenPopup("Save?");
    if (ImGui::BeginPopupModal("Save?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ImGui::Text("Filename:");
      ImGui::InputText("##export", gameState->uiState.saveFilename, ARRAY_LENGTH(gameState->uiState.saveFilename));
      if (ImGui::Button(" Yes ", ImVec2(0, 0)))
      {
        saveLevel(gameState->uiState.saveFilename, gameState);
        gameState->uiState.isSavePopupOpen = false;
      }
      ImGui::SameLine();
      if (ImGui::Button(" No ", ImVec2(0, 0)))
      {
        gameState->uiState.isSavePopupOpen = false;
      }
      ImGui::EndPopup();      
    }
  }

  if (gameState->uiState.isLoadPopupOpen)
  {
    if (!ImGui::IsPopupOpen("Load?")) ImGui::OpenPopup("Load?");
    if (ImGui::BeginPopupModal("Load?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
      ImGui::Text("File to load:");
      int loadSelectionIdx = gameState->uiState.loadSelectionIdx;
      for (int i = 0; i < gameState->uiState.loadFilenameCount; i++)
      {
        if (ImGui::Selectable(gameState->uiState.loadFilenameOpts[i], i == loadSelectionIdx))
        {
          loadSelectionIdx = i;
        }
      }
      gameState->uiState.loadSelectionIdx = loadSelectionIdx;

      if (ImGui::Button(" Yes ", ImVec2(0, 0)))
      {
        int loadSelectionIdx = gameState->uiState.loadSelectionIdx;
        if (loadSelectionIdx > -1)
        {
          char *levelName = gameState->uiState.loadFilenameOpts[loadSelectionIdx];
          GameState *newGameState = loadLevel(levelName);
          freeGameState(gameState);
          gameState = newGameState;
        }
      }
      ImGui::SameLine();
      if (ImGui::Button(" Cancel ", ImVec2(0, 0)))
      {
        gameState->uiState.loadSelectionIdx = -1;
        gameState->uiState.isLoadPopupOpen = false;
      }
      ImGui::EndPopup();
    }
  }
    
  ImGui::Text("Background color");  
  ImGui::ColorEdit3("color 1", gameState->bgColor.raw, 0);

  ImGui::NewLine();

  ImGui::Text("Floor color");
  ImGui::ColorEdit3("color 2", gameState->floor->color.raw, 0);
  ImGui::NewLine();

  ImGui::Separator();

  ImGui::Text("Debug");
  ImGui::NewLine();
  {
    const Vec3 &camPos = gameState->camera->camPos;
    char buffer[1 << 7];
    sprintf(buffer, "Camera Position: <%.2f, %.2f, %.2f>", camPos.x, camPos.y, camPos.z);
    ImGui::Text(buffer);
  }
  {
    const Vec3 &lookDir = gameState->camera->lookDir;
    char buffer[1 << 7];
    sprintf(buffer, "Camera Direction: <%.2f, %.2f, %.2f>", lookDir.x, lookDir.y, lookDir.z);
    ImGui::Text(buffer);
  }
  {
    int numerator = gameState->undoStack->stackHeadAt + 1;
    int denom = gameState->undoStack->stackSize;
    char buffer[1 << 7];
    sprintf(buffer, "Undo Stack Size: %d/%d", numerator, denom);
    ImGui::Text(buffer);
  }

  ImGui::Separator();

  ImGui::Text("Box selection");
  if (gameState->selectedBox)
  {
    float minScale = 1.f;
    float maxScale = 20.f;
    Box *box = gameState->selectedBox;
    Vec3 scaleFactor = box->scale_;

    ImGui::Text("Scale X");    
    ImGui::SliderFloat("##Scale X", &scaleFactor.x, minScale, maxScale, "%.3f");
    ImGui::NewLine();

    ImGui::Text("Scale Y");
    ImGui::SliderFloat("##Scale Y", &scaleFactor.y, minScale, maxScale, "%.3f");
    ImGui::NewLine();

    ImGui::Text("Scale Z");
    ImGui::SliderFloat("##Scale Z", &scaleFactor.z, minScale, maxScale, "%.3f");
    ImGui::NewLine();

    if (scaleFactor.x != box->scale_.x || scaleFactor.y != box->scale_.y || scaleFactor.z != box->scale_.z)
    {
      gameState->undoStack->recordChange(platformInput.T, gameState);
      box->scaleXYZ(scaleFactor);
    }
    ImGui::NewLine();

    ImGui::Text("Color");
    ImGui::ColorEdit3("color 3", box->color.raw, 0);
  }

  ImGui::End();  
}