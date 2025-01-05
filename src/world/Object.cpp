
#include "Object.h"

#include <vector>

// Can be Redundant and Silly
Object::Object(std::shared_ptr<Shader> shader, std::shared_ptr<Model> model, glm::mat4 raw_model_matrix, PXType px_type, 
		float scale)
		: _model(model), _shader(shader), px_type(px_type),
		  _scale(scale) {

	_model_matrix = glm::scale(raw_model_matrix, glm::vec3(_scale));
	updateNormModelMatrix();

	/* Calculate Details */
	_position = glm::vec3(_model_matrix[3][0], _model_matrix[3][1], _model_matrix[3][2]); // TODO: silly
	glm::mat3 rotate_mat = glm::mat3(raw_model_matrix); // TODO: up may be conflict with world up
	_rotation = glm::quat_cast(rotate_mat);
	
	/* Set Animator */
	animator = std::make_shared<Animator>(model->animation);
}

// Has Defect: need to use quat to eliminate
Object::Object(std::shared_ptr<Shader> shader, std::shared_ptr<Model> model, PXType px_type,
		glm::vec3 position, float scale, glm::vec3 front, glm::vec3 world_up)
		: _model(model), _shader(shader),
		  _position(position), _scale(scale), _front(front), _world_up(world_up) {
	
	// Never use this again in the following calculation, using Quat instead
	_front = glm::normalize(front); // assume world_up to be normalized
	_right = glm::normalize(glm::cross(_front, _world_up));
	_up = glm::normalize(glm::cross(_right, _front));

	/* Calculate Matrices: temporarily */
	_model_matrix = createModelMatrix(_position, _front, _up, _right, glm::vec3(_scale));
	updateNormModelMatrix();
	_rotation = glm::quat_cast(glm::mat3(_right, _up, _front));

	/* Set Animator */
	animator = std::make_shared<Animator>(model->animation);
}


void Object::draw(std::shared_ptr<Shader>& shader) {
	/* Implement Model Matrix(&Normal Model Matrix) */
	shader->setMat4f("model", glm::value_ptr(_model_matrix));
	//shader->setMat4f("normModel", glm::value_ptr(_norm_model_matrix));

	/* Implement Animation Bone Matrices */
	const std::vector<glm::mat4>& bone_transforms = animator->getFinalBoneMatrices();
	for (int i = 0; i < bone_transforms.size(); ++i) {
		shader->setMat4f("finalBonesMatrices[" + std::to_string(i) + "]", glm::value_ptr(bone_transforms[i]));
	}

	_model->draw(shader.get());
}

void Object::start() {
	if (px_type == DYNAMIC) {
		std::cout << "object: enable gravity" << std::endl;
		rigid_dynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
	}
}

void Object::update(float _delta_time) {
	if (_random_move) {
		updateRandomMove(_delta_time);
	}
	animator->update(_delta_time);
}

void Object::render(glm::vec3& view, glm::vec3& projection) { // DO NOT USE
	_shader->begin();

	// TODO: whether to render here or out in the world
	_model->draw(_shader.get());

	_shader->end();
}


/* Physics */

// private
void Object::cookAndCreateTriangleMesh(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams) {
	const auto& vertices = _model->px_combined_vertices;
	const auto& indices = _model->px_combined_indices;

	physx::PxTriangleMeshDesc mesh_desc;
	mesh_desc.points.count = vertices.size();
	mesh_desc.points.stride = sizeof(physx::PxVec3);
	mesh_desc.points.data = vertices.data();
	mesh_desc.triangles.count = indices.size() / 3;
	mesh_desc.triangles.stride = 3 * sizeof(physx::PxU32);
	mesh_desc.triangles.data = indices.data();

	physx::PxDefaultMemoryOutputStream write_buffer;
	if (!PxCookTriangleMesh(cookingParams, mesh_desc, write_buffer)) {
		std::cout << "Failed to cook convex mesh." << std::endl;
		return;
	}

	physx::PxDefaultMemoryInputData read_buffer(write_buffer.getData(), write_buffer.getSize());
	px_triangle_mesh = physics->createTriangleMesh(read_buffer);
}

// public
physx::PxRigidStatic* Object::createRigidStatic(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams, physx::PxMaterial* material) {
	cookAndCreateTriangleMesh(physics, cookingParams);

	px_type = STATIC;
	_px_transform = physx::PxTransform(
		physx::PxVec3(_position.x, _position.y, _position.z),
		physx::PxQuat(_rotation.x, _rotation.y, _rotation.z, _rotation.w)
	);
	_px_transform.p += physx::PxVec3(0.0f, 2.0, 0.0f);
	rigid_static = physics->createRigidStatic(_px_transform);
	{
		//physx::PxShape* shape = physics->createShape(physx::PxTriangleMeshGeometry(px_triangle_mesh), *material);
		physx::PxShape* shape = physics->createShape(physx::PxBoxGeometry(1.0f, 2.0, 1.0f), *material);
		
		shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
		shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
		physx::PxFilterData filterData;
		filterData.word0 = 1; filterData.word1 = 1;
		shape->setSimulationFilterData(filterData);

		rigid_static->attachShape(*shape);
		shape->release();
	}
	return rigid_static;
}

physx::PxRigidDynamic* Object::createRigidDynamic(physx::PxPhysics* physics, physx::PxCookingParams& cookingParams, physx::PxMaterial* material) {
	cookAndCreateTriangleMesh(physics, cookingParams);

	px_type = DYNAMIC;
	_px_transform = physx::PxTransform(
		physx::PxVec3(_position.x, _position.y, _position.z),
		physx::PxQuat(_rotation.x, _rotation.y, _rotation.z, _rotation.w)
	);
	rigid_dynamic = physics->createRigidDynamic(_px_transform);
	{
		physx::PxShape* shape = physics->createShape(physx::PxTriangleMeshGeometry(px_triangle_mesh), *material);
		rigid_dynamic->attachShape(*shape);
		shape->release();
	}
	rigid_dynamic->setMass(10.0f);
	rigid_dynamic->setLinearDamping(0.01f);
	rigid_dynamic->setAngularDamping(0.5f);
	rigid_dynamic->setSleepThreshold(0.05f);
	return rigid_dynamic;
}

void Object::updateSimulateResult() {
	if (px_type != DYNAMIC) return;
	
	// update PxTransform
	physx::PxTransform px_new_transform = rigid_dynamic->getGlobalPose();
	if (px_new_transform == _px_transform) return;

#ifdef DEBUG
	//std::cout << "[Player][prev px_transform]" << std::endl;
	//printPxTransform(_px_transform);
	//std::cout << "[Player][new  px_transform]" << std::endl;
	//printPxTransform(px_new_transform);
#endif

	_px_transform = px_new_transform;
	// convert to glm positon and rotate
	_position = glm::vec3(_px_transform.p.x, _px_transform.p.y, _px_transform.p.z);
	_rotation = glm::quat(_px_transform.q.w, _px_transform.q.x, _px_transform.q.y, _px_transform.q.z);
	// finally update model matrix and norm model matrix
	_model_matrix = createModelMatrix(_position, _rotation, glm::vec3(_scale));
	updateNormModelMatrix();
}


/* Special */ // TODO: Move in to new Derived Class
void Object::enableRandomMove(float height, float speed) {
	_random_height = height;
	_random_move_speed = speed;
	generateAndSetRandomTrack();
	_random_move = true;
}

void Object::generateAndSetRandomTrack() {
	// Need to Start/End out of World View Border */
	_random_from = generateRandomPoints(1, 35.0f, 45.0f, 0.0f)[0];
	_random_to = -_random_from;

	// Correct Height
	_random_from.y = _random_to.y = _random_height;
	std::cout << this << " [New Random Move]";
	std::cout << " from " << glm::to_string(_random_from) << " to " << glm::to_string(_random_to) << std::endl;
	
	// Reset
	_position = _random_from; _up = glm::vec3(0.0f, 1.0f, 0.0f);
	_front = glm::normalize(_random_to - _random_from); 
	_right = glm::cross(_front, _up);
	updateModelMatrixThroughEuler();
}

void Object::updateRandomMove(float delta_time) {
	float velocity = _random_move_speed * delta_time;
	if (glm::distance(_position, _random_to) < velocity) {
		generateAndSetRandomTrack();
		return;
	}
	_position += _front * velocity;
	//std::cout << "update position: " << glm::to_string(_position) << std::endl;
	updateModelMatrixThroughEuler();
}