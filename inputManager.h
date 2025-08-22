#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>

class InputManager {
public:
	InputManager(GLFWwindow* window);
	~InputManager() {}

	static void update();
	static bool isKeyDown(int key);
	static bool isKeyPressed(int key);
	static bool isKeyReleased(int key);


private:
	GLFWwindow* s_window;
	static std::unordered_map<int, bool> s_keys;
	static std::unordered_map<int, bool> s_keysPressed;
	static std::unordered_map<int, bool> s_keysReleased;

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);


};