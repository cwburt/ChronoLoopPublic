#include <openvr.h>
#include "../Common/Math/vec3f.h"

namespace Epoch {

	class InputTimeline {
	public:
		//store Controller input information.
		struct InputData {
			//BaseObject id of the controller at the time
			unsigned short mControllerId;
			float mTime;
			unsigned int mLastFrame;
			vr::EVRButtonId mButton;
			//-1:Down 0:Press 1:Up
			short mButtonState;
			vec3f mVelocity;
			bool mPrimary;
		};
		struct InputNode {
			InputNode* mNext = nullptr;
			InputNode* mPrev = nullptr;
			InputData mData;
		};
	private:
		InputNode* mHead = nullptr;
		InputNode* mCurrent = nullptr;
	public:
		InputTimeline();
		~InputTimeline();

		void Push_back(InputNode* _new);
		void Insert(InputNode* _data);
		InputNode* GetHead() { return mHead; };
		InputNode* GetCurr() { return mCurrent; };
		void DisplayTimeline();
		void SetCurr(InputNode* _set);
		void Clear();

	};
}
