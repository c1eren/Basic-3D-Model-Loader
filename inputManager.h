#pragma once
#include <GLFW/glfw3.h>
#include <unordered_map>

class InputManager {
public:
	InputManager(GLFWwindow* window);
	~InputManager() {}

	static void update();

	// Keys
	static bool isKeyDown(int key);
	static bool isKeyPressed(int key);
	static bool isKeyReleased(int key);

	// Mouse
	static bool isMouseDown(int button);
	static bool isMousePressed(int button);
	static bool isMouseReleased(int button);
	static double getMouseX();
	static double getMouseY();
	static float  getMouseDeltaX();
	static float  getMouseDeltaY();

	static float getScrollY();

private:
	GLFWwindow* s_window;
	// Keys
	static std::unordered_map<int, bool> s_keys;
	static std::unordered_map<int, bool> s_keysPressed;
	static std::unordered_map<int, bool> s_keysReleased;

	// Mouse
	static std::unordered_map<int, bool> s_mouseButtons;
	static std::unordered_map<int, bool> s_mousePressed;
	static std::unordered_map<int, bool> s_mouseReleased;
	
	static double lastX; 
	static double lastY; 
	//static double mouseX;
	//static double mouseY;
	static float  xOffset;
	static float  yOffset;
	static float  yScroll;

	static bool firstMouse;

private:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};