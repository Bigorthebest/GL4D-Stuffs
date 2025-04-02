#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screen_texture;  // L'écran (passé par OpenGL)
uniform float time;  // Permet de faire évoluer l'effet

void main() {
    // Coordonnée de l'écran
    vec2 uv = TexCoords;
    
    // Déformation VHS en fonction du temps (exemple de distorsion)
    float offset = sin(uv.y * 50.0 + time) * 0.005;
    uv.x += offset;
    
    // Lire la couleur depuis l'écran
    vec3 color = texture(screen_texture, uv).rgb;
    
    // Bruit aléatoire
    float noise = fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
    color += noise * 0.02; // Ajoute un effet de grain
    
    FragColor = vec4(color, 1.0);
}
