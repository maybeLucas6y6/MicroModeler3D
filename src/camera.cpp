#include "camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    mFront(glm::vec3(0.0f, 0.0f, -1.0f)),
    mMovementSpeed(4.0f),
    mMouseSensitivity(0.1f),
    mZoom(45.0f)
{
    mPosition = position;
    mWorldUp = up;
    mYaw = yaw;
    mPitch = pitch;
    UpdateCameraVectors();
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = mMovementSpeed * deltaTime;
    if (direction == CameraMovement::FORWARD)
        mPosition += mFront * velocity;
    if (direction == CameraMovement::BACKWARD)
        mPosition -= mFront * velocity;
    if (direction == CameraMovement::LEFT)
        mPosition -= mRight * velocity;
    if (direction == CameraMovement::RIGHT)
        mPosition += mRight * velocity;
    if (direction == CameraMovement::UP)
        mPosition += mUp * velocity;
    if (direction == CameraMovement::DOWN)
        mPosition -= mUp * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= mMouseSensitivity;
    yoffset *= mMouseSensitivity;

    mYaw += xoffset;
    mPitch += yoffset;

    if (constrainPitch)
    {
        if (mPitch > 89.0f)
            mPitch = 89.0f;
        if (mPitch < -89.0f)
            mPitch = -89.0f;
    }

    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    mZoom -= yoffset;
    if (mZoom < 1.0f)
        mZoom = 1.0f;
    if (mZoom > 90.0f)
        mZoom = 90.0f;
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    front.y = sin(glm::radians(mPitch));
    front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mFront = glm::normalize(front);
    mRight = glm::normalize(glm::cross(mFront, mWorldUp));
    mUp = glm::normalize(glm::cross(mRight, mFront));
}