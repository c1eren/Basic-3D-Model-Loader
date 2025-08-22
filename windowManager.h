#pragma once
#include <GLFW/glfw3.h>

class WindowManager {
public:
	WindowManager();
	~WindowManager() {}

	int init();

public:
	GLFWwindow* m_window;

};