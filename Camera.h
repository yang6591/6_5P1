#ifndef CAMERA_H
#define CAMERA_H

#include<QMatrix4x4>

#include<vector>
#include <windows.h>
#include<GL/gl.h>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    DOWN,
    UP
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;//视野角度


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    QVector3D Position;
    QVector3D Front;
    QVector3D Up;
    QVector3D Right;
    QVector3D WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(QVector3D(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(QVector3D(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = QVector3D(posX, posY, posZ);
        WorldUp = QVector3D(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    QMatrix4x4 GetViewMatrix()
    {
        QMatrix4x4 theMatrix;
        theMatrix.lookAt(Position, Position + Front, Up);
        return theMatrix;
    }

    // 新增：获取投影矩阵
    QMatrix4x4 GetProjectionMatrix(float aspectRatio) const {
        QMatrix4x4 projection;
        projection.perspective(Zoom, aspectRatio, 0.1f, 1000.0f);
        return projection;
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position += Right * velocity;
        if (direction == RIGHT)
            Position -= Right * velocity;
        if (direction == DOWN)
            Position -= Up * velocity;
        if (direction == UP)
            Position += Up * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
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

    //视角与模型距离不变，视角宽窄变化
    void ProcessMouseScroll(float yoffset)
    {
        float Sensitivity = 0.1f;  // 默认灵敏度
        float MinZoom = 1.0f;      // 最小缩放值
        float MaxZoom = 120.0f;     // 最大缩放值
        // 指数缩放 + 灵敏度控制
        float scaleFactor = std::exp(-yoffset * Sensitivity);//放大越来越慢缩小越来越快
        Zoom *= scaleFactor;
        Zoom = std::clamp(Zoom, MinZoom, MaxZoom);
        // Zoom -= (float)yoffset;
        // if (Zoom < 1.0f)
        //     Zoom = 1.0f;
        // if (Zoom > 120.0f)
        //     Zoom = 120.0f;
    }

    //视角宽窄不变，视角与模型间的距离变化
    float InitialDistance;  // 存储初始距离
    void SetInitialDistance(float distance) {
        InitialDistance = distance;
    }
    void ProcessDistanceChange(float yoffset, const QVector3D& targetPosition)
    {
        const float Sensitivity = 0.1f;       // 灵敏度
        const float MinDistance = 0.01f;       // 最小距离
        const float MaxDistance = InitialDistance * 10.0f;  // 最大距离为初始距离的10倍

        // 计算当前距离
        float currentDistance = (Position - targetPosition).length();

        // 计算缩放因子（指数变化）
        float scaleFactor = std::exp(yoffset * Sensitivity);

        // 计算新距离
        float newDistance = currentDistance * scaleFactor;
        newDistance = std::clamp(newDistance, MinDistance, MaxDistance);

        // 计算新的相机位置（沿视线方向移动）
        QVector3D direction = (Position - targetPosition).normalized();//方向从物体指向相机
        Position = targetPosition + direction * newDistance;//缩放因子越小相机离得越近并且近的越来越慢
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        float PI=3.1415926f;
        QVector3D front;
        front.setX(cos(Yaw*PI/180.0) * cos(Pitch*PI/180.0));
        front.setY(sin(Pitch*PI/180.0));
        front.setZ(sin(Yaw*PI/180.0) * cos(Pitch*PI/180.0));
        front.normalize();
        Front = front;
        // also re-calculate the Right and Up vector
        Right = QVector3D::crossProduct(Front, WorldUp);
        Right.normalize();
        // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = QVector3D::crossProduct(Right, Front);
        Up.normalize();
    }
};
#endif
