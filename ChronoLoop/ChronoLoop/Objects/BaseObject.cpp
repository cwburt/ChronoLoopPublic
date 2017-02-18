//#include "stdafx.h"
#include "BaseObject.h"
#include "../Actions/CodeComponent.hpp"
#include "../Common/Logger.h"

// 0 is reserved for the player.
unsigned int BaseObject::ObjectCount = 1;

BaseObject::BaseObject()
{
	mParent = nullptr;
	mUniqueID = BaseObject::ObjectCount++;
}
BaseObject::BaseObject(std::string _name, Transform _transform)
{
	mName = _name;
	mParent = nullptr;
	mTransform = _transform;
}

BaseObject::~BaseObject()
{
	if (mDestroyed) {
		SystemLogger::GetError() << "[Warning] Deleting an object that is marked as destroyed." << std::endl;
	} else {
		Destroy();
	}
}

//BaseObject BaseObject::Clone()
//{
//	BaseObject temp;
//	temp.m_name = this->m_name;
//	temp.m_transform = this->m_transform;
//	temp.m_parent = this->m_parent;
//	temp.m_components = this->m_components;
//	temp.m_children = this->m_children;
//	return temp;
//}
//
//BaseObject BaseObject::Clone(BaseObject _clone)
//{
//	_clone.m_name = this->m_name;
//	_clone.m_transform = this->m_transform;
//	_clone.m_parent = this->m_parent;
//	_clone.m_components = this->m_components;
//	_clone.m_children = this->m_children;
//	return _clone;
//}

BaseObject& BaseObject::operator=(BaseObject& _equals)
{
	mUniqueID   = _equals.mUniqueID;
	mName       = _equals.mName;
	mParent     = _equals.mParent;
	mChildren   = _equals.mChildren;
	mTransform  = _equals.mTransform;
	mComponents = _equals.mComponents;
	return *this;
}

void BaseObject::Destroy()
{
	if (mDestroyed) {
		SystemLogger::GetError() << "[Error] Attempting to destroy an object that is already marked as destroyed." << std::endl;
		return;
	}
	if (mParent) {
		delete mParent;
		mParent = nullptr;
	}
	for (auto iter = mComponents.begin(); iter != mComponents.end(); ++iter) {
		for (int i = 0; i < iter->second.size(); ++i) {
			iter->second[i]->Destroy();
			delete iter->second[i];
		}
	}
	mComponents.clear();
	for (auto it = mChildren.begin(); it != mChildren.end(); ++it) {
		delete (*it);
	}
	mChildren.clear();
	mDestroyed = true;
}

void BaseObject::Update() {
	if (mDestroyed) {
		SystemLogger::GetError() << "[Error] Attempting to update an object that is already marked as destroyed." << std::endl;
		return;
	}
	for (auto it = mComponents.begin(); it != mComponents.end(); ++it) {
		if (it->first == eCOMPONENT_COLLIDER) {
			continue;
		}
		for (auto cIt = it->second.begin(); cIt != it->second.end(); ++cIt) {
			(*cIt)->Update();
		}
	}
}

unsigned int BaseObject::AddComponent(Component * _comp) {
	if (mDestroyed) {
		SystemLogger::GetError() << "[Error] Attempting to add a compontnet to an object that is already marked as destroyed." << std::endl;
		return -1;
	}
	if (_comp->GetType() == eCOMPONENT_MAX) {
		SystemLogger::GetError() << "[Error] Trying to add a component with an invalid type. This is not allowed, returning -1U." << std::endl;
		return -1;
	}
	if (!_comp->IsValid()) {
		SystemLogger::GetError() << "[Error] Attempted to add a component that is marked invalid. It will be deleted." << std::endl;
		delete _comp;
		return -1;
	}
	_comp->mObject = this;
	mComponents[_comp->GetType()].push_back(_comp);
	if (_comp->GetType() == eCOMPONENT_CODE) {
		((CodeComponent*)_comp)->Start();
	}
	return (unsigned int)mComponents[_comp->GetType()].size();
}

bool BaseObject::RemoveComponent(Component * _comp) {
	if (mDestroyed) {
		SystemLogger::GetError() << "[Error] Attempting to remove a component from an object that is already marked as destroyed." << std::endl;
		return false;
	}
	if (_comp->GetType() == eCOMPONENT_MAX) {
		SystemLogger::GetError() << "[Error] Trying to remove a component with an invalid type. This is not allowed, returning -1U." << std::endl;
		return false;
	}
	ComponentType type = _comp->GetType();
	unsigned int size = (unsigned int)mComponents[type].size();
	for (auto it = mComponents[type].begin(); it != mComponents[type].end(); ++it) {
		if((*it) == _comp) {
			(*it)->Destroy();
			delete (*it);
			mComponents[type].erase(it);
			return true;
		}
	}
	return false;
}

//void BaseObject::AddComponent(ComponentType _type, Component* _comp)
//{
//	mComponents[_type].push_back(_comp);
//}