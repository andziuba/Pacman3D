#version 330

// Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

// Atrybuty
in vec4 vertex;  // Wspolrzedne wierzcholka w przestrzeni modelu
in vec4 color;  // Kolor zwi¹zany z wierzcholkiem
in vec4 normal;  // Wektor normalny w przestrzeni modelu
in vec2 texCoord0;
in vec2 texCoord1;
in vec2 texCoord2;
in vec2 texCoord3;
in vec2 texCoord4;
in vec2 texCoord5;
in vec2 texCoord6;
in vec2 texCoord7;

// Zmienne interpolowane
out vec4 ic;
out vec4 l1;
out vec4 l2;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0; 
out vec2 iTexCoord1;
out vec2 iTexCoord2; 
out vec2 iTexCoord3; 
out vec2 iTexCoord4; 
out vec2 iTexCoord5;
out vec2 iTexCoord6;
out vec2 iTexCoord7;

void main(void) {
    // Pozycje zrodel swiatla w przestrzeni swiata
    vec4 lp1 = vec4(5, 0, -10, 1); 
    vec4 lp2 = vec4(-5, 0, -10, 1); 

    // Wektory do zrodel swiatla w przestrzeni oka
    l1 = normalize(V * lp1 - V*M*vertex); 
    l2 = normalize(V * lp2 - V*M*vertex); 

    // Wektor do obserwatora w przestrzeni oka
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); 

    // Normalizacja wektora normalnego w przestrzeni oka
    n = normalize(V * M * normal);
    
    // Przekazywanie wspolrzednych tekstury
    iTexCoord0 = texCoord0;
    iTexCoord1 = texCoord1;
    iTexCoord2 = texCoord2;
    iTexCoord3 = texCoord3;
    iTexCoord4 = texCoord4;
    iTexCoord5 = texCoord5;
    iTexCoord6 = texCoord6;
    iTexCoord7 = texCoord7;

    // Przekazywanie koloru
    ic = color;
    
    // Obliczenie pozycji wierzcholka w przestrzeni ekranu
    gl_Position=P*V*M*vertex;
}
