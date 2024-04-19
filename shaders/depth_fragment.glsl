#version 330 core

out float FragDepth;

void main() {
    // Output depth value from vertex shader
    FragDepth = gl_FragCoord.z;
}
