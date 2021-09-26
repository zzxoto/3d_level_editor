#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <cstdarg>
#include <stdlib.h>
#include "zzxoto_gl.h"
#include "zzxoto_platform.h"

//all the imgui cpp files have been inlined here for 
//faster compilation
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "dearimgui/imgui.cpp"
#include "dearimgui/imgui_widgets.cpp"
#include "dearimgui/imgui_tables.cpp"
#include "dearimgui/imgui_draw.cpp"
#include "dearimgui/imgui_impl_opengl3.cpp"
#include "dearimgui/imgui_impl_win32.cpp"
#include "dearimgui/imgui_demo.cpp"

typedef HGLRC type_wglCreateContextAttribsARB(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL type_wglChoosePixelFormatARB(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

static bool g_running = true;

void platform_debugString(const char *buffer)
{
  OutputDebugStringA(buffer);
}

void platform_writeToFile(const char *filename, const char *buffer)
{
  FILE *fp;

  fp = fopen(filename, "w");
  assert(fp != NULL);
  int writeCount = fputs(buffer, fp);
  assert(writeCount != EOF);
  fclose(fp);
}

int platform_readFile(const char *filename, char *buffer)
{
  FILE *fp;

  fp = fopen(filename, "r");
  assert(fp);

  fseek(fp , 0 , SEEK_END);
  size_t fileSize = ftell(fp);
  rewind(fp);

  int readCount = fread(buffer, sizeof(char), fileSize, fp);
  int err = ferror(fp);
  if (err)
  {
    int err = errno;
    printf("errno: %d", err);
    assert(false);
  }
  fclose(fp);

  return readCount;
}

//NOTE: this api design promotes memory leak
//NOTE: this api will have undefined behavior
//if the directory contains too many files.
char **platform_listFilesInfolder(const char *folder_, int *filesCount)
{
  char folder[1 << 7];
  {
    int i = 0;
    while(*(folder_ + i))
    {
      folder[i] = *(folder_ + i);
      i++;
    }
    *(folder + i) = '\\';
    *(folder + i + 1) = '*';
    *(folder + i + 2) = 0;
  }
  
  WIN32_FIND_DATA FindFileData;
  HANDLE hFind;
  hFind = FindFirstFile(folder, &FindFileData);

  assert(hFind != INVALID_HANDLE_VALUE);

  char *str = (char *) malloc(1 << 12);
  char **strList = (char **) malloc(sizeof(char **) * 50);

  int strIdx = 0;
  int count = 0;
  do
  {
    char *s = FindFileData.cFileName;
    if (strcmp(s, ".") == 0 || strcmp(s, "..") == 0)
    {
      continue;
    }
    strcpy(str + strIdx, s);
    strList[count++] = str + strIdx;
    strIdx += strlen(s) + 1;
  } while(FindNextFileA(hFind, &FindFileData));

  *filesCount = count; 
  
  return strList;
}

static LRESULT win_windowProcedure_callback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
  LRESULT result = 1;
  switch(Message)
  {
    case WM_QUIT:
    case WM_CLOSE:
    {
      g_running = false;
    }
    break;
    case WM_PAINT:
    {
      //window would keep spamming WM_PAINT message 
      //had we not called BeginPaint and EndPaint
      PAINTSTRUCT Paint;
      BeginPaint(Window, &Paint);
      EndPaint(Window, &Paint);
    } 
    break;
    default:
    {
      result = DefWindowProc(Window, Message, WParam, LParam);
    } break;
  }

  return result;
}

typedef long long int int64;

static int64 win_timestamp()
{
  LARGE_INTEGER queryResult;
  QueryPerformanceCounter(&queryResult);
  int64 t = queryResult.QuadPart;

  return t;
}

static float win_timeElapsedSeconds(int64 t1, int64 t2, int64 frequency)
{
  int64 diff = t1 - t2;
  if (diff < 0)
  {
    diff = t2 - t1;
  }

  return ((float) diff) / ((float) frequency);
}

//IMGUI says:
// However it is generally more correct and easier that you poll flags 
// from the previous frame, then submit your inputs, then call NewFrame().
static void win_processMessage(PlatformInput *platformInput)
{
  ImGuiIO& imguiIO = ImGui::GetIO();
  MSG message;
  while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&message);
    LRESULT result = ImGui_ImplWin32_WndProcHandler(message.hwnd, message.message, message.wParam, message.lParam);

    if (!result)
    {
      UINT msg = message.message;
      if (msg == WM_KEYDOWN || msg == WM_KEYUP)
      {
        if (!imguiIO.WantCaptureKeyboard)
        {
          bool isDown = (message.lParam & (1 << 31)) == 0;

          static int keyboardEventCount = 0;

          switch(message.wParam)
          {
            case VK_SPACE:
            {
              platformInput->kboard.isSpaceDown = isDown;
            }
            break;
            case VK_LEFT:
            {
              platformInput->kboard.isLeftDown = isDown;
            }
            break;
            case VK_UP:
            {
              platformInput->kboard.isUpDown = isDown;
            }
            break;
            case VK_RIGHT:
            {
              platformInput->kboard.isRightDown = isDown;
            }
            break;
            case VK_DOWN:
            {
              platformInput->kboard.isDownDown = isDown;
            }
            break;
            case VK_LCONTROL:
            case VK_RCONTROL:
            case VK_CONTROL:
            {
              platformInput->kboard.isDown_ctrl = isDown;
            }
            break;
            case 0x57:
            {
              platformInput->kboard.isDown_w = isDown;
              platform_debugString("W pressed\n");
            }
            break;
            case 0x41:
            {
              platformInput->kboard.isDown_a = isDown;
            }
            break;
            case 0x53:
            {
              platformInput->kboard.isDown_s = isDown;
            }
            break;
            case 0x44:
            {
              platformInput->kboard.isDown_d = isDown;
            }
            break;
            case 0x43:
            {
              platformInput->kboard.isDown_c = isDown;
            }
            break;
            case 0x5A:
            {
              platformInput->kboard.isDown_z = isDown;
            }
            break;
          }
        }
      }
      else if (msg == WM_MOUSEMOVE || msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
      {
        if (!imguiIO.WantCaptureMouse)
        {
          UINT msg = message.message;

          bool newLMBDown = message.wParam & MK_LBUTTON;
          if (!newLMBDown && platformInput->mouse.isLMBDown)
          {
            platformInput->mouse.didLMBClick = true;
          }
          platformInput->mouse.isLMBDown = newLMBDown;

          bool newRMBDown = message.wParam & MK_RBUTTON;
          if (!newRMBDown && platformInput->mouse.isRMBDown)
          {
            platformInput->mouse.didRMBClick = true;
          }
          platformInput->mouse.isRMBDown = newRMBDown;
          
          int newMouseX = message.lParam & 0xffff;
          int newMouseY = (message.lParam >> 16) & 0xffff;

          int dx = newMouseX - platformInput->mouse.x;
          int dy = newMouseY - platformInput->mouse.y;
          if (dx > 100 || dx < -100 || dy > 100 || dy < -100)
          {
            dx = 0;
            dy = 0;
          }

          platformInput->mouse.x = newMouseX;
          platformInput->mouse.y = newMouseY;
          platformInput->mouse.dx = dx;
          platformInput->mouse.dy = dy;
        }
      }
      else
      {
        DispatchMessageA(&message);
      }
    }
  }
}

//call this before calling zzxoto_getGLInfo();
static bool win_initOpenGL(HINSTANCE hInstance, int major, int minor, HGLRC *glContext, HWND *winHandle)
{
  HWND tempHandle = CreateWindowEx(
    0,                         // no extended styles
    "zzxoto", // class name                   
    "My Game",                 // window name                  
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,             // default horizontal position  
    CW_USEDEFAULT,             // default vertical position    
    CW_USEDEFAULT,             // default width                
    CW_USEDEFAULT,             // default height               
    0,                         // no parent or owner window    
    0,                         // class menu used              
    hInstance,                 // instance handle              
    0);                        // no window creation data

  HDC tempDC = GetDC(tempHandle);

  //see https://guide.handmadehero.org/code/day235/ for pixelformatdescriptor nonsense
  PIXELFORMATDESCRIPTOR pfd = {};
  pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
  pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cDepthBits = 32;
  pfd.iLayerType = PFD_MAIN_PLANE;

  int pixelFormatIndex = ChoosePixelFormat(tempDC, &pfd);
  SetPixelFormat(tempDC, pixelFormatIndex, &pfd);
  PIXELFORMATDESCRIPTOR pixelFormatPFD;
  DescribePixelFormat(tempDC, pixelFormatIndex, sizeof(PIXELFORMATDESCRIPTOR), &pixelFormatPFD);

  HGLRC tempGLContext = wglCreateContext(tempDC); assert(tempGLContext);

  //NOTE: have to create the old context with wglCreateContext(windowDC)
  //and call wglMakeCurrent to be able to do wglGetProcAddress("wglCreateContextAttribsARB"),
  //else the wglGetProcAddress would return null
  wglMakeCurrent(tempDC, tempGLContext);  
  type_wglCreateContextAttribsARB* wglCreateContextAttribsARB;
  type_wglChoosePixelFormatARB *wglChoosePixelFormatARB;
  wglCreateContextAttribsARB = (type_wglCreateContextAttribsARB *) wglGetProcAddress("wglCreateContextAttribsARB");
  wglChoosePixelFormatARB = (type_wglChoosePixelFormatARB *) wglGetProcAddress("wglChoosePixelFormatARB");

  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(tempGLContext);
  ReleaseDC(tempHandle, tempDC);
  DestroyWindow(tempHandle);

  HWND wndHandle = CreateWindowEx(
    0,                         // no extended styles
    "zzxoto", // class name 
    "My Game",                 // window name                  
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,             // default horizontal position  
    CW_USEDEFAULT,             // default vertical position    
    CW_USEDEFAULT,             // default width                
    CW_USEDEFAULT,             // default height               
    0,                         // no parent or owner window    
    0,                         // class menu used              
    hInstance,                 // instance handle              
    0);                        // no window creation data

  HDC windowDC = GetDC(wndHandle);

  ShowWindow(wndHandle, SW_MAXIMIZE);
  UpdateWindow(wndHandle); 

  int piAttribIList[] = { 
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_COLOR_BITS_ARB, 32,
    WGL_RED_BITS_ARB, 8,
    WGL_GREEN_BITS_ARB, 8,
    WGL_BLUE_BITS_ARB, 8,
    WGL_ALPHA_BITS_ARB, 8,
    WGL_DEPTH_BITS_ARB, 16,
    WGL_STENCIL_BITS_ARB, 0,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
    WGL_SAMPLES_ARB, 8,
    0, 0
  };
  int pixelFormat2 = 0;
  UINT nNumFormats;
  wglChoosePixelFormatARB(windowDC, piAttribIList, NULL, 1, &pixelFormat2, &nNumFormats);
  assert(nNumFormats > 0);  
  SetPixelFormat(windowDC, pixelFormat2, &pfd);

  int attribs[] =
  {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 3,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0
  };

  HGLRC newGLContext = wglCreateContextAttribsARB(windowDC, 0, attribs); 
  DWORD error3 = GetLastError();  
  assert(newGLContext);

  wglMakeCurrent(windowDC, newGLContext);
  zzxoto_glLoadFunctions();

  *glContext = newGLContext;
  *winHandle = wndHandle;

  return true;
}
#define win_main WinMain
int win_main(HINSTANCE hInstance, HINSTANCE foo1, LPSTR foo2, int showCode)
{
  //2. register WindowClass
  WNDCLASS windowClass = {};
  windowClass.style = CS_HREDRAW|CS_VREDRAW;
  windowClass.lpfnWndProc = win_windowProcedure_callback;
  windowClass.hInstance = hInstance;
  windowClass.lpszClassName = "zzxoto";
  BOOL registerWindowClassResult = RegisterClass(&windowClass);

  PlatformInput platformInput = {};

  int64 clockFrequency = 0;
  {
    LARGE_INTEGER queryResult;
    QueryPerformanceFrequency(&queryResult);
    clockFrequency = queryResult.QuadPart;
  }

  if(registerWindowClassResult)
  {
    int updateHz = 60;
    float targetSecondsPerFrame = 1.0f / updateHz;
    bool canUseSleep = timeBeginPeriod(1) == TIMERR_NOERROR;

    //sets sleep granularity to 1 milliseconds
    //otherwise windows seem to sleep for roughly 16 ms
    //for every Sleep(1) call
    HGLRC openGLContext;
    HWND windowHandle;
    
    assert(win_initOpenGL(hInstance, 3, 3, &openGLContext, &windowHandle));

    zzxoto_GLInfo glInfo = zzxoto_getGLInfo();
    assert(glInfo.version >= 330);

    if (openGLContext)
    {
      hook_initialize();
      
      ImGui::CreateContext();
      ImGui::StyleColorsDark();
      ImGui_ImplWin32_Init(windowHandle);
      ImGui_ImplOpenGL3_Init("#version 330");

      int64  timestamp_lastFrame = win_timestamp();
      platformInput.deltaT = targetSecondsPerFrame;
      platformInput.T = win_timeElapsedSeconds(timestamp_lastFrame, 0, clockFrequency);
      while(g_running)
      {
        platformInput.mouse.dx = 0;
        platformInput.mouse.dy = 0;
        platformInput.mouse.didLMBClick = false;
        platformInput.mouse.didRMBClick = false;
        win_processMessage(&platformInput);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        RECT client;
        GetClientRect(windowHandle, &client);
        platformInput.screenWidth = client.right;
        platformInput.screenHeight = client.bottom;
        glViewport(0, 0, client.right, client.bottom);

        hook_updateAndRender(platformInput);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        HDC windowDC = GetDC(windowHandle);
        SwapBuffers(windowDC);
        ReleaseDC(windowHandle, windowDC);

        int64 timestamp_nextFrame = win_timestamp();
        float deltaTimestamp = win_timeElapsedSeconds(timestamp_nextFrame, timestamp_lastFrame, clockFrequency);
        while (deltaTimestamp < targetSecondsPerFrame)
        {
          if (canUseSleep)
          {
            Sleep(1);
          }
          timestamp_nextFrame = win_timestamp();
          deltaTimestamp = win_timeElapsedSeconds(timestamp_nextFrame, timestamp_lastFrame, clockFrequency);
        }
        platformInput.T = win_timeElapsedSeconds(timestamp_nextFrame, 0, clockFrequency);
        platformInput.deltaT = deltaTimestamp;
        timestamp_lastFrame = timestamp_nextFrame;

        #if 0
        char printBuffer[50];
        sprintf(printBuffer, "FPS: %.2f; targetDelta: %.3f\n", 1.0f / platformInput.deltaT);
        OutputDebugStringA(printBuffer);
        #endif
      } //while (game)

      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplWin32_Shutdown();
      ImGui::DestroyContext();
      wglDeleteContext(openGLContext);
    } //if openGLContext

    DestroyWindow(windowHandle);
  }
  else
  {
    //TODO: handle error
  }

  return 0;
}
