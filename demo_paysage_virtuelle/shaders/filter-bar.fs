#version 330

uniform sampler2D tex;
uniform vec2 vhs_resolution = vec2(320.0, 240.0);
uniform float time;
in vec2 vsoTexCoord;
out vec4 fragColor;

// Function to generate random numbers
float v2random(vec2 uv) {
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    vec2 uvn = vsoTexCoord;
    vec2 texSize = vec2(textureSize(tex, 0));

    // Divide the image into 10 vertical parts
    float partWidth = 1.0 / 10.0;
    float partIndex = floor(uvn.x / partWidth);
    float partX = partIndex * partWidth;

    // Calculate the number of bars to animate based on time
    float numBarsAnimated = mod(time, 10.0); // This will cycle through 0 to 9 bars over time

    // Apply vertical shift only to the bars that should be animated
    float shiftAmount = 0.0;
    if (partIndex < numBarsAnimated) {
        // Use a sine wave for smooth up and down movement
        shiftAmount = sin(time * 0.5 + partIndex * 0.5) * 0.05; // Adjust 0.5 and 0.05 for speed and amplitude
    }

    // Apply the shift
    vec2 shiftedUV = vec2(uvn.x, uvn.y + shiftAmount);

    // Sample the texture with the shifted UV coordinates
    vec3 col = texture(tex, shiftedUV).rgb;

    fragColor = vec4(col, 1.0);
}
