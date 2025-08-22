#include "globalVariables.h"

int toggle = 0;
bool skyboxDraw = 1;

bool mouseLeft = 0;
bool spacePress = 0;
bool scrolling = 0;
bool grabPress = 0;
bool alreadyGrabbing = 0;
bool isHolding = 0;
bool isSelection = 0;

double lastX = 400;
double lastY = 300;

unsigned int viewport_width = 800;
unsigned int viewport_height = 600;

float fov = 90.0f;
float xOffset = 0.0f;
float yOffset = 0.0f;
float velocity = 0.0f;
float rSensitivity = 0.1f;
float scrollSensitivity = 0.1f;
float xVelocity = 0.0f;
float yVelocity = 0.0f;
float yScroll = 0.0f;
float closestZ = 100.0f;

double mouseX;
double mouseY;

// Delta
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;

// FPS
int frameCount = 0;
double previousTime = 0;

// Window
int windowWidth = 800;
int windowHeight = 800;

// Sampler units
unsigned int samplerCounter = 1;

//// Key presses
//bool key_escape = 0;
//bool key_w = 0;
//bool key_a = 0;
//bool key_s = 0;
//bool key_d = 0;
//bool key_e = 0;
//bool key_p = 0;
//bool key_space = 0;
//bool key_leftShift = 0;
//bool key_leftShift = 0;
