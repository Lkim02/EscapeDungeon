
#include "Animation.h"
#include "assimp2glm.h"

// TODO: I want it to be a member of Model, rather than seperate from each other

Animation::Animation(const std::string& animation_path, Model& model) {
    // TODO: this is silly! try to reuse Model::setup()
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animation_path, aiProcess_Triangulate);
    assert(scene && scene->mRootNode);

    // Get Animation Data
    aiAnimation* animation = scene->mAnimations[0];
    _duration = animation->mDuration;
    _ticks_per_second = animation->mTicksPerSecond;
    _root = traverseAiNodeRecursive(scene->mRootNode);
    readBonesInvolved(animation, model);
}

Animation::Animation(const aiScene* scene, Model& model) {
    // Get Animation Data
    aiAnimation* animation = scene->mAnimations[0];
    _duration = animation->mDuration;
    _ticks_per_second = animation->mTicksPerSecond;
    _root = traverseAiNodeRecursive(scene->mRootNode); // TODO: also silly, can mixed in Model
    readBonesInvolved(animation, model);
}

Bone* Animation::findBone(const std::string& name) {
    auto it = std::find_if(_bones.begin(), _bones.end(), [&](const Bone& bone) {
        return bone.getBoneName() == name;
    });
    if (it == _bones.end()) return nullptr;
    else return &(*it);
}


AssimpNodeData Animation::traverseAiNodeRecursive(const aiNode* src) {
    AssimpNodeData new_node(
        src->mName.data,
        AssimpGLMHelpers::toGLMMat4(src->mTransformation),
        src->mNumChildren
    );
    for (int i = 0; i < src->mNumChildren; i++) {
        new_node.children.emplace_back(traverseAiNodeRecursive(src->mChildren[i]));
    }
    return new_node;
}

void Animation::readBonesInvolved(const aiAnimation* animation, Model& model) {
    auto& boneinfo_map = model.getBoneInfoMap();
    int& bone_cnt = model.getBoneCnt();

    // Get Bones that are Engaged in the Animation
    int size = animation->mNumChannels;
    for (int i = 0; i < size; i++) {
        auto channel = animation->mChannels[i];
        std::string bone_name = channel->mNodeName.data;
        // Also, add new boneinfo, for it could be missed in Model's meshprocessing
        if (boneinfo_map.find(bone_name) == boneinfo_map.end()) {
            boneinfo_map[bone_name].id = bone_cnt;
            bone_cnt++;
        }
        _bones.push_back(Bone(channel->mNodeName.data, boneinfo_map[channel->mNodeName.data].id, channel));
    }

    _boneinfo_map = boneinfo_map;
}