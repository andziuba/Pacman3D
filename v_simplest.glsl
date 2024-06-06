#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

//Atrybuty
in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
in vec4 color; //kolor zwi¹zany z wierzcho³kiem
in vec4 normal; //wektor normalny w przestrzeni modelu
in vec2 texCoord0;

//Zmienne interpolowane
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
    vec4 lp1 = vec4(5, 0, -10, 1); //pozcyja œwiat³a, przestrzeñ œwiat³a
    vec4 lp2 = vec4(-5, 0, -10, 1); //pozcyja œwiat³a, przestrzeñ œwiat³a
    l1 = normalize(V * lp1 - V*M*vertex); //wektor do œwiat³a w przestrzeni oka
    l2 = normalize(V * lp2 - V*M*vertex); //wektor do œwiat³a w przestrzeni oka
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //wektor do obserwatora w przestrzeni oka
    n = normalize(V * M * normal); //wektor normalny w przestrzeni oka
    
    iTexCoord0 = texCoord0;

    ic = color;
    
    gl_Position=P*V*M*vertex;
}
