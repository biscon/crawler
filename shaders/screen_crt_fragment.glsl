#version 330 core
#define CURVATURE 15
#define PI 3.14159265359

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D lightTexture;
uniform int useCrt;
uniform int useLight;

//vec2 screenResolution = vec2(1920, 1080);
//vec2 screenResolution = vec2(2560, 1440);
vec3 brightness = vec3(1.15, 1.15, 1.15);

// the blur kernel
const int kernelSize = 44;
float kernel[kernelSize] = float[](0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067, 0.00002292, 0.00078634, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00038771, 0.01330373, 0.11098164, 0.22508352, 0.11098164, 0.01330373, 0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00002292, 0.00078634, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067);

vec4 blur(sampler2D image, vec2 texelCoords, float blurAmount)
{
    // get the resolution of the image
    vec2 resolution = vec2(textureSize(image, 0));

    // the blur offset
    vec2 offset = blurAmount / resolution;

    // the color and alpha accumulators
    vec4 color = vec4(0.0);
    float alpha = 0.0;

    // apply the blur
    for (int i = 0; i < kernelSize; ++i)
    {
        // get the texel at the current offset
        vec4 texel = texture(image, texelCoords + offset * (float(i) - float(kernelSize / 2)));

        // accumulate the color and alpha
        color += texel * kernel[i];
        alpha += texel.a * kernel[i];
    }

    // return the blurred color
    return vec4(color.rgb, alpha);
}

vec2 curveRemapUV(vec2 uv)
{
    // as we near the edge of our screen apply greater distortion using a cubic function
    uv = uv * 2.0 - 1.0;
    vec2 offset = abs(uv.yx) / vec2(CURVATURE, CURVATURE);
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;
    return uv;
}

vec4 scanLineIntensity(float uv, float resolution, float opacity)
{
    float intensity = sin(uv * resolution * PI * 2.0);
    intensity = ((0.5 * intensity) + 0.5) * 0.9 + 0.1;
    return vec4(vec3(pow(intensity, opacity)), 1.0);
}


vec4 blendMultiply(vec4 base, vec4 blend) {
    return base*blend;
}

/*
vec3 blendMultiply(vec3 base, vec3 blend) {
    return base*blend;
}
*/


vec4 blendMultiplyOpacity(vec4 base, vec4 blend, float opacity) {
    return (blendMultiply(base, blend) * opacity + base * (1.0 - opacity));
}

const float blurAmount = 0.20f;

void main(void)
{
    if(useCrt == 1) {
        vec2 remappedUV = curveRemapUV(vec2(TexCoords.x, TexCoords.y));

        vec4 screen = texture(screenTexture, remappedUV);
        vec4 baseColor = texture(screenTexture, remappedUV);

        if(useLight == 1) {
            vec4 light = blur(lightTexture, remappedUV, blurAmount);
            baseColor = blendMultiply(screen, light);
        }

        //vec4 baseColor = sampleBlurred(screenTexture, remappedUV);
        vec2 screenResolution = vec2(textureSize(screenTexture, 0));
        baseColor *= scanLineIntensity(remappedUV.x, screenResolution.y, 0.01);
        baseColor *= scanLineIntensity(remappedUV.y, screenResolution.x, 0.05);
        baseColor *= vec4(vec3(brightness), 1.0);

        vec2 uv = remappedUV.xy / vec2(1, 1);
        uv *=  1.0 - uv.yx;//vec2(1.0)- uv.yx; -> 1.-u.yx; Thanks FabriceNeyret !
        float vig = uv.x*uv.y * 20.0;// multiply with sth for intensity
        vig = pow(vig, 0.15);// change pow for modifying the extend of the  vignette
        baseColor *= vec4(vig, vig, vig, 1.0);


        if (remappedUV.x < 0.0 || remappedUV.y < 0.0 || remappedUV.x > 1.0 || remappedUV.y > 1.0){
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        } else {
            FragColor = baseColor;
        }
    } else {
        if(useLight == 1) {
            vec4 screen = texture(screenTexture, TexCoords);
            //vec4 screen = blur(screenTexture, TexCoords, blurAmount);

            //vec4 light = blur(lightTexture, TexCoords, blurAmount);
            vec4 light = texture(lightTexture, TexCoords);

            //vec4 blend = sampleBlurred(lightTexture, TexCoords);
            FragColor = blendMultiply(screen, light);
        } else {
            FragColor = texture(screenTexture, TexCoords);
        }
    }
}
