#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    sampler2DArray diffuse;
    sampler2DArray normalMap;
    sampler2DArray specularMap;
    float shininess;
    int hasNormalMap;
    int hasSpecularMap;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int enabled;
    int shadowEnabled;
    samplerCube shadowCubeMap;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int enabled;
};

struct SSAO {
    int enabled;
    sampler2D texNoise;
    vec3 samples[64];
    int kernelSize;
    float radius;
    float bias;
};

#define NR_POINT_LIGHTS 8

in vec3 FragPos;
in vec3 Normal;
in vec3 TexCoord;
in mat3 TBN;

uniform vec3 CameraPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform float FogDensity; // Fog density (controls the rate of fog intensity increase with distance)
uniform vec3 FogColor;    // Fog color
uniform int FogEnabled;

uniform float FarPlane;
uniform int ShadowEnabled;
uniform sampler2D depthBufferTexture;

float shadowBias = 0.015;
vec3 corrected;

// function prototypes
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcAmbientLight();


vec3 CalcBumpedNormal()
{
    vec3 normal = texture(material.normalMap, TexCoord).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(TBN * normal);
    return normal;
}
void shade() {
    float gamma = 2.2;
    float alpha = texture(material.diffuse, TexCoord).a;
    corrected = pow(texture(material.diffuse, TexCoord).rgb, vec3(gamma));

    // properties
    vec3 norm = normalize(Normal);
    if(material.hasNormalMap == 1) {
        norm = CalcBumpedNormal();
    }
    vec3 viewDir = normalize(CameraPos - FragPos);

    // phase 1: ambient lighting
    vec3 result = CalcAmbientLight();
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        if(pointLights[i].enabled == 0) continue;
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    // phase 3: spot light
    if(spotLight.enabled == 1) {
        result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    }
    // clamp the output value
    //result = clamp(result, 0.0, 1.0);

    if(FogEnabled == 1)
    {
        // Calculate the distance from the fragment to the camera
        float distance = length(FragPos - CameraPos);

        // Calculate fog factor using exponential fog equation (exponential decay)
        float fogFactor = 1.0 / exp( (distance * FogDensity) * (distance * FogDensity));
        fogFactor = clamp( fogFactor, 0.0, 1.0 );

        // Blend the fragment color with the fog color based on the fog factor
        result = mix(FogColor, result, fogFactor);
    }

    // apply gamma correction
    FragColor = vec4(pow(result, vec3(1.0/gamma)), alpha);
}

void main()
{
    //FragColor = texture(material.diffuse, TexCoord);
    //return;
    vec2 screenCoords = gl_FragCoord.xy / vec2(textureSize(depthBufferTexture, 0));
    float depthValue = texture(depthBufferTexture, screenCoords).r;
    float currentDepth = gl_FragCoord.z - 0.0001;

    if (currentDepth <= depthValue) {
        shade();
    } else {
        discard;
    }
}

vec3 CalcAmbientLight() {
    return material.ambient * corrected;
}


float CalcShadowFactorPointLight(PointLight light, vec3 fragPos) {
    if(ShadowEnabled == 0)
        return 0.0;
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - light.position;
    // use the light to fragment vector to sample from the depth map
    float closestDepth = texture(light.shadowCubeMap, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= FarPlane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float shadow = currentDepth - shadowBias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

float CalcShadowFactorPCFPointLight(PointLight light, vec3 fragPos) {
    if(ShadowEnabled == 0)
        return 0.0;
    float shadow  = 0.0;
    float bias    = shadowBias;
    float samples = 4.0;
    float offset  = 0.1;
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - light.position;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    for (float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for (float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for (float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float closestDepth = texture(light.shadowCubeMap, fragToLight + vec3(x, y, z)).r;
                closestDepth *= FarPlane;// undo mapping [0;1]
                if (currentDepth - bias > closestDepth)
                shadow += 1.0;
            }
        }
    }
    shadow /= (samples * samples * samples);
    return shadow;
}

vec3 sampleOffsetDirections[20] = vec3[]
(
    vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
    vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
    vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
    vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
    vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float CalcShadowFactorPCFOptimizedPointLight(PointLight light, vec3 fragPos) {
    if(ShadowEnabled == 0)
        return 0.0;
    float shadow = 0.0;
    float bias   = shadowBias;
    int samples  = 20;
    float viewDistance = length(CameraPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / FarPlane)) / 25.0;
    //float diskRadius = 0.05;
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - light.position;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(light.shadowCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= FarPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
        shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow;
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    float ShadowFactor = 0.0;
    if(light.shadowEnabled == 1) {
        ShadowFactor = CalcShadowFactorPCFOptimizedPointLight(light, fragPos);
    }

    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * corrected;
    vec3 diffuse = light.diffuse * diff * corrected;
    vec3 specular = light.specular * spec * corrected;
    if(material.hasSpecularMap == 1) {
        // Sample the specular intensity from the specular map
        float specularIntensity = texture(material.specularMap, TexCoord).r;
        specular = specularIntensity * spec * light.specular * corrected;
    }
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + (1.0 - ShadowFactor) * (diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * corrected;
    vec3 diffuse = light.diffuse * diff * corrected;
    vec3 specular = light.specular * spec * corrected;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
