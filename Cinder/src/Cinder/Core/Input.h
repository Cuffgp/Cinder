#pragma once

#include <glm/glm.hpp>

#include "Cinder/Core/KeyCodes.h"
#include "Cinder/Core/MouseCodes.h"

namespace Cinder {

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
