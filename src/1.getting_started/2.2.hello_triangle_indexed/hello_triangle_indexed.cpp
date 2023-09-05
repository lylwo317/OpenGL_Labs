#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"//aPos = VAO.attribute[0]
                                 "void main(){\n"
                                 "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                 "out vec4 fragmentColor;\n"
                                 "void main(){\n"
                                 "  fragmentColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
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
    glShaderSource(vertexShader, 1/*也就数组的大小*/, &vertexShaderSource, nullptr);
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

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    //准备VAO，VBO, EBO
    float vertices[] = {
            -0.5f, 0.5f, 0.0f, // top left
            0.5f, 0.5f, 0.0f, // top right
            -0.5f, -0.5f, 0.0f, // bottom left
            0.5f, -0.5f, 0.0f, // bottom right
    };

    unsigned int indices[] = {
            0,1,3,
            0,2,3
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
    //导入数据到GPU（显存）
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices, GL_STATIC_DRAW);

    //绑定VBO
    glVertexAttribPointer(0, 3/*vec3*/, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);//解绑VBO

    //不能在VAO未解绑的时候，解绑EBO，因为EBO是存储在VAO上的。解绑操作会导致VAO与EBO解除绑定
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//解绑EBO

    glBindVertexArray(0);//解绑VAO

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//解绑EBO

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//RGBA
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);//由于VAO绑定了EBO，所以绑定VAO的时候，也会绑定EBO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//使用EBO来绘制

        glfwSwapBuffers(window);
        glfwPollEvents();//等待事件
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

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
