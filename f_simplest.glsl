#version 330

uniform sampler2D textureMap0; 
uniform sampler2D textureMap1; 
uniform sampler2D textureMap2; 
uniform sampler2D textureMap3; 
uniform sampler2D textureMap4; 
uniform sampler2D textureMap5;
uniform sampler2D textureMap6; 
uniform sampler2D textureMap7; 



out vec4 pixelColor0; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela
out vec4 pixelColor1; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela
out vec4 pixelColor2; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela
out vec4 pixelColor3; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela
out vec4 pixelColor4; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela
out vec4 pixelColor5; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela
out vec4 pixelColor6; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela
out vec4 pixelColor7; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela


in vec2 iTexCoord0;
in vec2 iTexCoord1;
in vec2 iTexCoord2;
in vec2 iTexCoord3;
in vec2 iTexCoord4;
in vec2 iTexCoord5;
in vec2 iTexCoord6;
in vec2 iTexCoord7;

in vec4 ic; 
in vec4 n;
in vec4 l1;
in vec4 l2;
in vec4 v;

void main(void) {

	//Znormalizowane interpolowane wektory
	vec4 ml1 = normalize(l1);
	vec4 ml2 = normalize(l2);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
	//Wektor odbity
	vec4 mr1 = reflect(-ml1, mn);
	vec4 mr2 = reflect(-ml2, mn);

	//Parametry powierzchni
	vec4 kd0 = texture(textureMap0, iTexCoord0);
	vec4 kd1 = texture(textureMap1, iTexCoord1);
	vec4 kd2 = texture(textureMap2, iTexCoord2);
	vec4 kd3 = texture(textureMap3, iTexCoord3);
	vec4 kd4 = texture(textureMap4, iTexCoord4);
	vec4 kd5 = texture(textureMap5, iTexCoord5);
	vec4 kd6 = texture(textureMap6, iTexCoord6);
	vec4 kd7 = texture(textureMap7, iTexCoord7);
	vec4 ks = vec4(1, 1, 1, 1);

	//Obliczenie modelu oœwietlenia
	float nl1 = clamp(dot(mn, ml1), 0, 1);
	float nl2 = clamp(dot(mn, ml2), 0, 1);
	float rv1 = pow(clamp(dot(mr1, mv), 0, 1), 50);
	float rv2 = pow(clamp(dot(mr2, mv), 0, 1), 50);

	pixelColor0= vec4(kd0.rgb * nl1, kd0.a) + vec4(kd0.rgb * nl2, kd0.a) + vec4(ks.rgb*rv2, 0) + vec4(ks.rgb*rv2, 0);
	pixelColor1= vec4(kd1.rgb * nl1, kd1.a) + vec4(kd1.rgb * nl2, kd1.a) + vec4(ks.rgb*rv2, 0) + vec4(ks.rgb*rv2, 0);
	pixelColor2= vec4(kd2.rgb * nl1, kd2.a) + vec4(kd2.rgb * nl2, kd2.a) + vec4(ks.rgb*rv2, 0) + vec4(ks.rgb*rv2, 0);
	pixelColor3= vec4(kd3.rgb * nl1, kd3.a) + vec4(kd3.rgb * nl2, kd3.a) + vec4(ks.rgb*rv2, 0) + vec4(ks.rgb*rv2, 0);
	pixelColor4= vec4(kd4.rgb * nl1, kd4.a) + vec4(kd4.rgb * nl2, kd4.a) + vec4(ks.rgb*rv2, 0) + vec4(ks.rgb*rv2, 0);
	pixelColor5= vec4(kd5.rgb * nl1, kd5.a) + vec4(kd5.rgb * nl2, kd5.a) + vec4(ks.rgb*rv2, 0) + vec4(ks.rgb*rv2, 0);
	pixelColor6= vec4(kd6.rgb * nl1, kd6.a) + vec4(kd6.rgb * nl2, kd6.a) + vec4(ks.rgb*rv2, 0) + vec4(ks.rgb*rv2, 0);
	pixelColor7= vec4(kd7.rgb * nl1, kd7.a) + vec4(kd7.rgb * nl2, kd7.a) + vec4(ks.rgb*rv2, 0) + vec4(ks.rgb*rv2, 0);

}
