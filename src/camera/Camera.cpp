
#include "Camera.h"
#include "utils.h"

#include <assert.h>

Camera::Camera(glm::vec3 pos, glm::vec3 wup, float yaw, float pitch):
		position(pos), world_up(wup), yaw(yaw), pitch(pitch),
		front(glm::vec3(0.0f, 0.0f, -1.0f)), // reverse to direction
		movement_speed(SPEED), mouse_sensitivity(SENSITIVITY), zoom(ZOOM) {

	update();
}


void Camera::follow(std::shared_ptr<Object> obj) {
	assert(obj != nullptr);
	_status = CAMERA_FOLLOW;
	obj->unlock();
	_followed_object = obj;
	_followed_offset = position - _followed_object->getPosition();
#ifdef DEBUG
	std::cout << "Camera: switch to mode FOLLOW" << std::endl;
#endif 
}

void Camera::free() {
	_status = CAMERA_FREE; 
	_followed_object->lock();
	_followed_object = nullptr;
	_followed_offset = glm::vec3(0.0f);
#ifdef DEBUG
	std::cout << "Camera: switch to mode FREE" << std::endl;
#endif 
}

void Camera::switchMode(std::shared_ptr<Object> obj) {
	if (_status == CAMERA_FREE) {
		follow(obj);
	}
	else if (_status == CAMERA_FOLLOW) {
		free();
	}
}

/* Camera Movement */
// TODO: can be clearer, seperate input semantics with movement semantics
void Camera::processKeyboard(float delta_t) {
	if (_status == CAMERA_FOLLOW) {
		assert(_followed_object != nullptr);
		position = _followed_object->getPosition() + _followed_offset;
		return ;
	} 

	float velocity = movement_speed * delta_t;
	if (keyboard[GLFW_KEY_W])		position += front * velocity;
	else if (keyboard[GLFW_KEY_S])	position -= front * velocity;
	else if (keyboard[GLFW_KEY_A])	position -= right * velocity;
	else if (keyboard[GLFW_KEY_D])  position += right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrain_pitch) {
	if (_status != CAMERA_FREE) return ;
	
	xoffset *= mouse_sensitivity;
    yoffset *= mouse_sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    // Limited the bound of pitch
    if (constrain_pitch) {
        if (pitch >  89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
    }

    update();
}

void Camera::processMouseScroll(float yoffset) {
	zoom -= yoffset;
	if (zoom <  1.0f) zoom = 1.0f;
	if (zoom > 45.0f) zoom = 45.0f;
}


/* Camera Vector Update */ 
void Camera::update() {
	/* Calculate the new Front vector */
	glm::vec3 front;
	front.y = sin(glm::radians(pitch));
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	/* Re-calculate the right and up vector, need normalized for usage in speed */
	this->front = glm::normalize(front);
	this->right = glm::normalize(glm::cross(this->front, this->world_up));
	this->up = glm::normalize(glm::cross(this->right, this->front));
}


