#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 VertexColor;
in vec3 FragPos;

// Uniforms
uniform sampler2D texture1;
uniform vec3 CameraPos;  // Camera position
uniform float FogDensity; // Fog density (controls the rate of fog intensity increase with distance)
uniform vec3 FogColor;    // Fog color
uniform int FogEnabled;

void main()
{
    float gamma = 2.2;
    float alpha = texture(texture1, TexCoord).a;
    vec3 diffuseColor = pow(texture(texture1, TexCoord).rgb, vec3(gamma));

    //FragColor = texture(texture1, TexCoord) * vec4(VertexColor, 1.0);
    vec3 shadedColor = diffuseColor * VertexColor;
    vec3 finalColor = shadedColor;
    //FragColor = vec4(shadedColor, 1.0);

    if(FogEnabled == 1)
    {
        // Calculate the distance from the fragment to the camera
        float distance = length(FragPos - CameraPos);

        // Calculate fog factor using exponential fog equation (exponential decay)
        float fogFactor = 1.0 / exp( (distance * FogDensity) * (distance * FogDensity));
        fogFactor = clamp( fogFactor, 0.0, 1.0 );

        // Blend the fragment color with the fog color based on the fog factor
        finalColor = mix(FogColor, shadedColor, fogFactor);
    }

    // apply gamma correction
    FragColor = vec4(pow(finalColor, vec3(1.0/gamma)), alpha);
}
