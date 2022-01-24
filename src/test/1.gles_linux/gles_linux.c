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

//    x,     y,     z
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

//R G B A
float colors[] = {
    0.0f,  1.0f,  0.0f,  1.0f,//底面 左下角 绿色
    0.0f,  1.0f,  0.0f,  1.0f,//底面 右下角 绿色
    1.0f,  0.5f,  0.0f,  1.0f,//底面 右上角 红+0.5绿 橙色
    1.0f,  0.5f,  0.0f,  1.0f,//底面 左上角 红+0.5绿 橙色
    1.0f,  0.0f,  0.0f,  1.0f,//正面 左下角 红色
    1.0f,  0.0f,  0.0f,  1.0f,//正面 右下角 红色
    0.0f,  0.0f,  1.0f,  1.0f,//正面 右上角 蓝色
    1.0f,  0.0f,  1.0f,  1.0f//正面 左上角 紫色
};

//一个正方体有6个面，一共12个三角形组成。
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
    //使用一个透视矩阵乘以当前矩阵
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

    //设置最小1帧才能交换，这样可以避免画面撕裂。
    eglSwapInterval(glDisplay, 1);

    //设置清屏色
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    //设置深度1
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    //<=
    glDepthFunc(GL_LEQUAL);

    //控制透视矫正的偏好，选择质量最优的选项
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glViewport(0, 0, screenwidth, screenheight);
    //将后续矩阵运算应用于投影矩阵栈。
    glMatrixMode(GL_PROJECTION);
    //设置矩阵为单位矩阵
    glLoadIdentity();
    //模拟GLU透视
    emulateGLUperspective(45.0f, (float) screenwidth / (float) screenheight, 0.1f, 100.0f);
    glViewport(0, 0, screenwidth, screenheight);
    //将后续矩阵运算应用于视图矩阵栈。
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //接下来就是绘图逻辑了：
    //loop: {
    //    //7. 画画画画画画画...
    //    //8. 交换缓冲区，让绘图的缓冲区显示出来
    //    egl.eglSwapBuffers(display, surface);
    //}
}

void draw_frame()
{
    //清屏
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //设置单位矩阵（针对视图矩阵栈）
    glLoadIdentity();

    //z轴下移8。就是拿远一点，不然就占满屏幕了
    glTranslatef(0.0f, 0.0f, -8.0f);

    //矩阵旋转，旋转的角度：mCubeRotation，旋转围绕的向量（0,0,0->0.1,1.0.10 轴）
    glRotatef(mCubeRotation, 0.1f, 1.0f, 1.0f);

    glFrontFace(GL_CW);

    //设置顶点数组
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    //设置颜色数组
    glColorPointer(4, GL_FLOAT, 0, colors);
    //启用glVertexPointer设置的顶点数组
    glEnableClientState(GL_VERTEX_ARRAY);
    //启用glColorPointer设置的颜色数组
    glEnableClientState(GL_COLOR_ARRAY);

    //真正绘制的地方
    //根据索引数组绘制三角形，一共要绘制12个三角形，共有12x3=36个索引
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

    //关闭顶点数组
    glDisableClientState(GL_VERTEX_ARRAY);
    //关闭颜色数组
    glDisableClientState(GL_COLOR_ARRAY);
    //变更旋转角度，每次顺时针加1度
    mCubeRotation -= 1.0f;
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
