#include "xentax.h"

typedef HGLRC HRC;
static HWND window = 0;
static HDC device = 0;
static HRC render = 0;

static INT_PTR CALLBACK PreviewProc(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam);
static INT_PTR EvInitDialog(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam);
static INT_PTR EvCommand(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam);
static INT_PTR EvDestroy(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam);
static INT_PTR EvPaint(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam);
static INT_PTR OnOK(HWND dialog, WPARAM wparam, LPARAM lparam);
static INT_PTR OnCancel(HWND dialog, WPARAM wparam, LPARAM lparam);

static BOOL InitOpenGL(void);
static BOOL FreeOpenGL(void);
static BOOL IsRenderContextValid(void);
static BOOL RenderFrame(void);
static BOOL SwapBuffers(void);

BOOL PreviewDialog(void)
{
 if(DialogBox(GetModuleHandle(NULL), TEXT("IDD_PREVIEW"), NULL, PreviewProc) == TRUE) return TRUE;
 return FALSE;
}

BOOL OnExtensionError(LPCSTR error)
{
 // forgive ARB_imaging
 if(strcmp("glColorTable", error) == 0) return TRUE;
 if(strcmp("glColorSubTable", error) == 0) return TRUE;
 if(strcmp("glColorTableParameteriv", error) == 0) return TRUE;
 if(strcmp("glColorTableParameterfv", error) == 0) return TRUE;
 if(strcmp("glCopyColorSubTable", error) == 0) return TRUE;
 if(strcmp("glCopyColorTable", error) == 0) return TRUE;
 if(strcmp("glGetColorTable", error) == 0) return TRUE;
 if(strcmp("glGetColorTableParameterfv", error) == 0) return TRUE;
 if(strcmp("glGetColorTableParameteriv", error) == 0) return TRUE;
 if(strcmp("glHistogram", error) == 0) return TRUE;
 if(strcmp("glResetHistogram", error) == 0) return TRUE;
 if(strcmp("glGetHistogram", error) == 0) return TRUE;
 if(strcmp("glGetHistogramParameterfv", error) == 0) return TRUE;
 if(strcmp("glGetHistogramParameteriv", error) == 0) return TRUE;
 if(strcmp("glMinmax", error) == 0) return TRUE;
 if(strcmp("glResetMinmax", error) == 0) return TRUE;
 if(strcmp("glGetMinmaxParameterfv", error) == 0) return TRUE;
 if(strcmp("glGetMinmaxParameteriv", error) == 0) return TRUE;
 if(strcmp("glConvolutionFilter1D", error) == 0) return TRUE;
 if(strcmp("glConvolutionFilter2D", error) == 0) return TRUE;
 if(strcmp("glConvolutionParameterf", error) == 0) return TRUE;
 if(strcmp("glConvolutionParameterfv", error) == 0) return TRUE;
 if(strcmp("glConvolutionParameteri", error) == 0) return TRUE;
 if(strcmp("glConvolutionParameteriv", error) == 0) return TRUE;
 if(strcmp("glCopyConvolutionFilter1D", error) == 0) return TRUE;
 if(strcmp("glCopyConvolutionFilter2D", error) == 0) return TRUE;
 if(strcmp("glGetConvolutionFilter", error) == 0) return TRUE;
 if(strcmp("glGetConvolutionParameterfv", error) == 0) return TRUE;
 if(strcmp("glGetConvolutionParameteriv", error) == 0) return TRUE;
 if(strcmp("glSeparableFilter2D", error) == 0) return TRUE;
 if(strcmp("glGetSeparableFilter", error) == 0) return TRUE;
 if(strcmp("glGetMinmax", error) == 0) return TRUE;
 return FALSE;
}

INT_PTR CALLBACK PreviewProc(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam)
{
 switch(message) {
   case(WM_INITDIALOG) : return EvInitDialog(dialog, message, wparam, lparam);
   case(WM_COMMAND) : return EvCommand(dialog, message, wparam, lparam);
   case(WM_DESTROY) : return EvDestroy(dialog, message, wparam, lparam);
   case(WM_PAINT) : return EvPaint(dialog, message, wparam, lparam);
  }
 return FALSE;
}

INT_PTR EvInitDialog(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam)
{
 window = dialog;
 if(InitOpenGL() == FALSE) {
    EndDialog(dialog, IDCANCEL);
   }
 return TRUE;
}

INT_PTR EvCommand(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam)
{
 int cmd = LOWORD(wparam);
 if(cmd == IDOK) return OnOK(dialog, wparam, lparam);
 else if(cmd == IDCANCEL) return OnCancel(dialog, wparam, lparam);
 return FALSE;
}

INT_PTR EvDestroy(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam)
{
 FreeOpenGL();
 window = NULL;
 return TRUE;
}

INT_PTR EvPaint(HWND dialog, UINT message, WPARAM wparam, LPARAM lparam)
{
 RenderFrame();
 SwapBuffers();
 ValidateRect(dialog, NULL);
 return TRUE;
}

INT_PTR OnOK(HWND dialog, WPARAM wparam, LPARAM lparam)
{
 EndDialog(dialog, IDOK);
 return TRUE;
}

INT_PTR OnCancel(HWND dialog, WPARAM wparam, LPARAM lparam)
{
 EndDialog(dialog, IDCANCEL);
 return TRUE;
}

BOOL InitOpenGL(void)
{
 // validate window
 if(!window || !IsWindow(window)) {
    MessageBox(window, TEXT("Invalid client window."), TEXT("Error"), MB_ICONSTOP);
    return FALSE;
   }

 // create device context
 device = GetDC(window);
 if(!device) {
    MessageBox(window, TEXT("Failed to create device context."), TEXT("Error"), MB_ICONSTOP);
    return FALSE;
   }

 // initialize device pixel format
 PIXELFORMATDESCRIPTOR pfd;
 ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
 pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
 pfd.nVersion   = 1;
 pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
 pfd.iPixelType = PFD_TYPE_RGBA;
 pfd.cColorBits = 32;
 pfd.cDepthBits = 32;
 pfd.iLayerType = PFD_MAIN_PLANE;

 // choose and set pixel format
 int pixel_format = ChoosePixelFormat(device, &pfd);
 if(pixel_format == 0) return FALSE;
 if(!SetPixelFormat(device, pixel_format, &pfd)) return FALSE;

 // create render context
 render = wglCreateContextEx(device);
 if(!render) {
    ReleaseDC(window, device);
    device = 0;
    MessageBox(window, TEXT("Failed to create render context."), TEXT("Error"), MB_ICONSTOP);
    return FALSE;
   }

 // make render context current
 wglMakeCurrent(device, render);

 // load extensions
 wglLoadExtensions(OnExtensionError);
 return TRUE;
}

BOOL FreeOpenGL(void)
{
 // validate window
 if(!window || !IsWindow(window)) return FALSE;

 // delete render context
 if(render) {
    if(wglMakeCurrent(NULL, NULL) == FALSE) {
       MessageBox(window, TEXT("Failed to release OpenGL rendering context."), TEXT("Error"), MB_ICONSTOP);
       return FALSE;
      }
    if(wglDeleteContext(render) == FALSE) {
       MessageBox(window, TEXT("Failed to delete OpenGL rendering context."), TEXT("Error"), MB_ICONSTOP);
       return FALSE;
      }
    render = 0;
   }

 // delete device context
 if(device) {
    if(ReleaseDC(window, device) == 0) {
       MessageBox(window, TEXT("Failed to release device context."), TEXT("Error"), MB_ICONSTOP);
       return FALSE;
      }
    device = 0;
   }

 return TRUE;
}

BOOL IsRenderContextValid(void)
{
 return (render == 0 ? FALSE : TRUE);
}

BOOL RenderFrame(void)
{
 if(!IsRenderContextValid()) return FALSE;
 glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 return TRUE;
}

BOOL SwapBuffers(void)
{
 return ::SwapBuffers(device);
}