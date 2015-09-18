#pragma once

namespace kl {
class Time {
public:
	static void SetDeltaTime( float dt ) {
		totalTime += dt;
		deltaTime = dt;
	}
	static float GetDeltaTime() {
		return deltaTime;
	}

	static float GetTotalTime() {
		return totalTime;
	}

protected:
	static float totalTime;
	static float deltaTime;
};
} // namespace mj
