#pragma once

#include <glm/glm.hpp>

namespace Cinder {

	class Camera
	{
	public:
		Camera(float width, float height);
		~Camera();
	private:
		glm::vec3 position;
		glm::vec3 direction;

		glm::mat4 view;
		glm::mat4 projection;
	};

}