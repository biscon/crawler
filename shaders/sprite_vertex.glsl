#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 CameraPos;
uniform vec3 CameraRight;
uniform vec3 CameraUp;
uniform vec3 SpritePos;
uniform vec3 SpriteNormal;
uniform int billboarding; // 0 = none, 1 = spherical, 2 = cylindrical

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    // calculate normal that faces the camera
    //Normal = normalize(SpritePos - CameraPos);
    Normal = normalize(CameraPos - SpritePos);


    vec3 diff = SpritePos - aPos;
    vec3 vertexPosition_worldspace;

    if(billboarding == 1) {
        vertexPosition_worldspace =
        SpritePos
        + CameraRight * diff.x
        - CameraUp * diff.y;
    } else if(billboarding == 2){
        // Compute the position relative to the sprite, keeping y constant
        vertexPosition_worldspace =
        SpritePos
        + CameraRight * diff.x
        - vec3(0.0, diff.y, 0.0);
    } else {
        vertexPosition_worldspace = aPos;
    }

    gl_Position = projection * view * vec4(vertexPosition_worldspace, 1.0);
}
