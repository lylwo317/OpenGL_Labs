#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main(){\n"
                                 "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                 "out vec4 fragmentColor;\n"
                                 "void main(){\n"
                                 "  fragmentColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                 "}\0";
const char *fragmentYellowShaderSource = "#version 330 core\n"
                                   "out vec4 fragmentColor;\n"
                                   "void main(){\n"
                                   "   fragmentColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
                                   "}\0";

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

    //vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    //检查编译结果
    int success;
    char infoLog[512];
    //获取编译状态
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE){
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Error shader vertex compilation failed\n" << infoLog << std::endl;
    }

    //fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    //检查编译结果

    //获取编译状态
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE){
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Error shader fragment compilation failed\n" << infoLog << std::endl;
    }

    //fragment yellow shader
    int fragmentYellowShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentYellowShader, 1, &fragmentYellowShaderSource, nullptr);
    glCompileShader(fragmentYellowShader);

    //检查编译结果

    //获取编译状态
    glGetShaderiv(fragmentYellowShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE){
        glGetShaderInfoLog(fragmentYellowShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Error shader fragment compilation failed\n" << infoLog << std::endl;
    }

    //link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE){
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Error shader program link failed\n" << infoLog << std::endl;
    }

    //link yellow shaders
    int shaderYellowProgram = glCreateProgram();
    glAttachShader(shaderYellowProgram, vertexShader);
    glAttachShader(shaderYellowProgram, fragmentYellowShader);
    glLinkProgram(shaderYellowProgram);

    glGetProgramiv(shaderYellowProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE){
        glGetProgramInfoLog(shaderYellowProgram, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Error shader program link failed\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(fragmentYellowShader);

//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    //准备VAO，VBO
    float firstVertices[] = {
            //first triangle
            -0.9f, -0.5f, 0.0f,//left
            0.0f, -0.5f, 0.0f,//right
            -0.45, 0.5f, 0.0f//top
    };

    float secondVertices[] = {
            //second triangle
            0.0f, -0.5f, 0.0f, // left
            0.9f, -0.5f, 0.0f, // right
            0.45f, 0.5f, 0.0f
    };

    GLuint VAOs[2], VBOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);

    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    //导入数据到GPU（显存）
    glBufferData(GL_ARRAY_BUFFER, sizeof (firstVertices), firstVertices, GL_STATIC_DRAW);

    //绑定VBO
    glVertexAttribPointer(0, 3/*vec3*/, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOs[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    //导入数据到GPU（显存）
    glBufferData(GL_ARRAY_BUFFER, sizeof (secondVertices), secondVertices, GL_STATIC_DRAW);

    //绑定VBO
    glVertexAttribPointer(0, 3/*vec3*/, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);//解绑VBO

    glBindVertexArray(0);//解绑VAO

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//RGBA
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUseProgram(shaderYellowProgram);
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();//等待事件
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(shaderYellowProgram);

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
