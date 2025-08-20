#include "globalVariables.h"

int toggle = 0;
bool skyboxDraw = 0;

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