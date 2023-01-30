// 这个要放在第一位，GLAD的头文件包含了正确的OpenGL头文件（例如GL/gl.h），所以需要在其它依赖于OpenGL的头文件之前包含GLAD。
// GLAD是用来管理OpenGL的函数指针的，所以在调用任何OpenGL的函数之前我们需要初始化GLAD。
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow * window);

int main()
{
    //初始化glfw
    glfwInit();
    //创建glfw上下文

    //设置使用的opengl版本 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    //告诉gflw使用核心渲染模式(core-profile)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    //Mac OSX 需要开启向前兼容
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //创建窗口
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();//反初始化
        return -1;
    }

    //将窗口设置为当前线程的主上下文，一个线程只能有一个上下文
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //glad使用glloader加载器（加载器由glfw的gflwGetProcAddress实现）获取函数名对应的函数地址
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //下面就可以使用glad来调用opengl函数了

    while(!glfwWindowShouldClose(window)){//检测close flag是否为true
        processInput(window);

        //不清屏，就会看到上一次的渲染结果。这个一般都不是我们想要的
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);//使用clearColor填充color buffer，实现清除效果

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

/**
* 当调整窗口大小时，会调用这个回调函数
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    //根据窗口的大小变更OpenGl的ViewPort大小
    glViewport(0,0,width,height);
}

void processInput(GLFWwindow * window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
