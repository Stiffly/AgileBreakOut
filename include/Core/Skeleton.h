/*
	This file is part of Daydream Engine.
	Copyright 2014 Adam Byléhn, Tobias Dahl, Simon Holmberg, Viktor Ljung
	
	Daydream Engine is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	Daydream Engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with Daydream Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef Skeleton_h__
#define Skeleton_h__

#include <sstream>

namespace dd
{

//struct Bone
//{
//	Bone(std::string name, glm::mat4 offsetMatrix)
//		: Name(name)
//		, OffsetMatrix(offsetMatrix)
//	{ }
//
//	~Bone()
//	{
//		for (auto kv : Children) {
//			delete kv.second;
//		}
//	}
//
//	std::string Name;
//	glm::mat4 OffsetMatrix;
//	glm::mat4 LocalMatrix;
//
//	std::map<std::string, Bone*> Children;
//};

class Skeleton
{
public:
	struct Bone
	{
		Bone(int id, Bone* parent, std::string name, glm::mat4 offsetMatrix)
			: ID(id)
			, Parent(parent)
			, Name(name)
			, OffsetMatrix(offsetMatrix)
		{ }

		int ID;
		std::string Name;
		glm::mat4 OffsetMatrix;

		Bone* Parent;
		std::vector<Bone*> Children;
	};

	struct Animation
	{
		struct Keyframe
		{
			struct BoneProperty
			{
				int ID;
				glm::vec3 Position;
				glm::quat Rotation;
				glm::vec3 Scale = glm::vec3(1);
			};

			int Index = 0;
			double Time = 0.0;
			std::map<int, Keyframe::BoneProperty> BoneProperties;
		};

		std::string Name;
		double Duration;
		std::vector<Keyframe> Keyframes;
	};

	Skeleton() { }
	~Skeleton();

	Bone* RootBone;

	std::map<int, Bone*> Bones;

	// Attach a new bone to the skeleton
	// Returns: New bone index
	int CreateBone(int ID, int parentID, std::string name, glm::mat4 offsetMatrix);

	int GetBoneID(std::string name);

	const Animation* GetAnimation(std::string name);
	std::vector<glm::mat4> GetFrameBones(const Animation& animation, double time, bool noRootMotion = false);
	void AccumulateBoneTransforms(bool noRootMotion, const Animation::Keyframe& currentFrame, const Animation::Keyframe& nextFrame, float progress, std::map<int, glm::mat4>& boneMatrices, const Bone* bone, glm::mat4 parentMatrix);
	void PrintSkeleton();
	void PrintSkeleton(const Bone* parent, int depthCount);
	std::map<std::string, Animation> Animations;

private:
	std::map<std::string, Bone*> m_BonesByName;

	int GetKeyframe(const Animation& animation, double time);
};

}

#endif // Skeleton_h__
