/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#include "shaderprogram.h"

// Procedura wczytuje plik do tablicy znakow.
char* ShaderProgram::readFile(const char* fileName) {
	int filesize;
	FILE* plik;
	char* result;

#pragma warning(suppress : 4996)  // Wylaczenie bledu w Visual Studio wynikające z nietrzymania się standardow przez Microsoft
	plik = fopen(fileName, "rb");
	if (plik != NULL) {
		fseek(plik, 0, SEEK_END);
		filesize = ftell(plik);
		fseek(plik, 0, SEEK_SET);
		result = new char[filesize + 1];
#pragma warning(suppress : 6386)  // Wylaczenie bledu w Visual Studio wynikającego z blednej analizy statycznej kodu
		int readsize = fread(result, 1, filesize, plik);
		result[filesize] = 0;
		fclose(plik);

		return result;
	}

	return NULL;

}

// Metoda wczytuje i kompiluje shader, a nastepnie zwraca jego uchwyt
GLuint ShaderProgram::loadShader(GLenum shaderType, const char* fileName) {
	// Wygeneruj uchwyt na shader
	GLuint shader = glCreateShader(shaderType);  // shaderType to GL_VERTEX_SHADER, GL_GEOMETRY_SHADER lub GL_FRAGMENT_SHADER
	// Wczytaj plik ze zrodlem shadera do tablicy znakow
	const GLchar* shaderSource = readFile(fileName);
	// Powiaz zrodlo z uchwytem shadera
	glShaderSource(shader, 1, &shaderSource, NULL);
	// Skompiluj zrodlo
	glCompileShader(shader);
	// Usun zrodlo shadera z pamieci (nie bedzie juz potrzebne)
	delete[]shaderSource;

	// Pobierz log bledow kompilacji i wyswietl
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 1) {
		infoLog = new char[infologLength];
		glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		delete[]infoLog;
	}

	// Zwroc uchwyt wygenerowanego shadera
	return shader;
}

ShaderProgram::ShaderProgram(const char* vertexShaderFile, const char* geometryShaderFile, const char* fragmentShaderFile) {
	// Wczytaj vertex shader
	printf("Loading vertex shader...\n");
	vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderFile);

	// Wczytaj geometry shader
	if (geometryShaderFile != NULL) {
		printf("Loading geometry shader...\n");
		geometryShader = loadShader(GL_GEOMETRY_SHADER, geometryShaderFile);
	}
	else {
		geometryShader = 0;
	}

	// Wczytaj fragment shader
	printf("Loading fragment shader...\n");
	fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderFile);

	// Wygeneruj uchwyt programu cieniujacego
	shaderProgram = glCreateProgram();

	// Podlacz do niego shadery i zlinkuj program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	if (geometryShaderFile != NULL) glAttachShader(shaderProgram, geometryShader);
	glLinkProgram(shaderProgram);

	// Pobierz log bledow linkowania i wyswietl
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 1) {
		infoLog = new char[infologLength];
		glGetProgramInfoLog(shaderProgram, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		delete[]infoLog;
	}

	printf("Shader program created \n");
}

ShaderProgram::~ShaderProgram() {
	// Odlacz shadery od programu
	glDetachShader(shaderProgram, vertexShader);
	if (geometryShader != 0) glDetachShader(shaderProgram, geometryShader);
	glDetachShader(shaderProgram, fragmentShader);

	// Wykasuj shadery
	glDeleteShader(vertexShader);
	if (geometryShader != 0) glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);

	// Wykasuj program
	glDeleteProgram(shaderProgram);
}

// Wlacz uzywanie programu cieniujacego reprezentowanego przez aktualny obiekt
void ShaderProgram::use() {
	glUseProgram(shaderProgram);
}

// Pobierz numer slotu odpowiadającego zmiennej jednorodnej o nazwie variableName
GLuint ShaderProgram::u(const char* variableName) {
	return glGetUniformLocation(shaderProgram, variableName);
}

// Pobierz numer slotu odpowiadajacego atrybutowi o nazwie variableName
GLuint ShaderProgram::a(const char* variableName) {
	return glGetAttribLocation(shaderProgram, variableName);
}
