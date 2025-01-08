
#include "World.h"
#include "utils.h"

bool kinematicTouchStatic = false;

World::World() {
	/* Init Time */ 
	_last_frame = 0.0f;
	_delta_time = 0.0f;

	/* Init Camera */
	_camera = Camera(glm::vec3(0.0f, 11.0f, 10.0f));

	/* Init Light */
	_light_pos = glm::vec3(1.2f, 1.0f, 2.0f);
	_light_color = glm::vec3(1.0f);

}

void World::init() {
#ifdef PHYSIC_IMPL
	initPhysics();
#endif
	initObjects();

	/* Init BGM */
	sound_engine->play2D("assets/audios/Skeleton_A_Bloody_Labyrinth.mp3", true);
}

void World::start() {
#ifdef PHYSIC_IMPL
	for (auto& obj : _objects) {
		obj->start();
	}
#endif // PHYSIC_IMPL

	// important! or the first _delta_time would be extremely large
	_last_frame = static_cast<float>(glfwGetTime());
}

void World::update() {
	/* Calculate time */
	float current_frame = static_cast<float>(glfwGetTime());
	_delta_time = current_frame - _last_frame;
	_last_frame = current_frame;

	/* React to Input */
	processKeyboardInput();

	/* Object Update: Animate or Try Moving */
	for (auto& obj : _objects) {
		obj->update(_delta_time);
	}

	/* Get Physcs Simulation: Update or Amend */
#ifdef PHYSIC_IMPL
	if (mScene) {
		mScene->simulate(_delta_time);
		mScene->fetchResults(true);
		for (auto& obj : _objects) {
			obj->updateSimulateResult();
		}
	}
#endif
}

void World::render() {
	/* Clear canvas */
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	/* Global Peojection & View Matrix */
	_projection = glm::perspective(glm::radians(_camera.zoom), (float)viewport_width / (float)viewport_height, 0.1f, 100.0f);
	_view = _camera.getViewMatrix();

	/* For Each Object: Get Model Matrix and Shade */
	// Use Global Shader for Now as a Makeshift
	_global_shader->begin();

	_global_shader->setMat4f("view", glm::value_ptr(_view));
	_global_shader->setMat4f("projection", glm::value_ptr(_projection));
	for (auto& obj : _objects) {
		obj->draw(_global_shader);
	}

	_global_shader->end();

	_signature->render(_view, _projection);
}

void World::initObjects() {
	/* Load Shader& Model */
	_global_shader = std::make_shared<Shader>(
		"assets/shaders/loadobj_nolight.vert", 
		"assets/shaders/loadobj_nolight.frag"
	);

	// Makeshift: to seperate from _global_shader, 
	//		for mixed sampler2D texture_diffuse2 would be used by other Object if they don't have >1 diffuse_texture
	auto signature_shader = std::make_shared<Shader>(
		"assets/shaders/loadobj.vert",
		"assets/shaders/loadobj_nolight.frag"
	); 

	auto ground = std::make_shared<Model>("assets/objects/ground/ground.obj");
	auto avatar = std::make_shared<Model>("assets/objects/darknight/darknight.fbx");
	
	auto barrel = std::make_shared<Model>("assets/objects/barrel/Barrel.obj");
	auto box = std::make_shared<Model>("assets/objects/box/box_resize.obj");
	auto barrels = std::make_shared<Model>("assets/objects/barrelpack/barrels_packed.obj");
	auto knight = std::make_shared<Model>("assets/objects/knightguard/KnightGuard-nospear.fbx");
	auto hawk = std::make_shared<Model>("assets/objects/hawk/SparrowHawk.fbx");

	auto signature = std::make_shared<Model>("assets/objects/signature/signature-tomix.obj");
	signature->loadMaterialTextureByHand("signature_diffuse2.png"); // Makeshift: for mixed texture
	signature->printTextureLoaded();

	/* Debug */
	// knight->printMesh();

	/* Create Ground */
	float ground_interval = 30.0f;
	for (int i = -1; i <= 1; i++) { // _objects[0 ~ 8]
		for (int j = -1; j <= 1; j++) {
			_objects.emplace_back(std::make_shared<Object>(_global_shader, ground, STATIC,
				glm::vec3(i * ground_interval, 0.0f, j * ground_interval)
			));
		}
	}
	
	/* Create Player */
	_player = std::make_shared<Player>(_global_shader, avatar, _border); // Must be Dynamic
	_objects.push_back(_player); // _objects[9]
	_camera.switchMode(_player);

	/* Create Objects */
	const int obj_begin = 10;
	const int barrel_num = 1, box_num = 3, barrelpack_num = 1, random_guard_num = 1;
	int random_pos_num = barrel_num + box_num + barrelpack_num + random_guard_num;
	std::vector<glm::vec3> random_positions = generateRandomPoints(random_pos_num, 1.5f, _border, 5.0f);
	std::cout << "[Randomly Generate Positions]" << std::endl;
	for (auto& pos : random_positions) {
		std::cout << "\t" << glm::to_string(pos) << std::endl;
	}

	// Interactable Barrel
	int barrel_end = 0 + barrel_num;
	for (int i = 0; i < barrel_end; i++) {
		_objects.emplace_back(std::make_shared<Object>(_global_shader, barrel, DYNAMIC,
			random_positions[i], 1.0f, glm::vec3(0.0f, -1.0f, 0.2f)
		));
	}
	
	// Grunge Box
	int box_end = barrel_end + box_num;
	for (int i = barrel_end; i < box_end; i++) {
		glm::vec3 position = random_positions[i]; position.y = 0.0f;
		_objects.emplace_back(std::make_shared<Object>(_global_shader, box, STATIC,
			position, 1.0f
		));
	}

	// Barrel Packs
	int barrelpack_end = box_end + barrelpack_num;
	for (int i = box_end; i < barrelpack_end; i++) {
		glm::vec3 position = random_positions[i]; position.y = 0.0f;
		_objects.emplace_back(std::make_shared<Object>(_global_shader, barrels, STATIC,
			position
		)); 
	}

	// Quad Guard: x 4
	for (uint8_t i = 0; i < 4; i++) {
		_objects.emplace_back(std::make_shared<Object>(_global_shader, knight, STATIC,
			glm::vec3(-21.0f + (i & 0b01) * 42.0f, 0.0f, -21.0f + (i >> 1) * 42.0f), 
			0.8f,
			glm::vec3(-1.0f + (i & 0b01) * 2.0f, 0.0f, -1.0f + (i >> 1) * 2.0f)
		));
	}
	glm::vec3 tmp_positon = random_positions[barrelpack_end]; tmp_positon.y = 0.0f;
	_objects.emplace_back(std::make_shared<Object>(_global_shader, knight, STATIC,
		tmp_positon, 0.8f, glm::vec3(0.0f, 0.0f, -1.0f)
	)); // Random Guard


	// Signature
	_signature = std::make_shared<Object>(signature_shader, signature, STATIC,
		glm::vec3(0.0f, 10.0f, -50.0f), 5.0f, glm::vec3(0.0f, 0.0f, -1.0f)
	);

	// Flying bird
	for (int i = 0; i < 2; i++) {
		auto random_flying_bird = std::make_shared<Object>(_global_shader, hawk, DYNAMIC,
			glm::vec3(-2.0f, 3.0f, 2.0f), 0.3f, glm::vec3(0.0f, 0.0f, -1.0f)
		);
		random_flying_bird->enableRandomMove(3.5f, 4.4f + i * 0.3);
		_objects.push_back(random_flying_bird);
	}
	

	/* init Objects Physics */
#ifdef PHYSIC_IMPL
	// Ground Plane
	physx::PxRigidStatic* ground_plane = mPhysics->createRigidStatic(physx::PxTransform(0.0f, -0.5f, 0.0f));
	{
		physx::PxShape* ground_shape = mPhysics->createShape(physx::PxBoxGeometry(50.0f, 0.5f, 50.0f), *mMaterial);
		ground_plane->attachShape(*ground_shape);
		ground_shape->release();
	}
	_objects[4]->rigid_static = ground_plane; // ground in _objects[4] is loacated in (0.0f, 0.0f, 0.0f) 
	mScene->addActor(*ground_plane);

	// Player
	auto player = _player->createRigidDynamic(mPhysics, *mCookingParams, mMaterial);
	mScene->addActor(*player);
	
	// Other Objects
	for (int i = 0; i < barrel_end; i++) {
		auto barrel_actor = _objects[obj_begin + i]->createRigidDynamic(mPhysics, *mCookingParams, mMaterial);
		barrel_actor->setMass(5.0f);
		barrel_actor->setCMassLocalPose(physx::PxTransform(0.0f, 0.7f, 0.0f));
		mScene->addActor(*barrel_actor);
	}

	for (int i = barrel_end; i < box_end; i++) {
		auto box_actor = _objects[obj_begin + i]->createRigidStatic(mPhysics, *mCookingParams, mMaterial);
		mScene->addActor(*box_actor);
	}
	
	//for (int i = box_end; i < barrelpack_end; i++) {
	//	auto barrelpack_actor = _objects[obj_begin + i]->createRigidStatic(mPhysics, *mCookingParams, mMaterial);
	//	mScene->addActor(*barrelpack_actor);
	//}

	//for (int i = 10; i < _objects.size(); i++) {/* Other Objects in Batch */}
#endif
}

void World::initPhysics() {
	std::cout << "Init PhysX" << std::endl;

	/* Initialize PhysX */
	// Create Foundation
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mAllocator, mErrorCallback);
	if (!mFoundation) throw("PxCreateFoundation failed!");

	// Create PVD
	//mPvd = PxCreatePvd(*mFoundation);
	//physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	//mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	// Creat Physics with Tolerance Scale
	// mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), true, mPvd); // Without
	mToleranceScale.length = 100; // typical length of an object
	mToleranceScale.speed = 981; // typical speed of an object, gravity*1s is a reasonable choice
	// mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale); // do not use PVD

	// Init PxCooking
	mCookingParams = new physx::PxCookingParams(mPhysics->getTolerancesScale());
	// mCookingParams->meshPreprocessParams |= physx::PxMeshPreprocessingFlag::eWELD_VERTICES

	// Creat SceneDesc
	physx::PxSceneDesc scene_desc(mPhysics->getTolerancesScale());
	scene_desc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

	// Create CPU Dispatcher
	mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	scene_desc.cpuDispatcher = mDispatcher;

	// Set Filter Shader
	scene_desc.filterShader = MyFilterShader;

	// Create Scene
	mScene = mPhysics->createScene(scene_desc);
	mCollisionCallback = new MyCollisionCallback();
	mScene->setSimulationEventCallback(mCollisionCallback);

	// Pvd Client
	//physx::PxPvdSceneClient* pvd_client = mScene->getScenePvdClient();
	//if (pvd_client) {
	//	pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//	pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//	pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//}

	/* Create Global Material */
	mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f); // static friction, dynamic friction, restitution
}

physx::PxFilterFlags MyFilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize) {
	// enable default flags
	pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
	// add touch notification
	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
	return physx::PxFilterFlag::eDEFAULT;
}


/*=== Interact ===*/
void World::processKeyboardPress() {
	if (is_wasd()) {
		_player->walk();
	}

	// Debug: report postion
	if (keyboard[GLFW_KEY_SPACE]) {
		std::cout << "[Player Position] " << glm::to_string(_player->getPosition()) << std::endl;
	}
	// Switch
	if (keyboard[GLFW_KEY_C]) _camera.switchMode(_player);
}

// Attension: This function is called successively every window refresh, not keyboard callback
void World::processKeyboardInput() {
	if (is_wasd()) {
		_camera.processKeyboard(_delta_time);
		_player->processKeyboard(_delta_time);
	}
}

void World::processKeyboardRelease() {
	if (!is_wasd()) {
		_player->idle();
	}
}

void World::processMouseMovement(float xoffset, float yoffset) {
	_camera.processMouseMovement(xoffset, yoffset, true);
}

void World::processMouseScroll(float yoffset) {
	_camera.processMouseScroll(yoffset);
}