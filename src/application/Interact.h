#ifndef INTERACT_H
#define INTERACT_H

#include "common.h"

/* Interact Interface Defination */

class Interactable {
public:
	virtual void processKeyboard(Movement direction, float deltaTime) = 0;
	virtual void processMouseMovement(float xoffset, float yoffset, GLboolean constrain_pitch = true) = 0;
	virtual void processMouseScroll(float xoffset, float yoffset) = 0;
};

#endif // !INTERACT_H
