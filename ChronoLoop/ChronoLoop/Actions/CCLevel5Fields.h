#pragma once
#include "CodeComponent.hpp"
#include "../Core/LevelManager.h"


namespace Epoch
{

	struct CCLevel5Fields : public CodeComponent
	{
		BaseObject* mBox;
		Level* cLevel;
		matrix4 initialMatrix;
		float mScaleTipper;
		bool canBoxShrink, isBoxShrinking, mSoundOnce = false;
		BoxSnapToControllerAction* leftBS, *rightBS;
		void SetIsBoxShrinking(bool _set) { isBoxShrinking = _set; }
		virtual void Start()
		{
			cLevel = LevelManager::GetInstance().GetCurrentLevel();
			mBox = cLevel->FindObjectWithName("Box");
			initialMatrix = mBox->GetTransform().GetMatrix();
			mScaleTipper = 1.0f;
			isBoxShrinking = false;

			std::vector<Component*> codes1 = LevelManager::GetInstance().GetCurrentLevel()->GetLeftController()->GetComponents(Epoch::ComponentType::eCOMPONENT_CODE);
			for (size_t x = 0; x < codes1.size(); ++x)
			{
				if (dynamic_cast<BoxSnapToControllerAction*>(codes1[x]))
				{
					leftBS = ((BoxSnapToControllerAction*)codes1[x]);
					break;
				}
			}

			codes1 = LevelManager::GetInstance().GetCurrentLevel()->GetRightController()->GetComponents(Epoch::ComponentType::eCOMPONENT_CODE);
			for (size_t x = 0; x < codes1.size(); ++x)
			{
				if (dynamic_cast<BoxSnapToControllerAction*>(codes1[x]))
				{
					rightBS = ((BoxSnapToControllerAction*)codes1[x]);
					break;
				}
			}
		}

		virtual void OnTriggerEnter(Collider& _col, Collider& _other)
		{
			if (_other.GetBaseObject()->GetName() == "Box" && _col.mColliderType != Collider::eCOLLIDER_Controller)
			{
				isBoxShrinking = true;

				if (!mSoundOnce)
				{
					if (_col.GetBaseObject()->GetComponentCount(eCOMPONENT_AUDIOEMITTER) > 0)
					{
						if (dynamic_cast<AudioEmitter*>(_col.GetBaseObject()->GetComponentIndexed(eCOMPONENT_AUDIOEMITTER, 0)))
							((AudioEmitter*)_col.GetBaseObject()->GetComponentIndexed(eCOMPONENT_AUDIOEMITTER, 0))->CallEvent(Emitter::ePlay);
					}
					mSoundOnce = true;
				}
			}
		}
		virtual void Update()
		{
			if (isBoxShrinking)
			{
				mScaleTipper -= 0.05f;
				mBox->GetTransform().SetMatrix(matrix4::CreateNewScale(mScaleTipper, mScaleTipper, mScaleTipper) * mBox->GetTransform().GetMatrix());
				if (mScaleTipper <= 0.0f)
				{
					if (mBox->GetComponentCount(ComponentType::eCOMPONENT_COLLIDER) > 0)
						((Collider*)mBox->GetComponentIndexed(ComponentType::eCOMPONENT_COLLIDER, 0))->mForces = vec3f();
					isBoxShrinking = false;
					mScaleTipper = 1.0f;
					leftBS->mHeld = false;
					leftBS->mPickUp = nullptr;
					rightBS->mHeld = false;
					rightBS->mPickUp = nullptr;
					mSoundOnce = false;
					mBox->GetTransform().SetMatrix(initialMatrix);
					vec3f pos = vec3f(*mBox->GetTransform().GetPosition());
					((CubeCollider*)mBox->GetComponentIndexed(eCOMPONENT_COLLIDER, 0))->SetPos(pos);
				}
			}
		}
	};

}