#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"
#include "Object.h"
#include "utils.h"


enum PlayerStatus {
	PLAYER_IDLE = 0,
	PLAYER_WALK,
};

class Player : public Object {
public:
	Player() = default;
	Player(std::shared_ptr<Shader> shader, std::shared_ptr<Model> model, float activity_range, glm::mat4 model_matrix)
			: Object(shader, model, model_matrix, DYNAMIC), _activity_range(activity_range) {
		animator->reset();
	}
	Player(std::shared_ptr<Shader> shader, std::shared_ptr<Model> model, float activity_range,
		glm::vec3 position = glm::vec3(0.0f), float scale = 1.0f,
		glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f))
		: Object(shader, model, DYNAMIC, position, scale, front, world_up), _activity_range(activity_range) {
		animator->reset();
	}
	~Player() {}

	void walk();
	void idle();
	void update(float delta_time) override;

	void processKeyboard(float delta_time) override;
	void processMouseMovement(float xoffset, float yoffset) override;
	void processMouseScroll(float yoffset) override;

	/* Physics */
	physx::PxRigidDynamic* createRigidDynamic(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams, physx::PxMaterial* material) override;
	void updateSimulateResult() override;

	/* Camera Mode */
	void lock() override { idle(); _islocked = true; }
	void unlock() override { _islocked = false; }

protected:
	PlayerStatus _status = PLAYER_IDLE;
	float _movement_speed = 4.0f;
	float _activity_range = 50.f;

	bool _islocked = false;

	/* Physics */
	float _aabb_hy = 3.0f;

	inline void limiting() {
		if (_position.x < -_activity_range) _position.x = -_activity_range;
		if (_position.x >  _activity_range) _position.x =  _activity_range;
		if (_position.z < -_activity_range) _position.z = -_activity_range;
		if (_position.z >  _activity_range) _position.z =  _activity_range;
	}

	irrklang::ISound* walking_sound = nullptr;
};

#endif // !PLAYER_H
