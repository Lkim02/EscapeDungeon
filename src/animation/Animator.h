#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "common.h"
#include "Animation.h"

#include <vector>


#define ANIMATION_MAX_BONES 100

class Animator {
public:
	Animator(std::shared_ptr<Animation> animation);
	~Animator() = default;

	void init(std::shared_ptr<Animation> played_animation);
	void update(float delta_time);
	void reset();

	void calculateBoneTransformRecursive(const AssimpNodeData* node, glm::mat4 parent_transform);
	
	const std::vector<glm::mat4>& getFinalBoneMatrices() { return _final_bone_matrices; }

private:
	std::vector<glm::mat4> _final_bone_matrices;
	std::shared_ptr<Animation> _current_animation = nullptr;
	float _current_time = 0.0f;
	float _delta_time = 0.0f;
};

#endif // !ANIMATOR_H
