#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;

out vec2 TexCoord;
out vec3 VertexColor;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    VertexColor = aColor;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
