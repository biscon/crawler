#version 330 core
// read from vertex buffer
layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texcoord;

// pass to fragment shader
out vec4 Color;
out vec2 Texcoord;

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float engineTime = 0;
uniform int useWind = 0;

// Wind settings.
uniform float windSpeed = 1.0;
uniform float windMinStrength = 0.1; // hint_range(0.0, 1.0);
uniform float windMaxStrength = 0.3; // hint_range(0.0, 1.0);
uniform float windStrengthScale = 100.0;
uniform float windInterval = 3.5;
uniform float windDetail = 1.0;
uniform float windDistortion = 0; // hint_range(0.0, 1.0);
uniform float windHeightOffset = 0.0;

float getWind(vec2 vertex, vec2 uv, float timer) {
    vec2 pos = mix(vec2(1.0), vertex, windDistortion).xy;
    float time = timer * windSpeed + pos.x * pos.y;
    float diff = pow(windMaxStrength - windMinStrength, 2.0);
    float strength = clamp(windMinStrength + diff + sin(time / windInterval) * diff, windMinStrength, windMaxStrength) * windStrengthScale;
    float wind = (sin(time) + cos(time * windDetail)) * strength * max(0.0, (1.0-uv.y) - windHeightOffset);
    return wind;
}

void main()
{
    Color = color;
    Texcoord = texcoord;
    vec2 pos = vec2(position.xy);
    if(useWind == 1) {
        pos.x += getWind(position.xy, texcoord, engineTime);
    }
    gl_Position = proj * view * model * vec4(pos, 0.0, 1.0);
}
