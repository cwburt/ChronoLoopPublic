#pragma once
#include "..\Common\Math.h"
#include "..\Rendering\Mesh.h"
#include <unordered_map>
#include "..\Physics\Physics.h"
#include <unordered_set>

namespace Epoch
{

	class BaseObject;
	class Transform;
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

		static unsigned short mComponentCount;
		unsigned short mComponentId;	//unique component id
		unsigned short mComponentNum;  //the nth number component of a base object. This is for knowing the position in the bitset

	protected:
		bool mDestroyed = false;
		bool mIsEnabled = true, mIsValid = true;
		ComponentType mType = eCOMPONENT_MAX;
		BaseObject* mObject = nullptr;
	public:
		Component();
		Component(ComponentType _cType);
		virtual ~Component();
		inline ComponentType GetType() { return mType; };
		inline bool IsEnabled() { return mIsEnabled; };
		inline bool IsValid() { return mIsValid; }
		inline void Disable() { mIsEnabled = false; };
		inline void Enable() { mIsEnabled = true; };
		unsigned short GetComponentNum() { return mComponentNum; }
		void GetMatrix(matrix4& _m);
		unsigned short GetColliderId() { return mComponentId; };
		Transform& GetTransform();
		Transform& GetTransform() const;

		inline BaseObject* GetBaseObject() {
			return mObject;
		}

		//Dont call this unless you are absolutly sure you know what you are doing
		inline void SetComponentId(unsigned short _id) { mComponentId = _id; }

		// Virtual functions for child classes
		virtual void Update() = 0;
		virtual void Destroy() = 0;
		virtual void OnAddedToObject() {}
	};

	class Listener : public Component
	{
	public:
		Listener() : Component(ComponentType::eCOMPONENT_AUDIOLISTENER) {}
		void Update() {}
		void Destroy() {}
	};

	class Emitter :public Component
	{
	public:
		enum sfxTypes { ePlayLoop, ePauseLoop, eResumeLoop, eStopLoop, ePlaySFX };

		Emitter() : Component(ComponentType::eCOMPONENT_AUDIOEMITTER)
		{
			mIsPaused = mIsPlaying = false;
		}

		void Play(int _id = 0);
		void Pause(int _id = 0);
		void Stop(int _id = 0);
		void PlaySFX(int _id = 0);
		void PlaySFX(int _id = 0, const vec4f* _pos = new vec4f());
		void AddSoundEvent(sfxTypes _type, int64_t _event);

		void Update();
		void Destroy();
	private:
		bool mIsPlaying = false, mIsPaused = false;
		std::unordered_map<sfxTypes, std::vector<int64_t>> mSFX;

	};

	class Collider : public Component
	{
	public:
		enum ColliderType
		{
			eCOLLIDER_Mesh,
			eCOLLIDER_Sphere,
			eCOLLIDER_Cube,
			eCOLLIDER_Plane,
			eCOLLIDER_Button,
			eCOLLIDER_Controller,
			eCOLLIDER_OrientedCube
		};

		ColliderType mColliderType;
		bool mShouldMove, mIsTrigger;
		vec4f mVelocity, mAcceleration, mTotalForce, mForces, mImpulsiveForce, mGravity, mWeight, mDragForce;
		float mMass, mElasticity, mKineticFriction, mStaticFriction, mInvMass, mRHO, mDrag, mArea;

		void Update();
		void Destroy();

		vec4f AddForce(vec4f _force) { mForces = _force; return mForces; };
		virtual vec4f GetPos();
		virtual void SetPos(const vec4f& _newPos);
	};

	//class MeshCollider : public Collider
	//{
	//public:
	//	MeshCollider() {}
	//	MeshCollider(BaseObject* _obj, bool _move, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, char* _path);
	//	Mesh* mMesh;
	//};

	class SphereCollider : public Collider
	{
	public:
		SphereCollider() {}
		SphereCollider(BaseObject* _obj, bool _move, bool _trigger, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, float _radius);
		SphereCollider(vec4f _pos, float _rad) { mCenter = _pos; mRadius = _rad; };
		vec4f mCenter;
		float mRadius;
		virtual void SetPos(const vec4f& _other);
	};

	class PlaneCollider : public Collider
	{
	public:
		PlaneCollider() {}
		PlaneCollider(BaseObject* _obj, bool _move, bool _trigger, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, float _offset, vec4f _norm);
		PlaneCollider(vec4f _norm, float _offset) { mNormal = _norm; mOffset = _offset; };
		vec4f mNormal, mMin, mMax;
		float mOffset;
	};

	class Frustrum : public Collider
	{
		Frustrum() {}
		PlaneCollider mFaces[6];
		vec4f mPoints[8];
	};

	class CubeCollider : public Collider
	{
	public:
		CubeCollider() {}
		CubeCollider(BaseObject* _obj, bool _move, bool _trigger, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, vec4f _min, vec4f _max);
		CubeCollider(vec4f _min, vec4f _max) { mMin = _min; mMax = _max; };
		vec4f mMin, mMax, mMinOffset, mMaxOffset;
		virtual void SetPos(const vec4f& _newPos);
	};

	class OrientedCubeCollider : public Collider
	{
	public:
		OrientedCubeCollider() {}
		OrientedCubeCollider(BaseObject* _obj, bool _move, bool _trigger, vec4f _gravity, float _mass, float _elasticity, float _staticFriction, float _kineticFriction, float _drag, float _xRadius, float _yRadius, float _zRadius);
		vec4f mCenter;
		float mWidth, mHeight, mDepth;
		vec4f mAxis[3];
		virtual void SetPos(const vec4f& _newPos);
	};

	class ButtonCollider : public CubeCollider
	{
	public:
		ButtonCollider() {}
		ButtonCollider(BaseObject* _obj, vec4f _min, vec4f _max, float _mass, float normForce, vec4f _pushNormal);
		vec4f mPushNormal;
		PlaneCollider mUpperBound, mLowerBound;
	};

	class ControllerCollider : public CubeCollider
	{
	public:
		ControllerCollider() {}
		ControllerCollider(BaseObject* _obj, vec4f _min, vec4f _max, bool _left);
		bool mLeft;
		std::unordered_set<Collider*> mHitting;
	};

	/*
	business entity- gmail, twitter, facebook, steam account
	art, audio, marketing, designer students ?

	first initial last name, password lower case
	gdserv.fullsail.com:8080
	install doc, follow it
	*/

}