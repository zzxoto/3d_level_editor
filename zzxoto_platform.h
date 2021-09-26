#ifndef ZZXOTO_PLATFORM_H
#define ZZXOTO_PLATFORM_H

struct PlatformInput
{
  struct {
    bool isLMBDown;
    bool isRMBDown;
    bool didLMBClick;
    bool didRMBClick;
    int x;
    int y;
    int dx;
    int dy;
  } mouse;

  struct {
    bool isLeftDown;
    bool isRightDown;
    bool isUpDown;
    bool isDownDown;
    bool isSpaceDown;
    bool isDown_w;
    bool isDown_a;
    bool isDown_s;
    bool isDown_d;
    bool isDown_c;
    bool isDown_z;
    bool isDown_ctrl;
  } kboard;

  float deltaT;
  float T;
  int screenWidth;
  int screenHeight;
};

void platform_debugString(const char *buffer);
void platform_writeToFile(const char *filename, const char *buffer);
int platform_readFile(const char *filename, char *buffer);
char **platform_listFilesInfolder(const char *folder, int *filesCount);

void hook_initialize();
void hook_updateAndRender(PlatformInput platformInput);
#endif