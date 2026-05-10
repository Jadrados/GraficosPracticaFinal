#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:

    glm::vec3 m_cop;
    glm::vec3 m_lookAt;
    glm::vec3 m_up;


    glm::mat4 buildViewMatrix() const {
        // 1. Calcular Forward (dirección hacia donde mira la cámara)
        glm::vec3 forward = glm::normalize(m_lookAt - m_cop);

        // 2. Calcular Right (perpendicular a Forward y Up)
        glm::vec3 right = glm::normalize(glm::cross(m_up, forward));

        // 3. Calcular el nuevo Up (perpendicular a Forward y Right)
        glm::vec3 upActual = glm::normalize(glm::cross(forward, right));

        // 4. Construir la matriz de vista manualmente
        glm::mat4 view(1.0f);

        // Primera fila (Right)
        view[0][0] = right.x;
        view[1][0] = right.y;
        view[2][0] = right.z;
        view[3][0] = -glm::dot(right, m_cop);

        // Segunda fila (Up)
        view[0][1] = upActual.x;
        view[1][1] = upActual.y;
        view[2][1] = upActual.z;
        view[3][1] = -glm::dot(upActual, m_cop);

        // Tercera fila (Forward)
        view[0][2] = -forward.x;
        view[1][2] = -forward.y;
        view[2][2] = -forward.z;
        view[3][2] = glm::dot(forward, m_cop);

        // Cuarta fila no se toca, queda [0,0,0,1]

        return view;
    }

public:

    Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 upVec) {
        m_cop = pos;
        m_lookAt = target;
        m_up = upVec;
    }


    glm::mat4 getViewMatrix() {
        return buildViewMatrix();
    }


    void setPosition(glm::vec3 newPos) {
        m_cop = newPos;
    }

    void setLookAt(glm::vec3 newTarget) {
        m_lookAt = newTarget;
    }

    void moveForward(float distance) {
        glm::vec3 direction = glm::normalize(m_lookAt - m_cop);
        m_cop = m_cop + direction * distance;
        m_lookAt = m_lookAt + direction * distance;
    }

    void moveBack(float distance) {
        moveForward(-distance);
    }

    void moveRight(float distance) {
        glm::vec3 direction = glm::normalize(m_lookAt - m_cop);
        glm::vec3 right = glm::normalize(glm::cross(m_up, direction));
        m_cop = m_cop + right * distance;
        m_lookAt = m_lookAt + right * distance;
    }

    void moveLeft(float distance) {
        moveRight(-distance);
    }

    void yaw(float angleDegrees) {
        // Girar a la izquierda/derecha (alrededor del eje Y)
        glm::vec3 forward = m_lookAt - m_cop;
        float rad = glm::radians(angleDegrees);

        // Matriz de rotación alrededor de Y (manual, sin glm::rotate)
        float cosA = cos(rad);
        float sinA = sin(rad);

        glm::vec3 newForward;
        newForward.x = forward.x * cosA + forward.z * sinA;
        newForward.y = forward.y;
        newForward.z = -forward.x * sinA + forward.z * cosA;

        m_lookAt = m_cop + newForward;
    }

    void pitch(float angleDegrees) {
        // Girar arriba/abajo (alrededor del eje X local de la cámara)
        glm::vec3 forward = m_lookAt - m_cop;
        glm::vec3 right = glm::normalize(glm::cross(m_up, forward));

        float rad = glm::radians(angleDegrees);
        float cosA = cos(rad);
        float sinA = sin(rad);

        // Rotar forward alrededor de right
        glm::vec3 newForward;
        newForward.x = forward.x * cosA + right.x * sinA;
        newForward.y = forward.y * cosA + right.y * sinA;
        newForward.z = forward.z * cosA + right.z * sinA;

        m_lookAt = m_cop + newForward;
    }

    glm::vec3 getPosition() const { return m_cop; }
    glm::vec3 getLookAt() const { return m_lookAt; }
};

#endif