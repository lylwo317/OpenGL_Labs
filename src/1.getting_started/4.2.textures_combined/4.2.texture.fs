#version 330 core
out vec4 FragColor;

in vec2 TexCoord; //顶点着色器中的变量

// texture sampler
uniform sampler2D texture1; // 默认值是0

void main()
{
	FragColor = texture(texture1, TexCoord);//传入纹理坐标，得到相应的颜色
}