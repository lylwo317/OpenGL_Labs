#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
                                 //最低版本要求3.3，核心渲染模式
const char *vertexShaderSource = "#version 330 core\n"
                                 //取vao中index == 15的顶点属性
                                 //in表示输入，vec3 3个一组的向量数据类型，变量名aPos
                                 "layout (location = 15) in vec3 aPos;\n"
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
    //写入着色器源码到vertextShader
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    //编译着色器
    glCompileShader(vertexShader);

    //检查编译结果
    int success; //GL_TRUE or GL_FALSE
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


    //准备VAO，VBO
    float vertices[] = {
            0.0f, 0.5f, 0.0f, // top
            -0.5f, -0.5f, 0.0f, // left
            0.5f, -0.5f, 0.0f, // right
    };

    GLuint VAOs[2]/*vertex array object*/, VBO/*vertex buffer object*/;
    glGenVertexArrays(2, VAOs);//
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAOs[1]);

    //绑定VBO到GL_ARRAY_BUFFER这个target
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //导入数据到GPU（显存）
    glBufferData(GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

    //修改VAO中的顶点属性指针

    // 绑定VBO到VAO中对应的index属性位置，这里是index == 15的属性
    // glVeretexAttribPointer函数，OpenGL获取此时绑定到GL_ARRAY_BUFFER的缓冲区对象然后关联它到
    // glVeretexAttribPointer指定的（最多有16个，这里是index == 15）顶点属性，相当于确定数据来源和数据解析方式。
    
    //a.参数index 表示顶点属性的索引 这个索引即是在顶点着色器中的属性索引，索引从0开始记起。
    //b.参数size 每个属性数据由几个分量组成。例如上面顶点每个属性为3个float组成的，size即为3。分量的个数必须为1,2, 3, 4 
    //c.参数type表示属性分量的数据类型，例如上面的顶点数据为float则填写GL_FLOAT.
    //d.参数normalized 表示是否归一化，当存储整型时，如果设置为GL_TRUE, 那么当被以浮点数形式访问时，有符号整型转换到[-1, 1], 无符号转换到[0, 1]。否则直接转换为float型，而不进行归一化。 
    //e.参数stride表示连续的两个顶点属性之间的间隔，以字节大小计算。当顶点属性紧密排列(tightly packed) 时，可以填0，由OpenGL代替我们计算出该值。 
    //f.参数pointer表示当前绑定到 GL_ARRAY_BUFFER缓冲对象的缓冲区中，顶点属性的第一个分量距离数据的起点的偏移量，以字节为单位计算。 
    glVertexAttribPointer(15, 3 /*vec3*/, GL_FLOAT, GL_FALSE, 0, nullptr); // vao中index == 15的位置指向
    glEnableVertexAttribArray(15);//启用vao中index == 15的属性

    //解绑vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //解绑vao
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//RGBA
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAOs[1]);//指定程序的输入（VAO）
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();//等待事件
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(1, &VBO);

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
