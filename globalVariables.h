#pragma once

extern int toggle;
extern bool skyboxDraw;

extern bool mouseLeft; 
extern bool spacePress;
extern bool scrolling; 
extern bool grabPress;
extern bool alreadyGrabbing;
extern bool isHolding;
extern bool isSelection;

extern double lastX;
extern double lastY;

extern unsigned int viewport_width;
extern unsigned int viewport_height;

extern float fov;
extern float xOffset;
extern float yOffset;
extern float velocity;
extern float rSensitivity;
extern float scrollSensitivity;
extern float xVelocity;
extern float yVelocity;
extern float yScroll;
extern float closestZ;

extern double mouseX;
extern double mouseY;

// Delta
extern float deltaTime;
extern float lastFrame;
extern float currentFrame;

// FPS
extern int frameCount;
extern double previousTime;

// Window
extern int windowWidth;
extern int windowHeight;

// Sampler units
extern unsigned int samplerCounter;

//// Key presses
//extern bool key_escape;
//extern bool key_w;
//extern bool key_a;
//extern bool key_s;
//extern bool key_d;
//extern bool key_e;
//extern bool key_p;
//extern bool key_space;
//extern bool key_leftShift;
//extern bool key_leftShift;
