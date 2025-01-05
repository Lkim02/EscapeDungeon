#ifndef OBJECT_H
#define OBJECT_H

#include "common.h"
#include "shader/Shader.h"
#include "model/Model.h"
#include "animation/Animator.h"
#include "utils.h"

#include <string>


class Object {
public:
	Object() {}
	Object(std::shared_ptr<Shader> shader, std::shared_ptr<Model> model, glm::mat4 raw_model_matrix, PXType px_type,
		float scale = 1.0f);
	Object(std::shared_ptr<Shader> shader, std::shared_ptr<Model> model, PXType px_type,
		glm::vec3 position = glm::vec3(0.0f), float scale = 1.0f,
		glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f), 
		glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f));
	~Object() {
		// mScene would have release this (make sure attached to mScene)
		//if (rigid_static) { rigid_static->release(); std::cout << "Cleared rigid_static" << std::endl; }
		//if (rigid_dynamic) { rigid_dynamic->release(); std::cout << "Cleared rigid_dynamic" << std::endl; }
		//if (px_triangle_mesh) { px_triangle_mesh->release(); std::cout << "Cleared px_triangle_mesh" << std::endl; }
	}

	void render(glm::vec3& view, glm::vec3& projection); // TODO: whether render here or out
	void draw(std::shared_ptr<Shader>& shader);
	void start();
	virtual void update(float _delta_time);

	void setModelMatrix(glm::mat4 model) { 
		_model_matrix = model;
		updateNormModelMatrix();
	}
	glm::mat4 getModelMatrix() const { return _model_matrix; }
	glm::mat3 getNormModelMatrix() const { return _norm_model_matrix; }
	glm::vec3 getPosition() const { return _position; }

	/* Physics */
	// PxActor
	//	©¸©¤©¤ PxRigidActor
	//		©À©¤©¤ PxRigidStatic
	//		©¸©¤©¤ PxRigidBody
	//				©À©¤©¤ PxRigidDynamic
	//				©¸©¤©¤ PxArticulationLink
	// physx::PxActorType::Enum px_atype = physx::PxActorType::Enum::eINVALID;
	// physx::PxActor* rigid_actor = nullptr;
	PXType px_type = NONE;
	physx::PxRigidStatic* rigid_static = nullptr;
	physx::PxRigidDynamic* rigid_dynamic = nullptr;
	physx::PxRigidStatic* createRigidStatic(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams, physx::PxMaterial* material);
	virtual physx::PxRigidDynamic* createRigidDynamic(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams, physx::PxMaterial* material);
	virtual void updateSimulateResult();

	/* Animator: Drive Animation */
	std::shared_ptr<Animator> animator = nullptr;

	/* Interact */
	virtual void processKeyboard(float delta_time) {}
	virtual void processMouseMovement(float xoffset, float yoffset) {}
	virtual void processMouseScroll(float yoffset) {}

	/* Special: Decorating */
	void enableRandomMove(float height, float speed);

	/* Camera Mode */
	virtual void lock() {}
	virtual void unlock() {}

protected:
	std::shared_ptr<Shader> _shader = nullptr; // shared
	std::shared_ptr<Model> _model = nullptr;   // shared
	
	/* Object Pose */
	glm::mat4 _model_matrix;
	glm::mat3 _norm_model_matrix;
	// Model Matrix in Details
	glm::vec3 _position;
	glm::vec3 _world_up;
	glm::vec3 _front, _right, _up;
	float _scale = 1.0f; // TODO: do not use
	glm::quat _rotation;
	// For Physics Transform
	physx::PxTransform _px_transform;
	

	void updateModelMatrixThroughEuler() { // TODO: Not a Good One, Unify to Quat
		// assume front.y != 1.0f/-1.0f
		_right = glm::cross(_front, _up); 
		_model_matrix = createModelMatrix(_position, _front, _up, _right, glm::vec3(_scale));
		updateNormModelMatrix();
	}
	inline void updateNormModelMatrix() {
		_norm_model_matrix = glm::transpose(glm::inverse(glm::mat3(_model_matrix)));
	}

	/* Physics */
	physx::PxTriangleMesh* px_triangle_mesh = nullptr;
	void cookAndCreateTriangleMesh(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams);

	/* Special */
	bool _random_move = false;
	float  _random_move_speed = 4.0f, _random_height = 3.0f;
	glm::vec3 _random_from, _random_to;
	void generateAndSetRandomTrack();
	void updateRandomMove(float delta_time);

};


#endif // !OBJECT_H
