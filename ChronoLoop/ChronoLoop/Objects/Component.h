#pragma once
#include "..\Common\Math.h"
#include "..\Rendering\Mesh.h"
class BaseObject;
//class Mesh;

enum ComponentType
{
	eCOMPONENT_UNKNOWN = 0,
	eCOMPONENT_CODE,
	eCOMPONENT_AUDIOEMITTER,
	eCOMPONENT_AUDIOLISTENER,
	eCOMPONENT_COLLIDER,
	eCOMPONENT_UI,
	eCOMPONENT_MESH,
	eCOMPONENT_MAX
};

class Component
{
	friend class Physics;
	friend class BaseObject;
protected:
	bool mIsEnabled;
	ComponentType mType = eCOMPONENT_MAX;
	BaseObject* mObject = nullptr;
public:
	ComponentType GetType() { return mType; };
	bool isEnabled() { return mIsEnabled; };
	void Disable() { mIsEnabled = false; };
	void Enable() { mIsEnabled = true; };
	virtual void Update() = 0;
	virtual void Destroy() = 0;
	void GetMatrix(matrix4& _m);
};

class Listener : Component
{
public:
	void Update() {}
	void Destroy() {}
};

class Emitter : Component
{
public:
	void Play();
	void Pause();
	void Stop();

	void Update();
	void Destroy();
};

namespace RenderEngine {
	struct RenderShape;
}

class MeshComponent : public Component {
	RenderEngine::RenderShape* mShape;
	bool mVisible;

public:
	MeshComponent(const char *_path);
	void Update();
	void Destroy();
	void SetVisible(bool _vis);
};

struct Action;
class CodeComponent : public Component {
	Action* mAction = nullptr;
public:
	// TODO Add a destructor
	CodeComponent(Action* _action);
	void Update();
	void Destroy();
};

class Collider : public Component {
public:
	enum ColliderType {
		eCOLLIDER_Mesh,
		eCOLLIDER_Sphere,
		eCOLLIDER_Cube,
		eCOLLIDER_Plane
	};

	bool mShouldMove, mColliding;
	vec4f mVelocity, mAcceleration, mTotalForce, mForces, mImpulsiveForce, mGravity;
	float mMass, mElasticity, mFriction;
	ColliderType mColliderType;

	void Update();
	void Destroy();

	vec4f AddForce(vec4f _force) { mShouldMove = true; mForces += _force; return mTotalForce; };
	virtual vec4f GetPos();
	virtual void SetPos(vec4f _newPos);
};

class MeshCollider : public Collider {
public:
	MeshCollider(bool _move, vec4f _gravity, float _mass, float _elasticity, float _friction, char* _path);
	Mesh* mMesh;
};

class SphereCollider : public Collider {
public:
	SphereCollider(bool _move, vec4f _gravity, float _mass, float _elasticity, float _friction, float _radius);
	float mRadius;
};

class CubeCollider : public Collider {
public:
	CubeCollider(bool _move, vec4f _gravity, float _mass, float _elasticity, float _friction, vec4f _min, vec4f _max);
	vec4f mMin, mMax, mMinOffset, mMaxOffset;

	void SetPos(vec4f _newPos);
};

class PlaneCollider : public Collider {
public:
	PlaneCollider(bool _move, vec4f _gravity, float _mass, float _elasticity, float _friction, float _offset, vec4f _norm);
	vec4f mNormal;
	float mOffset;
};

/*
business entity- gmail, twitter, facebook, steam account
art, audio, marketing, designer students ?

first initial last name, password lower case
gdserv.fullsail.com:8080
install doc, follow it
*/