#pragma once
#include "../Objects/BaseObject.h"
#include "../Common/Logger.h"
#include "CodeComponent.hpp"
#include "../Objects/MeshComponent.h"
#include "../Physics/Physics.h"
#include "../Input/VRInputManager.h"

struct TeleportAction : public CodeComponent {
	MeshComponent *mPlaneMesh;
	

	virtual void Start() {
		mPlaneMesh = (MeshComponent*)BaseObject::GetObjectByName("plane")->GetComponentIndexed(eCOMPONENT_MESH, 0);
	}

	virtual void Update() {
		if (!VRInputManager::Instance().iIsInitialized()) {
			return;
		}
		// I'm lazy so, let's just set this thing's position to the controller's position.
		matrix4 mat = VRInputManager::Instance().iGetController(true).GetPosition();
		mObject->GetTransform().SetMatrix(mat);

		if (VRInputManager::Instance().iGetController(true).GetPressDown(vr::EVRButtonId::k_EButton_SteamVR_Trigger)) {
			vec4f forward(0, 0, 1, 0);
			forward *= mObject->GetTransform().GetMatrix();
			Triangle *tris = mPlaneMesh->GetTriangles();
			size_t numTris = mPlaneMesh->GetTriangleCount();
			vec4f position = mat.fourth;
			for (unsigned int i = 0; i < numTris; ++i) {
				float time = 0;
				if (Physics::Instance()->RayToTriangle((tris + i)->Vertex[0], (tris + i)->Vertex[1], (tris + i)->Vertex[2], (tris + i)->Normal, position, forward, time)) {
					forward *= time;
					VRInputManager::Instance().iGetPlayerPosition()[3][0] += forward[0]; // x
					VRInputManager::Instance().iGetPlayerPosition()[3][2] += forward[2]; // z
					//VRInputManager::Instance().iGetPlayerPosition()[3][3] += forward[3]; // w
				}
			}
		}
	}

};