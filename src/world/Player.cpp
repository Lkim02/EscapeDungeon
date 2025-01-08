
#include "Player.h"

#include "utils.h"

void Player::walk() {
	if (_islocked) return;

	_status = PLAYER_WALK; 
	if (!walking_sound || walking_sound->isFinished()) {
		walking_sound = sound_engine->play2D("assets/audios/walking.mp3", true, false, true);
		walking_sound->setVolume(0.8f);
	}
}

void Player::idle() { 
	if (_islocked) return;

	_status = PLAYER_IDLE; 
	animator->reset(); 
	if (walking_sound && !walking_sound->isFinished()) {
		walking_sound->stop();
	}
}

void Player::update(float _delta_time) {

	if (_status == PLAYER_WALK) {
		animator->update(_delta_time);
	}

	physx::PxTransform transform_to_test(
		physx::PxVec3(_position.x, _position.y, _position.z),
		physx::PxQuat(_rotation.x, _rotation.y, _rotation.z, _rotation.w)
	);
	transform_to_test.p = physx::PxVec3(_position.x, _position.y + _aabb_hy, _position.z); // ignore rotation first
#ifdef PHYSIC_IMPL
	rigid_dynamic->setKinematicTarget(transform_to_test);
#endif
}


void Player::processKeyboard(float delta_time) {
	if (_islocked) return;

	/* position and rotation */
	float velocity = _movement_speed * delta_time;

	// be careful: assume true = 1, false = 0
	glm::vec3 pace = static_cast<float>(keyboard[GLFW_KEY_W]) * pace_vec[GLFW_KEY_W] 
				   + static_cast<float>(keyboard[GLFW_KEY_S]) * pace_vec[GLFW_KEY_S] 
				   + static_cast<float>(keyboard[GLFW_KEY_A]) * pace_vec[GLFW_KEY_A] 
				   + static_cast<float>(keyboard[GLFW_KEY_D]) * pace_vec[GLFW_KEY_D];
	
	if (pace != glm::vec3(0.0f, 0.0f, 0.0f)) {
		pace = glm::normalize(pace);
		_front = pace; // depend on createModelMactrix: front (not -front)
		_position += pace * velocity;
		limiting();
	}

}

void Player::processMouseMovement(float xoffset, float yoffset) {

}

void Player::processMouseScroll(float yoffset) {

}


physx::PxRigidDynamic* Player::createRigidDynamic(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams, physx::PxMaterial* material) {
	cookAndCreateTriangleMesh(physics, cookingParams);

	px_type = DYNAMIC;
	_px_transform = physx::PxTransform(
		physx::PxVec3(_position.x, _position.y, _position.z),
		physx::PxQuat(_rotation.x, _rotation.y, _rotation.z, _rotation.w)
	);
	_px_transform.p += physx::PxVec3(0.0f, _aabb_hy, 0.0f);
	rigid_dynamic = physics->createRigidDynamic(_px_transform);
	{
		//physx::PxShape* shape = physics->createShape(physx::PxTriangleMeshGeometry(px_triangle_mesh), *material);
		physx::PxShape* shape = physics->createShape(physx::PxBoxGeometry(0.8f, _aabb_hy, 0.3f), *material);
		
		shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
		shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
		physx::PxFilterData filterData;
		filterData.word0 = 1; filterData.word1 = 1;
		shape->setSimulationFilterData(filterData);
		
		rigid_dynamic->attachShape(*shape);
		shape->release();
	}
	rigid_dynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
	return rigid_dynamic;
}

void Player::updateSimulateResult() {
	if (kinematicTouchStatic) {
		// the _position = > transform_to_test is illegal, so set back to previous _px_transform
		_position = glm::vec3(_px_transform.p.x, _px_transform.p.y - _aabb_hy, _px_transform.p.z);
	}
	else { // thus confirm to update _px_transform to new _position
		_px_transform.p = physx::PxVec3(_position.x, _position.y + _aabb_hy, _position.z);
	}

	updateModelMatrixThroughEuler();
}