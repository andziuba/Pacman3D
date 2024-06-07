#include "game_logic.h"
#include <irrKlang.h>
using namespace irrklang;

ISoundEngine* soundEngine;

// Pacman
glm::vec3 pacmanPosition = pacmanPositionInitial;
const float pacmanSpeed = 2.0f;
float pacmanSpeed_x = 0.0f;
float pacmanSpeed_y = 0.0f;
float detectionDistance = 0.18f;
float pointDetectionDistance = 0.2;
Direction lastPacmanDirection = RIGHT; 
Direction desiredPacmanDirection = RIGHT; 

// Duszki
glm::vec3 ghostPositionRed = ghostPositionRedInitial;
glm::vec3 ghostPositionBlue = ghostPositionBlueInitial;
glm::vec3 ghostPositionPink = ghostPositionPinkInitial;
glm::vec3 ghostPositionOrange = ghostPositionOrangeInitial;
const float ghostSpeed = 2.0f;
float ghostDetectionDistance = 0.3f;
GhostState ghostStates[4] = { NORMAL, PREAPARING, PREAPARING, PREAPARING }; // red, blue, pink, orange

// Zatrzymanie ruchu Pacmana
void stopPacman() {
    pacmanSpeed_x = 0.0f;
    pacmanSpeed_y = 0.0f;
}

// Resetuje pozycje duszkow
void resetGhosts() {
    ghostPositionRed = ghostPositionRedInitial;
    ghostPositionBlue = ghostPositionBlueInitial;
    ghostPositionPink = ghostPositionPinkInitial;
    ghostPositionOrange = ghostPositionOrangeInitial;

    ghostStates[0] = NORMAL;  // red
    ghostStates[1] = PREAPARING;
    ghostStates[2] = PREAPARING;
    ghostStates[3] = PREAPARING;
}

void resetGame(bool& gameStarted, bool& gameOver) {
    // Komunikat o koncu gry
    system("cls");
    printf("Game over\n");
    gameStarted = false;
    gameOver = true;

    // Dzwiek przegranej
    soundEngine->stopAllSounds();
    soundEngine->play2D("resources/audio/pacman_death.wav", false);  

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Komunikat o rozpoczeciu nowej gry
    printf("Press space to start");
    soundEngine->stopAllSounds();
    soundEngine->play2D("resources/audio/pacman_beginning.wav", true);  // Dzwiek poczatkowy

    // Przywrocenie pozycji
    pacmanPosition = pacmanPositionInitial;
    stopPacman();
    resetGhosts();
    lastPacmanDirection = RIGHT;
    pointPositions = pointPositionsCopy;
}

void handlePacmanControl(int key, int action) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_W: 
            desiredPacmanDirection = UP;
            break;
        case GLFW_KEY_S: 
            desiredPacmanDirection = DOWN;
            break;
        case GLFW_KEY_A: 
            desiredPacmanDirection = LEFT;
            break;
        case GLFW_KEY_D: 
            desiredPacmanDirection = RIGHT;
            break;
        }
    }
}

// Sprawdza czy punkt p lezy wewnatrz trojkata okreslonego przez punkty a, b i c
// Jesli tak, zwraca wspolrzedne barycentryczne (u, v) punktu p w trojkacie 
bool isPointInTriangle(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c, float& u, float& v) {
    // Wektory od punktow
    glm::vec2 v0 = b - a;
    glm::vec2 v1 = c - a;
    glm::vec2 v2 = p - a;

    // Obliczenie iloczynow skalarnych
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);

    // Obliczenie wyznacznika
    float denom = d00 * d11 - d01 * d01;

    if (denom == 0.0f) {
        return false; // trojkat zdegenerowany
    }

    // Obliczenie wspolrzednych barycentrycznych
    u = (d11 * d20 - d01 * d21) / denom;
    v = (d00 * d21 - d01 * d20) / denom;
    float w = 1.0f - u - v;

    // Sprawdzenie czy punkt lezy wewnatrz trojkata
    return (u >= 0.0f) && (v >= 0.0f) && (w >= 0.0f);
}

bool checkWallCollision(const glm::vec3& position, const std::vector<float>& mazeVertices) {
    // Przechodzenie przez wierzcholki labiryntu w krokach po 9 (3 wierzcholki - 9 liczb)
    for (size_t i = 0; i < mazeVertices.size(); i += 9) {
        // Pobranie trzech wierzcholkow trojkata
        glm::vec3 v0(mazeVertices[i], mazeVertices[i + 1], mazeVertices[i + 2]);
        glm::vec3 v1(mazeVertices[i + 3], mazeVertices[i + 4], mazeVertices[i + 5]);
        glm::vec3 v2(mazeVertices[i + 6], mazeVertices[i + 7], mazeVertices[i + 8]);

        // Sprawdzenie czy punkt znajduje siê wewnatrz trojkata
        float u, v;
        if (isPointInTriangle(glm::vec2(position.x, position.z), glm::vec2(v0.x, v0.z), glm::vec2(v1.x, v1.z), glm::vec2(v2.x, v2.z), u, v)) {
            return true; // Kolizja ze sciana
        }
    }
    return false; // Brak kolizji
}

// Sprawdza, czy dwie pozycje koliduja, porownujac odleg³osc euklidesowa z collisionDistance
bool detectCollision(glm::vec3 position1, glm::vec3 position2, float collisionDistance) {
    float distance = glm::distance(position1, position2);
    return distance < collisionDistance;
}

// Sprawdza kolizje pomiedzy Pacmanem i ktorymkolwiek z duszkow
bool checkPacmanGhostCollision() {
    if (detectCollision(pacmanPosition, ghostPositionRed, detectionDistance)) {
        return true;
    }
    if (detectCollision(pacmanPosition, ghostPositionBlue, detectionDistance)) {
        return true;
    }
    if (detectCollision(pacmanPosition, ghostPositionPink, detectionDistance)) {
        return true;
    }
    if (detectCollision(pacmanPosition, ghostPositionOrange, detectionDistance)) {
        return true;
    }
    return false;
}

// Sprawdza kolizje pomiedzy Pacmanem i monetami
bool checkPacmanPointCollision() {
    for (const glm::vec3& position : pointPositions) {
        if (detectCollision(pacmanPosition, position, pointDetectionDistance)) {
            return true;
        }
    }
    return false;
}

void updatePacmanPosition(float deltaTime, const std::vector<float>& mazeVertices, bool& gameStarted, bool& gameOver) {
        if (pacmanPosition.x > 4.5f) {  // Przejscie na druga strone labiryntu
            pacmanPosition.x = -4.3f;
        }
        else if (pacmanPosition.x < -4.5f) {  // Przejscie na druga strone labiryntu
            pacmanPosition.x = 4.3f;
        }
        else {
            glm::vec3 potentialPosition = pacmanPosition;
            float newSpeed_x = 0.0f;
            float newSpeed_y = 0.0f;

            // Ustawia potencjalna nowa predkosc, na podstawie pozadanego nowego kierunku
            switch (desiredPacmanDirection) {
            case UP:
                newSpeed_y = -pacmanSpeed;
                break;
            case DOWN:
                newSpeed_y = pacmanSpeed;
                break;
            case LEFT:
                newSpeed_x = -pacmanSpeed;
                break;
            case RIGHT:
                newSpeed_x = pacmanSpeed;
                break;
            }

            // Oblicza potencjalna nowa pozycje
            potentialPosition += glm::vec3(newSpeed_x * deltaTime, 0.0f, newSpeed_y * deltaTime);

            // Sprawdza kolizje w pozadanym kierunku, uzywajac detectionDistance
            glm::vec3 detectionPosition = pacmanPosition + glm::vec3(newSpeed_x * detectionDistance, 0.0f, newSpeed_y * detectionDistance);
            if (!checkWallCollision(detectionPosition, mazeVertices)) {  // Sprawdza kolizje ze sciana
                if (checkPacmanGhostCollision()) {  // Sprawdza kolizje z duszkami
                    resetGame(gameStarted, gameOver); 
                }
                else {
                    // Jesli nie ma kolizji, aktualizuje pozycje Pacmana
                    pacmanPosition = potentialPosition;
                    pacmanSpeed_x = newSpeed_x;
                    pacmanSpeed_y = newSpeed_y;
                    lastPacmanDirection = desiredPacmanDirection;
                }
            }
            else {
                // Jesli wystapi kolizcja, probuje kontynuowac ruch w poprzednim prawidlowym kierunku
                potentialPosition = pacmanPosition;
                newSpeed_x = 0.0f;
                newSpeed_y = 0.0f;

                switch (lastPacmanDirection) {
                case UP:
                    newSpeed_y = -pacmanSpeed;
                    break;
                case DOWN:
                    newSpeed_y = pacmanSpeed;
                    break;
                case LEFT:
                    newSpeed_x = -pacmanSpeed;
                    break;
                case RIGHT:
                    newSpeed_x = pacmanSpeed;
                    break;
                }

                // Sprawdza kolizje w ostatnim prawidlowym kierunku
                detectionPosition = pacmanPosition + glm::vec3(newSpeed_x * detectionDistance, 0.0f, newSpeed_y * detectionDistance);
                potentialPosition = pacmanPosition + glm::vec3(newSpeed_x * deltaTime, 0.0f, newSpeed_y * deltaTime);
                if (!checkWallCollision(detectionPosition, mazeVertices)) {
                    // Jesli nie ma kolizji, kontynuje poprzedni ruch
                    if (checkPacmanGhostCollision()) {
                        resetGame(gameStarted, gameOver);
                    }
                    else {
                        pacmanPosition = potentialPosition;
                        pacmanSpeed_x = newSpeed_x;
                        pacmanSpeed_y = newSpeed_y;
                    }
                }
                else {
                    // Jesli w poprzednim prawid³owym kierunku takze nastepuje kolizja, Pacman jest zatrzymywany
                    stopPacman();
                }
            }
        }

        // Jesli wykryto kolizje Pacmana z moneta, model monety jest usuwany
        for (auto it = pointPositions.begin(); it != pointPositions.end(); ) {
            if (detectCollision(pacmanPosition, *it, detectionDistance)) {
                it = pointPositions.erase(it);  // Usun monete z wektora
            }
            else {
                ++it;
            }
        }

        // Sprawdza czy wszystkie monety zosta³y usuniete
        if (pointPositions.empty()) {
            system("cls");
            printf("Win!");
            gameOver = true;
            gameStarted = false;
            soundEngine->stopAllSounds();
            soundEngine->play2D("resources/audio/pacman_win.wav", false); // Dzwiek wygranej
        }
}

Direction getRandomDirection() {
    return Direction(rand() % 4);
}

glm::vec3 getDirectionVector(Direction direction) {
    switch (direction) {
    case UP:
        return glm::vec3(0.0f, 0.0f, -1.0f);
    case DOWN:
        return glm::vec3(0.0f, 0.0f, 1.0f);
    case LEFT:
        return glm::vec3(-1.0f, 0.0f, 0.0f);
    case RIGHT:
        return glm::vec3(1.0f, 0.0f, 0.0f);
    default:
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

void updateGhostTransition(glm::vec3& ghostPosition, glm::vec3 targetPosition, float deltaTime) {
    glm::vec3 direction = glm::normalize(targetPosition - ghostPosition);
    glm::vec3 movement = direction * ghostSpeed * deltaTime;

    if (glm::length(targetPosition - ghostPosition) < glm::length(movement)) {
        ghostPosition = targetPosition;
    }
    else {
        ghostPosition += movement;
    }
}

bool isInRestrictedZone(const glm::vec3& position) {
    return (position.x > restrictedZoneMin.x && position.x < restrictedZoneMax.x &&
        position.z > restrictedZoneMin.z && position.z < restrictedZoneMax.z);
}

void updateGhostPosition(glm::vec3& ghostPosition, Direction& currentDirection, const std::vector<float>& mazeVertices, float deltaTime) {
    glm::vec3 directionVector = getDirectionVector(currentDirection);
    glm::vec3 potentialPosition = ghostPosition + directionVector * ghostSpeed * deltaTime;

    // Sprawdza kolizje w pozadanym kierunku przy uzyciu ghostDetectionDistance
    glm::vec3 detectionPosition = ghostPosition + directionVector * ghostDetectionDistance;

    // Sprawdza, czy potencjalna pozycja znajduje siê w strefie ograniczonej dla duszkow
    if (!checkWallCollision(detectionPosition, mazeVertices) && !isInRestrictedZone(potentialPosition)) {
        ghostPosition = potentialPosition;
    }
    else {
        // Jesli wystapi kolizja lub duch probuje wejsc w strefe ograniczona, wybiera nowy losowy kierunek
        currentDirection = getRandomDirection();
    }

    // Przechodzenie na druga strone labiryntu
    if (ghostPosition.x > 4.5f) {
        ghostPosition.x = -4.3f;
    }
    else if (ghostPosition.x < -4.5f) {
        ghostPosition.x = 4.3f;
    }
}

void updateGhostPositions(float deltaTime, const std::vector<float>& mazeVertices, bool& gameStarted, bool& gameOver) {
    static Direction currentDirectionRed = getRandomDirection();
    static Direction currentDirectionBlue = getRandomDirection();
    static Direction currentDirectionPink = getRandomDirection();
    static Direction currentDirectionOrange = getRandomDirection();

    // Czerwony duszek
    updateGhostPosition(ghostPositionRed, currentDirectionRed, mazeVertices, deltaTime);

    // Na poczatku gry (tryb PREAPATING duszkow), przesuniecie niebieskiego, rozowego i pomaranczowego duszka

    if (ghostStates[1] == PREAPARING) {
        updateGhostTransition(ghostPositionBlue, ghostPositionRedInitial, deltaTime);
        if (ghostPositionBlue == ghostPositionRedInitial) {  // Przesuniecie na pozycje czerwonego duszka
            ghostStates[1] = NORMAL;
        }
    }
    else {
        updateGhostPosition(ghostPositionBlue, currentDirectionBlue, mazeVertices, deltaTime);
    }

    if (ghostStates[2] == PREAPARING) {
        updateGhostTransition(ghostPositionPink, ghostPositionRedInitial, deltaTime);
        if (ghostPositionPink == ghostPositionRedInitial) {
            ghostStates[2] = NORMAL;
        }
    }
    else {
        updateGhostPosition(ghostPositionPink, currentDirectionPink, mazeVertices, deltaTime);
    }

    if (ghostStates[3] == PREAPARING) {
        updateGhostTransition(ghostPositionOrange, ghostPositionRedInitial, deltaTime);
        if (ghostPositionOrange == ghostPositionRedInitial) {
            ghostStates[3] = NORMAL;
        }
    }
    else {
        updateGhostPosition(ghostPositionOrange, currentDirectionOrange, mazeVertices, deltaTime);
    }
    if (checkPacmanGhostCollision()) {
        resetGame(gameStarted, gameOver);
    }
}
