#pragma once
#include "CodeComponent.hpp"
#include "..\Common\Logger.h"
#include "..\Core\LevelManager.h"
#include "..\Core\Pool.h"
#include "..\Core\TimeManager.h"
#include "..\Common\Settings.h"
#include "..\Actions\UIRewind.h"
#include "..\Actions\CCPauseToCancel.h"
#include "..\Actions\CCTeleToPlay.h"
#include "..\Actions\CCDisplayOnPause.h"
#include "..\Actions\UICreateToDeleteClone.h"
#include "..\Actions\UIClonePlusToMinus.h"
#include "..\Actions\UICloneText.h"
#include "..\Actions\BoxSnapToControllerAction.hpp"
#include "..\Actions\TeleportAction.hpp"
#include "..\PauseMenu.hpp"
#include "..\Actions\CCTimeIndicator.h"
#include "..\Rendering\Draw2D.h"
#include "..\Rendering\Renderer.h"
#include "..\Objects\TransparentMeshComponent.h"
#include "..\Rendering\TextureManager.h"
#include "..\Objects\TransparentMeshComponent.h"
#include <wrl\client.h>
#include "../Sound/SoundEngine.h"


namespace Epoch 
{

	struct CCEnterLevel : public CodeComponent 
	{
		const wchar_t* _basePath = L"../Resources/Soundbanks/";
		const wchar_t* _initSB = L"Init.bnk";
		const wchar_t* _aSB = L"Test_Soundbank.bnk";
		const wchar_t* _mainS = L"Chrono_Sound.bnk";

		
		bool once = true;
		void SetOnce(bool _set) { once = _set; };
		bool GetOnce() { return once; };
		virtual void OnTriggerEnter(Collider& _col1, Collider& _col2) 
		{
			once = false;
		}
		virtual void Start()
		{
			once = true;
		}
		virtual void Update() {
			if (!once) {
				Settings::GetInstance().SetBool("LevelIsLoading", true);
				Level* next = new Level;
				next->LoadLevel("../Resources/Level2.xml");
				// Todo: Un-hardcode this
				// use a setting string for next level path?
				//LM::LevelStatus status = LevelManager::GetInstance().LoadLevelAsync("../Resources/Level1_2_6.xml", &next);
				if (/*status == LM::LevelStatus::Success*/ true)
				{
					// Clean up the current level and request the new one be used next time.
					Physics::Instance()->PhysicsLock.lock();
					TimeManager::Instance()->Destroy();
					Physics::Instance()->mObjects.clear();
					LevelManager::GetInstance().RequestLevelChange(next);

					//Sound Initializing---------------------------------------------------
					TimeManager::Instance();

					Listener* ears = new Listener();
					Emitter* ambient = new AudioEmitter();
					((AudioEmitter*)ambient)->AddEvent(Emitter::EventType::ePlay, AK::EVENTS::PLAY_A_TIMELAPSE);
					((AudioEmitter*)ambient)->AddEvent(Emitter::EventType::ePause, AK::EVENTS::PAUSE_A_TIMELAPSE);
					((AudioEmitter*)ambient)->AddEvent(Emitter::EventType::eResume, AK::EVENTS::RESUME_A_TIMELAPSE);
					((AudioEmitter*)ambient)->AddEvent(Emitter::EventType::eStop, AK::EVENTS::STOP_A_TIMELAPSE);

					AudioWrapper::GetInstance().AddListener(ears, "Listener");
					AudioWrapper::GetInstance().AddEmitter(ambient, "ambiance");

					//new stuff
					Transform identity, t;
					t.SetMatrix(matrix4::CreateXRotation(DirectX::XM_PI / 2) * matrix4::CreateTranslation(8.8f, 1.3f, -4.75f));
					BaseObject* RightController = Pool::Instance()->iGetObject()->Reset("Controller1 - 0", t);
					BaseObject* LeftController = Pool::Instance()->iGetObject()->Reset("Controller2 - 0", identity); 
					BaseObject* headset = Pool::Instance()->iGetObject()->Reset("Headset - 0", identity);
					MeshComponent *mc = new MeshComponent("../Resources/Controller.obj");

					ControllerCollider* rightConCol = new ControllerCollider(RightController, vec3f(-0.15f, -0.15f, -0.15f), vec3f(0.15f, 0.15f, 0.15f), false);
					BoxSnapToControllerAction* pickup = new BoxSnapToControllerAction();
					((BoxSnapToControllerAction*)pickup)->mControllerRole = eControllerType_Primary;
					TransparentMeshComponent *rightRaycaster = new TransparentMeshComponent("../Resources/RaycastCylinder.obj");
					rightRaycaster->AddTexture("../Resources/Teal.png", eTEX_DIFFUSE);
					rightRaycaster->SetAlpha(0.4f);
					mc->AddTexture("../Resources/vr_controller_lowpoly_texture.png", eTEX_DIFFUSE);
					TeleportAction *ta = new TeleportAction(eControllerType_Primary);
					TimeManipulation* tm = new TimeManipulation(eControllerType_Primary);
					RightController->AddComponent(mc);
					RightController->AddComponent(rightConCol);
					RightController->AddComponent(pickup);
					RightController->AddComponent(rightRaycaster);
					RightController->AddComponent(ta);
					RightController->AddComponent(tm);

					BaseObject *clonePanel = Pool::Instance()->iGetObject()->Reset("ClonePanel", identity);
					MeshComponent* disp = new MeshComponent("../Resources/ClonePanel.obj");
					disp->AddTexture("../Resources/ClonePanel.png", eTEX_DIFFUSE);
					clonePanel->AddComponent(disp);
					clonePanel->SetParent(RightController);
					RightController->AddChild(clonePanel);

					t.SetMatrix(matrix4::CreateScale(.1f, .1f, .1f));
					BaseObject *timeDisplay = Pool::Instance()->iGetObject()->Reset("TimeIndicatorLine", t);
					MeshComponent* tdisp = new MeshComponent("../Resources/TimeIndicatorLine.obj");
					tdisp->AddTexture("../Resources/TimeIndicatorLine.png", eTEX_DIFFUSE);
					timeDisplay->AddComponent(tdisp);
					timeDisplay->SetParent(RightController);
					RightController->AddChild(timeDisplay);

					BaseObject *timeDisplayNeedle = Pool::Instance()->iGetObject()->Reset("TimeIndicatorNeedle", t);
					MeshComponent* tdispn = new MeshComponent("../Resources/TimeIndicator.obj");
					CCTimeIndicator* time = new CCTimeIndicator();
					timeDisplayNeedle->AddComponent(time);
					tdispn->AddTexture("../Resources/TimeIndicator.png", eTEX_DIFFUSE);
					timeDisplayNeedle->AddComponent(tdispn);
					timeDisplayNeedle->SetParent(RightController);
					RightController->AddChild(timeDisplayNeedle);

					t.SetMatrix(matrix4::CreateTranslation(0, .015f, .054f));
					BaseObject *rewindDisplay = Pool::Instance()->iGetObject()->Reset("RewindDisplay", t);
					MeshComponent* rewind = new MeshComponent("../Resources/UIRewind.obj");
					rewind->AddTexture("../Resources/rewind.png", eTEX_DIFFUSE);
					UIRewind* spin = new UIRewind();
					CCDisplayOnPause* rdop = new CCDisplayOnPause();
					rewindDisplay->AddComponent(rdop);
					rewindDisplay->AddComponent(spin);
					rewindDisplay->AddComponent(rewind);
					rewindDisplay->SetParent(RightController);
					RightController->AddChild(rewindDisplay);

					t.SetMatrix(matrix4::CreateScale(.75f, 1, 1) * matrix4::CreateTranslation(0.073f, -0.018f, -0.043f));
					BaseObject *cloneDisplayBack = Pool::Instance()->iGetObject()->Reset("cloneDisplayBack", t);
					TransparentMeshComponent* cdispb = new TransparentMeshComponent("../Resources/UIClone.obj");
					cdispb->AddTexture("../Resources/clearBlue.png", eTEX_DIFFUSE);
					cloneDisplayBack->AddComponent(cdispb);
					cloneDisplayBack->SetParent(RightController);
					RightController->AddChild(cloneDisplayBack);

					t.SetMatrix(matrix4::CreateTranslation(0.073f, -0.016f, -0.043f));
					BaseObject *cloneDisplay = Pool::Instance()->iGetObject()->Reset("cloneDisplay", t);
					MeshComponent* cdisp = new MeshComponent("../Resources/UIClone.obj");
					//cdisp->AddTexture("../Resources/clear.png", eTEX_DIFFUSE);

					HRESULT hr;
					Microsoft::WRL::ComPtr<ID3D11Texture2D> screenTex;
					Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

					if (TextureManager::Instance()->iGetTexture2D("memory:Clone Display", &srv, &screenTex) != TextureManager::TextureStatus::eError)
					{
						std::string str("Clone Display");
						cdisp->AddTexture(str.c_str(), eTEX_DIFFUSE);
						Font* font = new Font();
						cdisp->GetContext().mTextures[eTEX_DIFFUSE] = srv;
						UICloneText* ct = new UICloneText();
						cloneDisplay->AddComponent(ct);
						cloneDisplay->AddComponent(cdisp);
						cloneDisplay->SetParent(RightController);
						RightController->AddChild(cloneDisplay);
					}
					else
					{
						D3D11_TEXTURE2D_DESC bitmapDesc;
						bitmapDesc.Width = 256;
						bitmapDesc.Height = 256;
						bitmapDesc.MipLevels = 1;
						bitmapDesc.ArraySize = 1;
						bitmapDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
						bitmapDesc.SampleDesc.Count = 1;
						bitmapDesc.SampleDesc.Quality = 0;
						bitmapDesc.Usage = D3D11_USAGE_DEFAULT;
						bitmapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
						bitmapDesc.CPUAccessFlags = 0;
						bitmapDesc.MiscFlags = 0;
						hr = Renderer::Instance()->GetDevice()->CreateTexture2D(&bitmapDesc, nullptr, screenTex.GetAddressOf());
						std::string str("Clone Display");
						TextureManager::Instance()->iAddTexture2D(str, screenTex, &srv);
						cdisp->AddTexture(str.c_str(), eTEX_DIFFUSE);

						Font* font = new Font();
						cdisp->GetContext().mTextures[eTEX_DIFFUSE] = srv;
						UICloneText* ct = new UICloneText();
						cloneDisplay->AddComponent(ct);
						cloneDisplay->AddComponent(cdisp);
						cloneDisplay->SetParent(RightController);
						RightController->AddChild(cloneDisplay);
					}

					//Font* font = new Font();
					cdisp->GetContext().mTextures[eTEX_DIFFUSE] = srv;
					UICloneText* ct = new UICloneText();
					cloneDisplay->AddComponent(ct);
					cloneDisplay->AddComponent(cdisp);
					cloneDisplay->SetParent(RightController);
					RightController->AddChild(cloneDisplay);

					t.SetMatrix(matrix4::CreateTranslation(-0.039f, 0.015f, 0.054f));
					BaseObject *rewindHelp = Pool::Instance()->iGetObject()->Reset("RewindHelp", t);
					MeshComponent* rhdisp = new MeshComponent("../Resources/help.obj");
					rhdisp->SetVisible(false);
					rhdisp->AddTexture("../Resources/rewindHelp.png", eTEX_DIFFUSE);
					CCDisplayOnPause* dop = new CCDisplayOnPause();
					rewindHelp->AddComponent(dop);
					rewindHelp->AddComponent(rhdisp);
					rewindHelp->SetParent(RightController);
					RightController->AddChild(rewindHelp);

					t.SetMatrix(matrix4::CreateTranslation(0, 0.015f, 0.054f));
					BaseObject *teleportHelp = Pool::Instance()->iGetObject()->Reset("teleportHelp", t);
					MeshComponent* thdisp = new MeshComponent("../Resources/help.obj");
					thdisp->AddTexture("../Resources/teleport.png", eTEX_DIFFUSE);
					CCTeleToPlay* ttp = new CCTeleToPlay();
					teleportHelp->AddComponent(ttp);
					teleportHelp->AddComponent(thdisp);
					teleportHelp->SetParent(RightController);
					RightController->AddChild(teleportHelp);

					t.SetMatrix(matrix4::CreateTranslation(-0.03f, -0.01f, 0.089f));
					BaseObject *pauseHelp = Pool::Instance()->iGetObject()->Reset("pauseHelp", t);
					MeshComponent* phdisp = new MeshComponent("../Resources/help.obj");
					phdisp->AddTexture("../Resources/pause.png", eTEX_DIFFUSE);
					CCPauseToCancel* ptc = new CCPauseToCancel();
					pauseHelp->AddComponent(ptc);
					pauseHelp->AddComponent(phdisp);
					pauseHelp->SetParent(RightController);
					RightController->AddChild(pauseHelp);
					
					t.SetMatrix(matrix4::CreateTranslation(0.032f, -0.03f, 0.047f));
					BaseObject *cloneHelp = Pool::Instance()->iGetObject()->Reset("cloneHelp", t);
					MeshComponent* chdisp = new MeshComponent("../Resources/help.obj");
					chdisp->AddTexture("../Resources/createClone.png", eTEX_DIFFUSE);
					CCDisplayOnPause* cdop = new CCDisplayOnPause();
					UICreateToDeleteClone* cd = new UICreateToDeleteClone();
					cloneHelp->AddComponent(cd);
					cloneHelp->AddComponent(cdop);
					cloneHelp->AddComponent(chdisp);
					cloneHelp->SetParent(RightController);
					RightController->AddChild(cloneHelp);

					t.SetMatrix(matrix4::CreateScale(.5f, .5f, .5f) * matrix4::CreateTranslation(0.042f, -0.03f, 0.047f));
					BaseObject *clonePlus = Pool::Instance()->iGetObject()->Reset("clonePlus", t);
					MeshComponent* cphdisp = new MeshComponent("../Resources/help.obj");
					cphdisp->AddTexture("../Resources/createClone.png", eTEX_DIFFUSE);
					CCDisplayOnPause* cpdop = new CCDisplayOnPause();
					UIClonePlusToMinus* pm = new UIClonePlusToMinus();
					clonePlus->AddComponent(pm);
					clonePlus->AddComponent(cpdop);
					clonePlus->AddComponent(cphdisp);
					clonePlus->SetParent(RightController);
					RightController->AddChild(clonePlus);

					//pat added
					MeshComponent *mc2 = new MeshComponent("../Resources/Controller.obj");
					ControllerCollider* leftConCol = new ControllerCollider(LeftController, vec3f(-0.15f, -0.15f, -0.15f), vec3f(0.15f, 0.15f, 0.15f), true);
					BoxSnapToControllerAction* pickup2 = new BoxSnapToControllerAction();
					((BoxSnapToControllerAction*)pickup2)->mControllerRole = eControllerType_Secondary;
					TransparentMeshComponent *leftRaycaster = new TransparentMeshComponent("../Resources/RaycastCylinder.obj");
					leftRaycaster->AddTexture("../Resources/Teal.png", eTEX_DIFFUSE);
					leftRaycaster->SetAlpha(0.4f);
					mc2->AddTexture("../Resources/vr_controller_lowpoly_texture.png", eTEX_DIFFUSE);
					TeleportAction *ta2 = new TeleportAction(eControllerType_Secondary);
					TimeManipulation* tm2 = new TimeManipulation(eControllerType_Secondary);
					LeftController->AddComponent(mc2);
					LeftController->AddComponent(leftConCol);
					LeftController->AddComponent(pickup2);
					LeftController->AddComponent(leftRaycaster);
					LeftController->AddComponent(ta2);
					LeftController->AddComponent(tm2);

					MeshComponent *visibleMesh2 = new MeshComponent("../Resources/TinyCube.obj");
					visibleMesh2->AddTexture("../Resources/cube_texture.png", eTEX_DIFFUSE);
					visibleMesh2->SetVisible(false);
					headset->AddComponent(visibleMesh2);

					HeadsetFollow* hfollow = new HeadsetFollow();
					headset->AddComponent(hfollow);
					headset->AddComponent(ears);
					headset->AddComponent(ambient);
					PauseMenu* pauseComp = new PauseMenu();
					headset->AddComponent(pauseComp);

					AudioWrapper::GetInstance().STOP();

					((AudioEmitter*)ambient)->CallEvent(Emitter::EventType::ePlay);


					ParticleSystem::Instance()->Clear();
					LevelManager::GetInstance().RequestLevelChange(next);


					//Enter effect
					Particle* start = &Particle::Init();
					start->SetPos(vec3f(0, 0, 0));
					start->SetColors(vec3f(.2f, .2f, 1), vec3f(0, 1, .2f));
					start->SetLife(500);
					start->SetSize(.35f, .15f);
					ParticleEmitter* startEmit = new TeleportEffect(400, 250, 2, vec4f(8, 0, -4, 1));
					startEmit->SetParticle(start);
					startEmit->SetTexture("../Resources/BasicRectP.png");
					((TeleportEffect*)startEmit)->y1 = 8;
					((TeleportEffect*)startEmit)->y2 = 12;
					((TeleportEffect*)startEmit)->SetPosBounds(vec3f(-1, 0, -1), vec3f(1, 1, 1));
					((TeleportEffect*)startEmit)->SetVelBounds(vec3f(.5f, 1, .5f), vec3f(.5f, 5, .5f));
					ParticleSystem::Instance()->AddEmitter(startEmit);
					startEmit->FIRE();

					start = &Particle::Init();
					start->SetPos(vec3f(0, 0, 0));
					start->SetColors(vec3f(.5f, 0, .25f), vec3f(.2f, .8f, .5f));
					start->SetLife(500);
					start->SetSize(.15f, .05f);
					ParticleEmitter* startEmit2 = new TeleportEffect(400, 150, 1, vec4f(8, 0, -4, 1));
					startEmit2->SetTexture("../Resources/BasicCircleP.png");
					startEmit2->SetParticle(start);
					((TeleportEffect*)startEmit2)->y1 = 1;
					((TeleportEffect*)startEmit2)->y2 = 5;
					((TeleportEffect*)startEmit2)->SetPosBounds(vec3f(-1, 0, -1), vec3f(1, 1, 1));
					((TeleportEffect*)startEmit2)->SetVelBounds(vec3f(.5f, 1, .5f), vec3f(.5f, 5, .5f));
					ParticleSystem::Instance()->AddEmitter(startEmit2);
					startEmit2->FIRE();

					next->AssignPlayerControls(headset, LeftController, RightController);
					next->AddObject(headset);
					next->AddObject(LeftController);
					next->AddObject(RightController);
					next->AddObject(cloneDisplay);
					next->AddObject(cloneDisplayBack);
					next->AddObject(clonePanel);
					next->AddObject(timeDisplay);
					next->AddObject(timeDisplayNeedle);
					next->AddObject(rewindDisplay);
					next->AddObject(rewindHelp);
					next->AddObject(pauseHelp);
					next->AddObject(cloneHelp);
					next->AddObject(teleportHelp);
					next->AddObject(clonePlus);
					
					TimeManager::Instance()->AddObjectToTimeline(RightController);
					TimeManager::Instance()->AddObjectToTimeline(LeftController);
					TimeManager::Instance()->AddObjectToTimeline(headset);

					Light* l1 = new Light();
					l1->Type = 4;
					l1->Color = vec3f(1, 1, 1);
					l1->ConeDirection = vec3f(0, -1, 0);
					l1->Position = vec3f(0.07529334, 1.129413, 8.11148);
					l1->ConeRatio = .1f;

					Light* l2 = new Light();
					l2->Type = 2;
					l2->Position = vec3f(-4, 0, -5);
					l2->Color = vec3f(0, 0, .2f);

					Light* l3 = new Light();
					l3->Type = 4;
					l3->Color = vec3f(.9f, .9f, 1);
					l3->ConeDirection = vec3f(0, -1, 0);
					l3->Position = vec3f(-8.8f, 0, 5);
					l3->ConeRatio = .5f;

					Renderer::Instance()->SetLight(l1, 0);
					Renderer::Instance()->SetLight(l2, 1);
					Renderer::Instance()->SetLight(l3, 2);

					SystemLogger::Debug() << "Loading complete" << std::endl;
					Physics::Instance()->PhysicsLock.unlock();
					Settings::GetInstance().SetBool("LevelIsLoading", false);
					Settings::GetInstance().SetBool("PlayingLevel2", true);
				}
			}
		}
	};

}