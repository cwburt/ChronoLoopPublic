#pragma once
#include "../Objects/BaseObject.h"
#include "CodeComponent.hpp"
#include "../Common/Logger.h"

struct CCElasticAABBtoAABB : public CodeComponent
{
	vec4f norm = { 0,0,0,0 };
	bool colliding = false;
	virtual void OnCollision(Collider& _col, Collider& _other, float _time)
	{
		if (!colliding && _other.mColliderType == Collider::eCOLLIDER_Cube)
		{
			colliding = true;

			vec4f max = ((CubeCollider*)&_other)->mMax;
			vec4f min = ((CubeCollider*)&_other)->mMin;
			vec4f otherCenter = (max + min) * 0.5f;
			vec4f center = _col.GetPos();
			vec4f V = center - otherCenter;

			//vec4f norm(0,0,0,0);
			if (center.y >= max.y)
				norm = { 0,1,0,0 };
			else if (center.y <= min.y)
				norm = { 0,-1,0,0 };
			else if ((center.x <= max.x && center.x >= min.x) && V.z > 0)
				norm = { 0,0,1,0 };
			else if ((center.x <= max.x && center.x >= min.x) && V.z < 0)
				norm = { 0,0,-1,0 };
			else if ((center.z <= max.z && center.z >= min.z) && V.x > 0)
				norm = { 1,0,0,0 };
			else if ((center.z <= max.z && center.z >= min.z) && V.x < 0)
				norm = { -1,0,0,0 };
			

			//float avgElasticity = (_col.mElasticity + _other.mElasticity) / 2;
			//_col.mVelocity = norm * (1 + avgElasticity);

			//No velocivty in the collision normal
			float flip = -_col.mVelocity * norm;
			vec4f prev = _col.mVelocity;
			prev += norm * flip;
			_col.mVelocity = { 0,0,0,1 };
			float avgElasticity = (_col.mElasticity + _other.mElasticity) / 2;
			_col.mVelocity = norm * (1 + avgElasticity) + prev;

			//float flip = -_col.mVelocity * norm;
			//vec4f prev = _col.mVelocity;
			//prev += norm * flip;
			//_col.mVelocity = { 0,0,0,1 };
			//float avgElasticity = (_col.mElasticity + _other.mElasticity) / 2;
			//_col.mVelocity = norm * (flip * (1 - avgElasticity)) + prev;
		}
		else
			colliding = false;
	}
};