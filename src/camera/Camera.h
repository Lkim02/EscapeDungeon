#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"
#include "world/Object.h"

const float YAW = -90.0f;
const float PITCH = -50.0f;
const float SPEED = 6.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

enum CameraStatus {
	CAMERA_FREE = 0,
	CAMERA_FOLLOW
};

class Camera {
public:
    /* Camera Attribute */
    glm::vec3 position;
	glm::vec3 front, up, right;
	glm::vec3 world_up;
	/* Euler Angles */
    float yaw;
    float pitch;
	/* Camera options */
	float movement_speed;
	float mouse_sensitivity;
	float zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 wup = glm::vec3(0.0f, 1.0f, 0.0f), 
		   float yaw = YAW, float pitch = PITCH);
	~Camera() = default;

	glm::mat4 getViewMatrix() { return glm::lookAt(position, position + front, up); }

	void switchMode(std::shared_ptr<Object> obj);

	void processKeyboard(float delta_time);
	void processMouseMovement(float xoffset, float yoffset, GLboolean constrain_pitch = true);
	void processMouseScroll(float yoffset);

private:
	CameraStatus _status = CAMERA_FREE;
	std::shared_ptr<Object> _followed_object = nullptr;
	glm::vec3 _followed_offset;
	void follow(std::shared_ptr<Object> obj);
	void free();

	void update(); // update camera vector
};

#endif // !CAMERA_H
