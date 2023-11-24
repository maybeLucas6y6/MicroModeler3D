#pragma once 

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

enum class CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0, float pitch = 0.0f);

    inline glm::mat4 GetViewMatrix() { return glm::lookAt(mPosition, mPosition + mFront, mUp); }
    const float GetZoom() { return mZoom; }
    const glm::vec3 GetPosition() { return mPosition; }

    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);

private:
    glm::vec3 mPosition;
    glm::vec3 mFront;
    glm::vec3 mUp;
    glm::vec3 mRight;
    glm::vec3 mWorldUp;

    float mYaw;
    float mPitch;

    float mMovementSpeed;
    float mMouseSensitivity;
    float mZoom;

    void UpdateCameraVectors();
};