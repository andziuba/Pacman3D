#version 330

uniform sampler2D textureMap0;
uniform vec4 lp1; // Pozycja pierwszego Ÿród³a œwiat³a
uniform vec4 lp2; // Pozycja drugiego Ÿród³a œwiat³a
uniform vec4 ks;  // Kolor odbiæ

in vec4 v_normal;
in vec2 v_texCoord;
in vec4 v_position;

out vec4 pixelColor;

void main() {
    vec4 kd = texture(textureMap0, v_texCoord);
    
    // Obliczenia dla pierwszego Ÿród³a œwiat³a
    vec4 l1 = normalize(lp1 - v_position);
    vec4 n = normalize(v_normal);
    vec4 v = normalize(vec4(0, 0, 0, 1) - v_position);
    vec4 r1 = reflect(-l1, n);
    
    float nl1 = clamp(dot(n, l1), 0.0, 1.0);
    float rv1 = pow(clamp(dot(r1, v), 0.0, 1.0), 50.0); // Mo¿na zmieniæ wyk³adnik
    
    // Obliczenia dla drugiego Ÿród³a œwiat³a
    vec4 l2 = normalize(lp2 - v_position);
    vec4 r2 = reflect(-l2, n);
    
    float nl2 = clamp(dot(n, l2), 0.0, 1.0);
    float rv2 = pow(clamp(dot(r2, v), 0.0, 1.0), 50.0); // Mo¿na zmieniæ wyk³adnik
    
    // Sumowanie wk³adów z obu Ÿróde³ œwiat³a
    vec4 diffuse = kd * (nl1 + nl2);
    vec4 specular = ks * (rv1 + rv2);
    
    pixelColor = diffuse + specular;
}
