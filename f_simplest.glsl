#version 330

uniform sampler2D textureMap0;
uniform vec4 lp1; // Pozycja pierwszego �r�d�a �wiat�a
uniform vec4 lp2; // Pozycja drugiego �r�d�a �wiat�a
uniform vec4 ks;  // Kolor odbi�

in vec4 v_normal;
in vec2 v_texCoord;
in vec4 v_position;

out vec4 pixelColor;

void main() {
    vec4 kd = texture(textureMap0, v_texCoord);
    
    // Obliczenia dla pierwszego �r�d�a �wiat�a
    vec4 l1 = normalize(lp1 - v_position); // Wektor od piksela do pierwszego �r�d�a �wiat�a
    vec4 n = normalize(v_normal); // Normalna powierzchni
    vec4 v = normalize(vec4(0, 0, 0, 1) - v_position); // Widok z kamery
    vec4 r1 = reflect(-l1, n); // Wektor odbicia dla pierwszego �r�d�a �wiat�a
    
    // Obliczenia dla drugiego �r�d�a �wiat�a
    vec4 l2 = normalize(lp2 - v_position); // Wektor od piksela do drugiego �r�d�a �wiat�a
    vec4 r2 = reflect(-l2, n); // Wektor odbicia dla drugiego �r�d�a �wiat�a
    
    float nl1 = clamp(dot(n, l1), 0.0, 1.5); // Sk�adowa nat�enia �wiat�a diffusowego dla pierwszego �r�d�a
float rv1 = pow(clamp(dot(r1, v), 0.0, 1.0), 50.0); // Sk�adowa nat�enia �wiat�a specular dla pierwszego �r�d�a

float nl2 = clamp(dot(n, l2), 0.0, 1.5); // Sk�adowa nat�enia �wiat�a diffusowego dla drugiego �r�d�a
float rv2 = pow(clamp(dot(r2, v), 0.0, 1.0), 50.0); // Sk�adowa nat�enia �wiat�a specular dla drugiego �r�d�a

    
    // Cel-shading: Stylizowanie kolor�w na podstawie nat�enia �wiat�a
    float intensity = (nl1 + nl2) * 0.5; // �rednie nat�enie �wiat�a z obu �r�de�
    vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0); // Bia�y kolor �wiat�a
    
    if (intensity > 0.95)
        pixelColor = lightColor * kd;
    else if (intensity > 0.5)
        pixelColor = lightColor * kd * 0.8;
    else if (intensity > 0.25)
        pixelColor = lightColor * kd * 0.6;
    else
        pixelColor = lightColor * kd * 0.4;
}
