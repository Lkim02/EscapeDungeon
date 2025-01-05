
#include "Animator.h"
#include "Animation.h"

#include <cmath>

Animator::Animator(std::shared_ptr<Animation> animation) {
	_current_time = 0.0f;
	_current_animation = animation;
	
	_final_bone_matrices.reserve(ANIMATION_MAX_BONES);
	for (int i = 0; i < ANIMATION_MAX_BONES; i++) {
		_final_bone_matrices.emplace_back(glm::mat4(1.0f));
	}
}

void Animator::init(std::shared_ptr<Animation> played_animation) {
	_current_animation = played_animation;
	_current_time = 0.0f;
}

void Animator::update(float delta_time) {
	_delta_time = delta_time;
	if (_current_animation) {
		_current_time += _current_animation->getTicksPerSecond() * _delta_time;
		_current_time = fmod(_current_time, _current_animation->getDuration());
		calculateBoneTransformRecursive(&_current_animation->getRootNode(), glm::mat4(1.0f));
	}
}

void Animator::reset() {
	if (_current_animation) {
		_current_time = 0.0f;
		calculateBoneTransformRecursive(&_current_animation->getRootNode(), glm::mat4(1.0f));
	}
}

void Animator::calculateBoneTransformRecursive(const AssimpNodeData* node, glm::mat4 parent_transform) {
	const std::string& node_name = node->name;
	glm::mat4 node_transform = node->transform; // init

	// Search for Bones Involved and Exert Transformation
	Bone* bone = _current_animation->findBone(node_name);
	if (bone) {
		bone->update(_current_time);
		node_transform = bone->getLocalTransform();
	}

	glm::mat4 global_transform = parent_transform * node_transform;

	// Add Offset Matrix of Each Bone
	auto& boneinfo_map = _current_animation->getBoneInfoMap();
	auto it = boneinfo_map.find(node_name);
	if (it != boneinfo_map.end()) {
		const BoneInfo& boneinfo = it->second;
		_final_bone_matrices[boneinfo.id] = global_transform * boneinfo.offset;
	}

	// Recursive update
	for (int i = 0; i < node->child_num; i++) {
		calculateBoneTransformRecursive(&node->children[i], global_transform);
	}
}

