#pragma once
#include "../Common/Math.h"
class BaseObject;
struct matrix4;


enum ComponentType
{
	eCOMPONENT_UNKNOWN,
	eCOMPONENT_CODE,
	eCOMPONENT_COLLIDER,
	eCOMPONENT_AUDIOEMITTER,
	eCOMPONENT_AUDIOLISTENER,
	eCOMPONENT_UI
};
class Component
{
protected:
	bool isEnable;
	ComponentType type;

	BaseObject* object = nullptr;
public:
	ComponentType GetType() { return type; };
	bool isEnabled() { return isEnable; };
	void Disable() { isEnable = false; };
	void Enable() { isEnable = true; };
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

class Collider : Component
{
	vec4f mAcceleration;
	vec4f mVelocity;
	
public:
	vec4f GetAcceleration() { return mAcceleration; };
	vec4f GetVelocity() { return mVelocity; };
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
	void Update();
	void Destroy();
	void SetVisible(bool _vis);
};

/*
business entity- gmail, twitter, facebook, steam account
art, audio, marketing, designer students ?

first initial last name, password lower case
gdserv.fullsail.com:8080
install doc, follow it




*/










