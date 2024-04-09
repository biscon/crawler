#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

// Uniforms
uniform mat4 model;
uniform sampler2D texture1;
uniform vec3 CameraPos;  // Camera position
uniform float FogDensity; // Fog density (controls the rate of fog intensity increase with distance)
uniform vec3 FogColor;    // Fog color
uniform int FogEnabled;
uniform vec3 LightColor;
uniform vec3 LightPos;

#define NO_LIGHTS 8
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    float constant;
    float linear;
    float quadratic;
    int enabled;
};

uniform Light Lights[NO_LIGHTS];

vec3 calculateLighting(vec3 fragPos, vec3 corrected, Light light) {

    // ambient
    vec3 ambientLight = light.ambient;
    vec3 ambient = ambientLight * corrected;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * corrected;

    // specular
    vec3 viewDir = normalize(CameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specularLight = vec3(1.0);
    vec3 specular = specularLight * spec * corrected;

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic);

    ambient *= attenuation;
    diffuse *= attenuation;

    /*
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 viewDir = normalize(CameraPos - FragPos);
    // diffuse shading
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * corrected;
    vec3 diffuse = light.diffuse * diff * corrected;
    //vec3 specular = light.specular * spec * corrected;
    ambient *= attenuation;
    diffuse *= attenuation;
    //specular *= attenuation;
    */

    return (ambient + diffuse);
}

void main()
{
    float gamma = 2.2;
    vec3 corrected = pow(texture(texture1, TexCoord).rgb, vec3(gamma));
    vec3 result = vec3(0.0);
    for(int i = 0; i < NO_LIGHTS; i++) {
        if(Lights[i].enabled == 0) continue;
        result += calculateLighting(FragPos, corrected, Lights[i]);
    }
    vec3 finalColor = LightColor * result;

    if(FogEnabled == 1)
    {
        // Calculate the distance from the fragment to the camera
        float distance = length(FragPos - CameraPos);

        // Calculate fog factor using exponential fog equation (exponential decay)
        float fogFactor = 1.0 / exp( (distance * FogDensity) * (distance * FogDensity));
        fogFactor = clamp( fogFactor, 0.0, 1.0 );

        // Blend the fragment color with the fog color based on the fog factor
        finalColor = mix(FogColor, finalColor, fogFactor);
    }

    // apply gamma correction
    FragColor = vec4(pow(finalColor, vec3(1.0/gamma)), 1.0);
}
