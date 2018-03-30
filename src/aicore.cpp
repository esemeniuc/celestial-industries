#include <aicore.h>

namespace aicore {

	DecisionTreeNode* Decision::makeDecision() {
		// Choose a branch based on the getBranch method
		if (getBranch()) {
			// Make sure its not null before recursing.
			if (trueBranch == nullptr) return nullptr;
			else return trueBranch->makeDecision();
		} else {
			// Make sure its not null before recursing.
			if (falseBranch == nullptr) return nullptr;
			else return falseBranch->makeDecision();
		}
	}

	RandomDecision::RandomDecision() :
			lastDecisionFrame(0),
			lastDecision(false) {
	}

	bool RandomDecision::getBranch() {
		unsigned thisFrame = 0;//TimingData::get().frameNumber;

		// If we didn't get here last time, then things may change
		if (thisFrame > lastDecisionFrame + 1) {
			lastDecision = false;//randomBoolean();
		}

		// In any case, store the frame number
		lastDecisionFrame = thisFrame;

		// And return the stored value
		return lastDecision;
	}

	RandomDecisionWithTimeOut::RandomDecisionWithTimeOut() :
			RandomDecision(),
			firstDecisionFrame(0) {
	}

	bool RandomDecisionWithTimeOut::getBranch() {
		unsigned thisFrame = 0;//TimingData::get().frameNumber;

		// Check if the stored decision is either too old, or if we
		// timed out.
		if (thisFrame > lastDecisionFrame + 1 ||
			thisFrame > firstDecisionFrame + timeOutDuration) {

			// Make a new decision
			lastDecision = false;//randomBoolean();

			// And record that it was just made
			firstDecisionFrame = thisFrame;
		}

		// Update the frame number
		lastDecisionFrame = thisFrame;

		// And return the stored value
		return lastDecision;
	}

} // end of namespace