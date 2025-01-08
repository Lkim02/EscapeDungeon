#ifndef WORLD_H
#define WORLD_H

#include "common.h"
#include "world/Object.h"
#include "world/Player.h"

#include "camera/Camera.h"
#include "shader/Shader.h"
#include "model/Model.h"
#include "animation/Animator.h"
#include "animation/Animation.h"
#include "physics/CollisionCB.h"

#include <vector>

class World {
public:
	World();
	~World() {
		// Must in order: Material -> Scene
		if (mCookingParams) { delete mCookingParams;	mCookingParams = nullptr;	std::cout << "Cleared mCookingParams" << std::endl; }
		if (mMaterial)		{ mMaterial->release();		mMaterial = nullptr;		std::cout << "Cleared mMaterial" << std::endl; }
		if (mScene)			{ mScene->release();		mScene = nullptr;			std::cout << "Cleared mScene" << std::endl; }
		if (mPhysics)		{ mPhysics->release();		mPhysics = nullptr;			std::cout << "Cleared mPhysics" << std::endl; }
		if (mDispatcher)	{ mDispatcher->release();	mDispatcher = nullptr;		std::cout << "Cleared mDispatcher" << std::endl; }
		if (mPvd) { mPvd->release();			mPvd = nullptr;				std::cout << "Cleared mPvd" << std::endl; } // Not used

		if (mCollisionCallback) { delete mCollisionCallback; mCollisionCallback = nullptr; }

		// Must be the Last One
		if (mFoundation)	{ mFoundation->release();	mFoundation = nullptr;		std::cout << "Cleared mFoundation" << std::endl; }
	}

	void init();
	void start();
	void update();
	void render();

	/*=== Interact ===*/
	void processKeyboardPress();
	void processKeyboardInput();
	void processKeyboardRelease();
	void processMouseMovement(float xoffset, float yoffset);
	void processMouseScroll(float yoffset);

private:
	/*=== World ===*/
	float _border = 20.0f;

	/*=== Object ===*/
	std::shared_ptr<Player> _player = nullptr;
	std::vector<std::shared_ptr<Object>> _objects;
	std::shared_ptr<Object> _signature = nullptr;

	/*=== Camera ===*/
	Camera _camera;

	/*=== Light ===*/
	glm::vec3 _light_pos;
	glm::vec3 _light_color;

	/*=== Time ===*/
	float _last_frame = 0.0f;
	float _delta_time = 0.0f;

	/*=== MVP ===*/
	glm::mat4 _model, _view, _projection;
	glm::mat3 _norm_model;

	/*=== Shader ===*/
	std::shared_ptr<Shader> _global_shader;

	/*=== Physics ===*/
	// PhysX Variables
	physx::PxDefaultAllocator		mAllocator;
	physx::PxDefaultErrorCallback	mErrorCallback;
	physx::PxFoundation*			mFoundation = nullptr;
	physx::PxPvd*					mPvd = nullptr;
	physx::PxDefaultCpuDispatcher*	mDispatcher = nullptr;
	
	physx::PxTolerancesScale		mToleranceScale;
	physx::PxPhysics*				mPhysics = nullptr;
	physx::PxCookingParams*			mCookingParams = nullptr;  // no more PxCooking
	
	physx::PxScene*					mScene = nullptr;
	//physx::PxPvdSceneClient*		mPvdClient = nullptr;
	physx::PxMaterial*				mMaterial = nullptr;
	MyCollisionCallback*			mCollisionCallback = nullptr;
	
	/* Utils */
	bool need_start = true;
	bool first_start = true;

	void initPhysics();
	void initObjects();
};


physx::PxFilterFlags MyFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);

#endif // !WORLD_H

