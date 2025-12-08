#pragma once

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// define all possible directions of camera movements
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// default camera values
const float YAW = -90.0f;  // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float ROTATION_SPEED = 1.0f; // adjust rotation speed as needed
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera
{
private:

    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }

public:

    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // euler Angles
    float Yaw;
    float Pitch;

    // camera options
    float MovementSpeed;
    float RotationSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors | has default values | using an init list is more efficient than assigning values inside the constructor body
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
    : Front(glm::vec3(0.0f, 0.0f, -1.0f))
    , MovementSpeed(SPEED)
    , RotationSpeed(ROTATION_SPEED)
    , MouseSensitivity(SENSITIVITY)
    , Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // constructor with scalar values | has no default values | using an init list is more efficient than assigning values inside the constructor body
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
    : Front(glm::vec3(0.0f, 0.0f, -1.0f))
    , MovementSpeed(SPEED)
    , RotationSpeed(ROTATION_SPEED)
    , MouseSensitivity(SENSITIVITY)
    , Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system
    // accepts input parameter in the form of camera defined ENUM 
    // (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        float dTheta = RotationSpeed * deltaTime;

        if (direction == FORWARD)
        {
            // Position += Front * velocity;
            glm::mat4 rotation_matr = glm::rotate(glm::mat4(1.0f), -dTheta, Right); // not glm::vec3(1, 0, 0) to rotate around the axis but Right to rotate in suface
            glm::vec4 rotated4 = rotation_matr * glm::vec4(Position, 1.0f);
            Position = glm::vec3(rotated4);
            updateCameraVectors();
        }
        if (direction == BACKWARD)
        {
            // Position -= Front * velocity;
            glm::mat4 rotation_matr = glm::rotate(glm::mat4(1.0f), dTheta, Right);
            glm::vec4 rotated4 = rotation_matr * glm::vec4(Position, 1.0f);
            Position = glm::vec3(rotated4);
            updateCameraVectors();
        }
        if (direction == LEFT)
        {
            glm::mat4 rotation_matr = glm::rotate(glm::mat4(1.0f), -dTheta, Up);
            glm::vec4 rotated4 = rotation_matr * glm::vec4(Position, 1.0f);
            Position = glm::vec3(rotated4);
            updateCameraVectors();
        }
        if (direction == RIGHT)
        {
            glm::mat4 rotation_matr = glm::rotate(glm::mat4(1.0f), dTheta, Up);
            glm::vec4 rotated4 = rotation_matr * glm::vec4(Position, 1.0f);
            Position = glm::vec3(rotated4);
            updateCameraVectors();
        }
    }

    // processes input received from a mouse input system
    // expects the offset value in both the x and y direction
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. 
    // only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;

        if (Zoom < 1.0f)
            Zoom = 1.0f;

        if (Zoom > 120.0f)
            Zoom = 120.0f;
    }
};


