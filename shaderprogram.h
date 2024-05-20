/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOśCI HANDLOWEJ albo PRZYDATNOśCI DO OKREśLONYCH
ZASTOSOWAń.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/


#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <GL/glew.h>
#include "stdio.h"

class ShaderProgram {
private:
	GLuint shaderProgram;  // Uchwyt reprezentujący program cieniujacy
	GLuint vertexShader;   // Uchwyt reprezentujący vertex shader
	GLuint geometryShader; // Uchwyt reprezentujący geometry shader
	GLuint fragmentShader; // Uchwyt reprezentujący fragment shader
	char* readFile(const char* fileName);  // Metoda wczytująca plik tekstowy do tablicy znaków
	GLuint loadShader(GLenum shaderType, const char* fileName);  // Metoda wczytuje i kompiluje shader, a następnie zwraca jego uchwyt
public:
	ShaderProgram(const char* vertexShaderFile, const char* geometryShaderFile, const char* fragmentShaderFile);
	~ShaderProgram();
	void use();  // Włącza wykorzystywanie programu cieniującego
	GLuint u(const char* variableName);  // Pobiera numer slotu związanego z daną zmienną jednorodną
	GLuint a(const char* variableName);  // Pobiera numer slotu związanego z danym atrybutem
};

#endif
