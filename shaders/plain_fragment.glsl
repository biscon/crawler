#version 330 core
// passed from vertex shader
in vec4 Color;
in vec2 Texcoord;

// resulting fragment color
out vec4 outColor;

// texture
uniform sampler2D tex;
uniform sampler2D lutTexture;
uniform sampler2D lutTexture2;
uniform int use_texture;
uniform int is_font;
uniform int useLut;
uniform int matte;
uniform vec3 matteColor;
uniform float lutMix;

const float offset = 1.0 / 10000.0;

vec4 sampleBlurred(sampler2D tex, vec2 uv)
{
    vec2 offsets[9] = vec2[](
    vec2(-offset,  offset), // top-left
    vec2( 0.0f,    offset), // top-center
    vec2( offset,  offset), // top-right
    vec2(-offset,  0.0f),   // center-left
    vec2( 0.0f,    0.0f),   // center-center
    vec2( offset,  0.0f),   // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f,   -offset), // bottom-center
    vec2( offset, -offset)  // bottom-right
    );

    // sharpen kernel
    /*
    float kernel[9] = float[](
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
    );
    */
    // blur kernel
    float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16
    );

    vec4 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec4(texture(tex, uv.st + offsets[i]));
    }
    vec4 col = vec4(0.0);
    for(int i = 0; i < 9; i++)
    col += sampleTex[i] * kernel[i];


    return col;
}

vec2 uv_cstantos( vec2 uv, vec2 res ) {
    vec2 pixels = uv * res;

    // Updated to the final article
    vec2 alpha = 0.7 * fwidth(pixels);
    vec2 pixels_fract = fract(pixels);
    vec2 pixels_diff = clamp( .5 / alpha * pixels_fract, 0, .5 ) +
    clamp( .5 / alpha * (pixels_fract - 1) + .5, 0, .5 );
    pixels = floor(pixels) + pixels_diff;
    return pixels / res;
}

vec2 uv_klems( vec2 uv, ivec2 texture_size ) {

    vec2 pixels = uv * texture_size + 0.5;

    // tweak fractional value of the texture coordinate
    vec2 fl = floor(pixels);
    vec2 fr = fract(pixels);
    vec2 aa = fwidth(pixels) * 0.75;

    fr = smoothstep( vec2(0.5) - aa, vec2(0.5) + aa, fr);

    return (fl + fr - 0.5) / texture_size;
}

vec2 uv_iq( vec2 uv, ivec2 texture_size ) {
    vec2 pixel = uv * texture_size;

    vec2 seam = floor(pixel + 0.5);
    vec2 dudv = fwidth(pixel);
    pixel = seam + clamp( (pixel - seam) / dudv, -0.5, 0.5);

    return pixel / texture_size;
}

vec2 uv_aa_linear( vec2 uv, vec2 res, float width ) {
    vec2 pixels = uv * res;

    vec2 pixels_floor = floor(pixels + 0.5);
    vec2 pixels_fract = clamp( (pixels - pixels_floor) / fwidth(pixels) / width, -0.5, 0.5);

    return (pixels_floor + pixels_fract) / res;
}

vec2 uv_aa_smoothstep( vec2 uv, vec2 res, float width ) {
    vec2 pixels = uv * res;

    vec2 pixels_floor = floor(pixels + 0.5);
    vec2 pixels_fract = fract(pixels + 0.5);
    vec2 pixels_aa = fwidth(pixels) * width * 0.5;
    pixels_fract = smoothstep( vec2(0.5) - pixels_aa, vec2(0.5) + pixels_aa, pixels_fract );

    return (pixels_floor + pixels_fract - 0.5) / res;
}

vec2 uv_nearest( vec2 uv, ivec2 texture_size ) {
    vec2 pixel = uv * texture_size;
    pixel = floor(pixel) + .5;
    return pixel / texture_size;
}

vec4 lookup(vec4 textureColor, sampler2D lookupTable) {
    #ifndef LUT_NO_CLAMP
    textureColor = clamp(textureColor, 0.0, 1.0);
    #endif

    mediump float blueColor = textureColor.b * 63.0;

    mediump vec2 quad1;
    quad1.y = floor(floor(blueColor) / 8.0);
    quad1.x = floor(blueColor) - (quad1.y * 8.0);

    mediump vec2 quad2;
    quad2.y = floor(ceil(blueColor) / 8.0);
    quad2.x = ceil(blueColor) - (quad2.y * 8.0);

    highp vec2 texPos1;
    texPos1.x = (quad1.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);
    texPos1.y = (quad1.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);

    #ifdef LUT_FLIP_Y
    texPos1.y = 1.0-texPos1.y;
    #endif

    highp vec2 texPos2;
    texPos2.x = (quad2.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.r);
    texPos2.y = (quad2.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor.g);

    #ifdef LUT_FLIP_Y
    texPos2.y = 1.0-texPos2.y;
    #endif

    lowp vec4 newColor1 = texture(lookupTable, texPos1);
    lowp vec4 newColor2 = texture(lookupTable, texPos2);

    lowp vec4 newColor = mix(newColor1, newColor2, fract(blueColor));
    return newColor;
}

void main()
{
    if(use_texture == 1) {
        if(is_font == 0) {
            ivec2 texSize = textureSize(tex, 0);

            if(matte == 1) {
                //vec2 texCoord = uv_aa_linear(Texcoord, texSize, 1.5);
                vec2 texCoord = uv_nearest(Texcoord, texSize);
                vec4 baseColor = texture(tex, texCoord) * Color;
                float alpha = baseColor.a;
                // draw all non transparent pixels in matteColor
                if (alpha > 0) {
                    outColor.rgb = matteColor;
                } else {
                    discard;
                }

                // restore alpha
                outColor.a = alpha;
            } else {
                //vec2 texCoord = uv_aa_linear(Texcoord, texSize, 1.5);
                //vec2 texCoord = uv_nearest(Texcoord, texSize);
                //vec2 texCoord = uv_cstantos(Texcoord, texSize);
                vec2 texCoord = uv_iq(Texcoord, texSize);
                //vec2 texCoord = uv_klems(Texcoord, texSize);
                //vec2 texCoord = uv_aa_smoothstep(Texcoord, texSize, 1.5);

                //outColor = texture(tex, texCoord) * Color;
                vec4 baseColor = texture(tex, texCoord) * Color;

                if(useLut == 1) {
                    float alpha = baseColor.a;
                    vec4 firstColor = lookup(baseColor, lutTexture);
                    vec4 secondColor = lookup(baseColor, lutTexture2);

                    outColor = mix(firstColor, secondColor, lutMix);
                    // restore alpha
                    outColor.a = alpha;

                } else {
                    outColor = baseColor;
                }
            }


            //outColor = texture(tex, Texcoord) * Color;
            //outColor = sampleBlurred(tex, Texcoord) * Color;
            //outColor = texture(tex, Texcoord) * Color;
        } else {
            ivec2 texSize = textureSize(tex, 0);
            vec2 texCoord = uv_aa_linear(Texcoord, texSize, 1);
            //vec2 texCoord = uv_nearest(Texcoord, texSize);
            float r = texture(tex, texCoord).r;
            outColor = Color * vec4(r, r, r, r);
        }

    } else {
        outColor = Color;
    }

}
