
#include "Bone.h"
#include "assimp2glm.h"

Bone::Bone(const std::string& name, int id, const aiNodeAnim* channel) {
	_name = name;
	_id = id;

	// Load Position Key
	_pos_num = channel->mNumPositionKeys;
	for (int i = 0; i < _pos_num; i++) {
		auto key = channel->mPositionKeys[i];

		KeyPosition data;
		data.position = AssimpGLMHelpers::toGLMVec3(key.mValue);
		data.timestamp = key.mTime;
		_positions.push_back(data);
	}

	// Load Rotation Key
	_rot_num = channel->mNumRotationKeys;
	for (int i = 0; i < _rot_num; i++) {
		auto key = channel->mRotationKeys[i];

		KeyRotation rot_key;
		rot_key.rotation = AssimpGLMHelpers::toGLMQuat(key.mValue);
		rot_key.timestamp = key.mTime;
		_rotations.push_back(rot_key);
	}

	// Load Scale Key
	_scl_num = channel->mNumScalingKeys;
	for (int i = 0; i < _scl_num; i++) {
		auto key = channel->mScalingKeys[i];

		KeyScale scl_key;
		scl_key.scale = AssimpGLMHelpers::toGLMVec3(key.mValue);
		scl_key.timestamp = key.mTime;
		_scales.push_back(scl_key);
	}
}

void Bone::update(float animation_time) {
	glm::mat4 translation = interpolatePosition(animation_time);
	glm::mat4 rotation = interpolateRotation(animation_time);
	glm::mat4 scale = interpolateScaling(animation_time);
	_local_transform = translation * rotation * scale;
}

inline float Bone::getScaleFactor(float last_timestamp, float next_timestamp, float animation_time) {
	return (animation_time - last_timestamp) / (next_timestamp - last_timestamp);
}

glm::mat4 Bone::interpolatePosition(float animation_time) {
	if (_pos_num == 1) return glm::translate(glm::mat4(1.0f), _positions[0].position);

	int p0_index = getPositionIndex(animation_time);
	int p1_index = p0_index + 1;
	float scale_factor = getScaleFactor(
		_positions[p0_index].timestamp,
		_positions[p1_index].timestamp,
		animation_time
	);
	glm::vec3 final_position = glm::mix(
		_positions[p0_index].position,
		_positions[p1_index].position,
		scale_factor
	);
	return glm::translate(glm::mat4(1.0f), final_position);
}

glm::mat4 Bone::interpolateRotation(float animation_time) {
	if (_rot_num == 1) return glm::toMat4(glm::normalize(_rotations[0].rotation));

	int p0_index = getRotationIndex(animation_time);
	int p1_index = p0_index + 1;
	float scale_factor = getScaleFactor(
		_rotations[p0_index].timestamp,
		_rotations[p1_index].timestamp,
		animation_time
	);
	glm::quat final_rotation = glm::slerp(
		_rotations[p0_index].rotation,
		_rotations[p1_index].rotation,
		scale_factor
	);
	return glm::toMat4(glm::normalize(final_rotation));
}

glm::mat4 Bone::interpolateScaling(float animation_time) {
	if (_scl_num == 1) return glm::scale(glm::mat4(1.0f), _scales[0].scale);

	int p0_index = getScaleIndex(animation_time);
	int p1_index = p0_index + 1;
	float scale_factor = getScaleFactor(
		_scales[p0_index].timestamp,
		_scales[p1_index].timestamp,
		animation_time
	);
	glm::vec3 final_scale = glm::mix(
		_scales[p0_index].scale,
		_scales[p1_index].scale,
		scale_factor
	);
	return glm::scale(glm::mat4(1.0f), final_scale);
}