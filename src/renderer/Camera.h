//
// Created by bison on 10-12-2017.
//

#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    TURN_LEFT,
    TURN_RIGHT
};

// Default camera values
const float YAW        = -90.0f;
const float PITCH      = 0.0f;
const float SPEED      = 3.0f;
const float SENSITIVTY = 0.075f;
//const float SENSITIVTY =  0.015f;
const float ZOOM       = 75.0f;

struct CameraAnimation {
    Camera_Movement direction;
    float distance;
    float duration;
    float elapsed;
    float angle;
    glm::vec3 position;
};

struct HeadBobbing {
    float frequency = 2.5f; //  Frequency of head bobbing (how many times per second)
    float magnitude = 0.05f; //  Magnitude of head bobbing (how far the head moves)
    float phase = 0.0f; //  Phase offset of the head bobbing
    float displacement = 0.0f; //  Current displacement of the head bobbing
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    std::vector<CameraAnimation> animations;
    HeadBobbing headBobbing;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        auto position = glm::vec3(Position.x, Position.y + headBobbing.displacement, Position.z);
        return glm::lookAt(position, position + Front, Up);
    }

    void AnimateMove(Camera_Movement direction, float duration, float distance) {
        CameraAnimation a{};
        a.direction = direction;
        a.duration = duration;
        a.distance = distance;
        a.elapsed = 0.0f;
        a.position = Position;
        a.angle = Yaw;
        if(direction == TURN_LEFT) {
            a.distance = -90.0f;
        }
        else if(direction == TURN_RIGHT) {
            a.distance = 90.0f;
        }
        animations.push_back(a);
    }

    void updateHeadBobbing(float delta) {
        // Calculate headbobbing displacement using a sinusoidal function
        headBobbing.displacement = headBobbing.magnitude * sin(2 * M_PI * headBobbing.frequency * headBobbing.phase);
        headBobbing.phase += delta;
    }

    void Update(float delta) {
        if(animations.empty()) {
            return;
        }

        auto& a = animations[0];
        a.elapsed += delta;
        if(a.elapsed >= a.duration) {
            a.elapsed = a.duration;
        }
        float t = a.elapsed / a.duration;
        float d = a.distance * t;
        switch(a.direction) {
            case FORWARD: {
                Position = a.position + Front * d;
                updateHeadBobbing(delta);
                break;
            }
            case BACKWARD:
                Position = a.position - Front * d;
                updateHeadBobbing(delta);
                break;
            case LEFT:
                Position = a.position - Right * d;
                updateHeadBobbing(delta);
                break;
            case RIGHT:
                Position = a.position + Right * d;
                updateHeadBobbing(delta);
                break;
            case TURN_LEFT:
                Yaw = a.angle + d;
                updateCameraVectors();
                break;
            case TURN_RIGHT:
                Yaw = a.angle + d;
                updateCameraVectors();
                break;
        }
        if(a.elapsed >= a.duration) {
            animations.erase(animations.begin());
            if(!animations.empty()) {
                animations[0].position = Position;
                animations[0].angle = Yaw;
            }
            //SDL_Log("complete animations.size() = %zu", animations.size());
        }
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Eular angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= ZOOM)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= ZOOM)
            Zoom = ZOOM;
    }

    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

#endif //GAME_CAMERA_H
