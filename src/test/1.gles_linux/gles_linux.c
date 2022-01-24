// 08-JAN-2017
// $ gcc gles_linux.c -lm -lSDL2 -lEGL -lGLESv1_CM -o gles_linux

// GLES
#include <GLES/gl.h>
#include <GLES/glext.h>

// EGL 是OpenGL ES 和 本地窗口系统（Native Window System）之间的通信接口
#include <GLES/egl.h>

// SDL2
#include <SDL2/SDL.h> // For Events
#include <SDL2/SDL_syswm.h>

EGLDisplay  glDisplay;
EGLConfig   glConfig;
EGLContext  glContext;
EGLSurface  glSurface;

const char  *gl_vendor, *gl_renderer, *gl_version, *gl_extensions;

SDL_Window *glesWindow = NULL;

float mCubeRotation = 0.0f;

float vertices[] = {
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f
};

float colors[] = {
    0.0f,  1.0f,  0.0f,  1.0f,
    0.0f,  1.0f,  0.0f,  1.0f,
    1.0f,  0.5f,  0.0f,  1.0f,
    1.0f,  0.5f,  0.0f,  1.0f,
    1.0f,  0.0f,  0.0f,  1.0f,
    1.0f,  0.0f,  0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,  1.0f,
    1.0f,  0.0f,  1.0f,  1.0f
};

char indices[] = {
    0, 4, 5, 0, 5, 1,
    1, 5, 6, 1, 6, 2,
    2, 6, 7, 2, 7, 3,
    3, 7, 4, 3, 4, 0,
    4, 7, 6, 4, 6, 5,
    3, 0, 1, 3, 1, 2
};


void emulateGLUperspective(GLfloat fovY, GLfloat aspect, GLfloat zNear,
                           GLfloat zFar)
{
    GLfloat fW, fH;
    fH = tan(fovY / 180 * M_PI) * zNear / 2;
    fW = fH * aspect;
    glFrustumf(-fW, fW, -fH, fH, zNear, zFar);
}

void init_GLES(void)
{
    int screenwidth  = 800;
    int screenheight = 480;
    int screenbpp    =  16;
    int fullscreen   =   0;

    EGLint numConfigs, majorVersion, minorVersion;

    //创建窗口
    glesWindow = SDL_CreateWindow("LOR_GLES_DEMO", 0, 0, screenwidth, screenheight,
                                  fullscreen ? (SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN) : SDL_WINDOW_OPENGL);
    //获取SDL版本
    SDL_SysWMinfo sysInfo;
    SDL_VERSION(&sysInfo.version)

    //获取默认显示设备
    glDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    //egl初始化
    eglInitialize(glDisplay, &majorVersion, &minorVersion);

    EGLint egl_config_attr[] = {
            EGL_BUFFER_SIZE,    16,
            EGL_DEPTH_SIZE,     16,
            EGL_STENCIL_SIZE,   0,
            EGL_SURFACE_TYPE,   EGL_WINDOW_BIT,
            EGL_NONE
    };
    // 选择 EGL 配置。
    // 可以自己先设定好一个期望的 EGL 配置，比如 RGB 三种颜色各占几位，可以随便配。
    // 因为 EGL 可能无法满足所有要求，这时，它会返回一些与你的要求最接近的配置供选择
    eglChooseConfig(glDisplay, egl_config_attr, &glConfig, 1, &numConfigs);

    SDL_GetWindowWMInfo(glesWindow, &sysInfo);

    //创建Context
    glContext = eglCreateContext(glDisplay, glConfig, EGL_NO_CONTEXT, NULL);
    //获取Surface
    glSurface = eglCreateWindowSurface(glDisplay, glConfig,
                                       (EGLNativeWindowType)sysInfo.info.x11.window, 0); // X11?

    //将渲染环境设置到当前线程
    //相当于让其拥有了 Open GL 的绘图能力。为什么做了这步操作线程就拥有了 Open GL 的绘图能力？下文会具体讲解。
    eglMakeCurrent(glDisplay, glSurface, glSurface, glContext);
    //接下来就是绘图逻辑了：
    //loop: {
    //    //7. 画画画画画画画...
    //    //8. 交换缓冲区，让绘图的缓冲区显示出来
    //    egl.eglSwapBuffers(display, surface);
    //}

    //设置最小1帧才能交换，这样可以避免画面撕裂。
    eglSwapInterval(glDisplay, 1);

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glViewport(0, 0, screenwidth, screenheight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    emulateGLUperspective(45.0f, (float) screenwidth / (float) screenheight, 0.1f,
                          100.0f);
    glViewport(0, 0, screenwidth, screenheight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void draw_frame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f);
    glRotatef(mCubeRotation, 1.0f, 1.0f, 1.0f);
    glFrontFace(GL_CW);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(4, GL_FLOAT, 0, colors);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glLoadIdentity();
    mCubeRotation -= 0.50f;
}

int main(void)
{
    int loop = 1;
    SDL_Event event;
    init_GLES();

    while (loop) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    loop = 0;
                    break;
                }
            }
        }

        draw_frame();
        eglSwapBuffers(glDisplay, glSurface);
    }

    // Cleaning
    eglDestroySurface(glDisplay, glSurface);
    eglDestroyContext(glDisplay, glContext);
    eglTerminate(glDisplay);
    SDL_DestroyWindow(glesWindow);

    return 0;
}
