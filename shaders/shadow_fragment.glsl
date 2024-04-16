#version 330 core
out float LightToPixelDistance;

in vec3 FragPos;
uniform vec3 LightPos;
uniform float FarPlane;

void main()
{
    LightToPixelDistance = length(FragPos - LightPos) / FarPlane;
}