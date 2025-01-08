#ifndef ANIMATION_H
#define ANIMATION_H

#include "common.h"
#include "model/Bone.h"
#include "model/Model.h"

#include <vector>
#include <string>
#include <map>

struct AssimpNodeData {
	std::string name;
	glm::mat4 transform;
	
	std::vector<AssimpNodeData> children;
	int child_num = 0;

	AssimpNodeData() = default;
	AssimpNodeData(const std::string& name, const glm::mat4& transform, int child_num)
		:transform(transform), name(name), child_num(child_num) {}
};


class Animation {
public:
	Animation() = default;
	Animation(const std::string& animation_path, Model& model);
	Animation(const aiScene* scene, Model& model);
	~Animation() = default;

	Bone* findBone(const std::string& name);

	inline float getDuration() { return _duration; }
	inline float getTicksPerSecond() { return _ticks_per_second; }
	inline const auto& getRootNode() { return _root; }
	inline const auto& getBoneInfoMap() { return _boneinfo_map; }

private:
	float _duration;
	int _ticks_per_second;
	std::vector<Bone> _bones; // bones that are engaged in the animation
	AssimpNodeData _root; // TODO: maybe pointer is better, but be careful about memory leak
	std::map<std::string, BoneInfo> _boneinfo_map;

	AssimpNodeData traverseAiNodeRecursive(const aiNode* src);
	void readBonesInvolved(const aiAnimation* animation, Model& model);
};

#endif // !ANIMATION_H
