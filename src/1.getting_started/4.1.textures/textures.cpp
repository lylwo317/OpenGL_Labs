#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "learnopengl/shader.h"
#include "learnopengl/filesystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main(){

    //初始化glfw
    glfwInit();

    //设置使用的OpenGL版本 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    //Mac OSX 需要开启向前兼容
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);

    if (window == nullptr){
        std::cerr << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    //设置当前线程为上下文，也就是说这里会存储opengl的相关状态
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    //准备shader program
    Shader shader("4.1.texture.vs", "4.1.texture.fs");

    //准备VAO，VBO
    //画一个三角形
    float vertices[] = {
            // pos                  // color                // texture
            -0.5f, 0.5f, 0.0f,       0.0f, 1.0f, 0.0f,       0.0f, 1.0f, // top left
            0.5f, 0.5f, 0.0f,       0.0f, 1.0f, 0.0f,       1.0f, 1.0f, // top right
            -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,       0.0f, 0.0f,// bottom left
            0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 1.0f,       1.0f, 0.0f//  bottom right
    };

    unsigned int indices[] = {
            0, 3, 2,
            0, 1, 3
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //导入数据到GPU（显存）
    glBufferData(GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices, GL_STATIC_DRAW);

    //设置VertexAttribPointer
    glVertexAttribPointer(0, 3/*vec3*/, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2/*vec2*/, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    //加载并创建纹理
    unsigned int texture;
    //texture unit 0 默认就激活了，所以BindTexture就是绑定到了texture unit 0
    //glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    int width, height, nChannels;
    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/container.jpg").c_str(), &width, &height,
                                    &nChannels, 0);

    if (data) {
        //拷贝图片数据到GPU的纹理中
        glTexImage2D(GL_TEXTURE_2D /**/,
                     0,
                     GL_RGB,
                     width,
                     height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     data);
        //生成多级渐远纹理
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load image data" << std::endl;
    }

    stbi_image_free(data);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//RGBA
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        shader.use();
        //由于texture1默认值就是0了，所以不需要设置
        //glUniform1i(glGetUniformLocation(shader.id, "texture1"), 0); // 手动设置

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//数据类型要对上，unsigned int indices[]

        glfwSwapBuffers(window);
        glfwPollEvents();//等待事件
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texture);


    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}
