<h2>Global state checks:</h2>

	Persistent between loops:
		bool skyboxDraw = 0;
		bool alreadyGrabbing = 0;

	Model related:
		bool isHolding = 0;
		bool isSelection = 0;
	
	Input related:
		bool mouseLeft = 0;
		bool spacePress = 0;
		bool scrolling = 0;
		bool grabPress = 0;


<h2>Per model state checks: </h2>

	One-time use:
		bool m_firstDraw	  = 1;

	Determined per-loop:
		bool m_isManipulating = 0;
		bool m_hasMoved		  = 0;
		bool m_rotationOn	  = 0;
		bool m_translationOn  = 0;
		bool m_scaleOn		  = 0;
		bool m_isSelected	  = 0;
		bool m_isGrabbed	  = 0;

	?I can't remember what this one is for?:
		bool m_rebindRequired = 0;


	Application
 ├─ Window
 ├─ Input
 ├─ SceneManager
 │   └─ Scene
 │       ├─ Entities
 │       │   └─ Model / Mesh / Material
 │       └─ Lights
 └─ Renderer
     ├─ Shaders
     ├─ Textures
     └─ Camera