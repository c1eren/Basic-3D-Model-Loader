#include "inputManager.h"

std::unordered_map<int, bool> InputManager::s_keys;
std::unordered_map<int, bool> InputManager::s_keysPressed;
std::unordered_map<int, bool> InputManager::s_keysReleased;

InputManager::InputManager(GLFWwindow* window) 
{
	s_window = window;
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	lastX   = 0.0;
	lastY   = 0.0;
	//mouseX  = 0.0;
	//mouseY  = 0.0;
	xOffset = 0.0f;
	yOffset = 0.0f;
	firstMouse = true;
}

void InputManager::update()
{
	s_keysPressed.clear();
	s_keysReleased.clear();
	s_mousePressed.clear();
	s_mouseReleased.clear();
	xOffset = 0.0f;
	yOffset = 0.0f;
}

bool InputManager::isKeyDown(int key)
{
	return s_keys[key];
}

bool InputManager::isKeyPressed(int key)
{
	return s_keysPressed[key];
}

bool InputManager::isKeyReleased(int key)
{
	return s_keysReleased[key];
}

bool InputManager::isMouseDown(int button)
{
	return s_mouseButtons[button];
}

bool InputManager::isMousePressed(int button)
{
	return s_mousePressed[button];
}

bool InputManager::isMouseReleased(int button)
{
	return s_mouseReleased[button];
}

double InputManager::getMouseX()
{
	return lastX;
}

double InputManager::getMouseY()
{
	return lastY;
}

float InputManager::getMouseDeltaX()
{
	return xOffset;
}

float InputManager::getMouseDeltaY()
{
	return yOffset;
}

float InputManager::getScrollY()
{
	return yScroll;
}

void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		s_keys[key] = true;
		s_keysPressed[key] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		s_keys[key] = false;
		s_keysReleased[key] = true;
	}

	// s_keys[key]		   -> Current state | true if held down, false if not
	// s_keysPressed[key]  -> True only in the frame the key was first pressed
	// s_keysReleased[key] -> True only in the frame the key was released
}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		s_mouseButtons[button] = true;
		s_mousePressed[button] = true;
	}
	else if (action == GLFW_RELEASE)
	{
		s_mouseButtons[button] = false;
		s_mouseReleased[button] = true;
	}
}

void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	xOffset = xpos - lastX;
	yOffset = lastY - ypos;
	lastX	= xpos;
	lastY	= ypos;
}

void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	yScroll = static_cast<float>(yoffset);
}
