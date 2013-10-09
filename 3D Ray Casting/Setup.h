// Dependencies

/**
* Initialize GL Window
*
* @param argc System args count
* @param argv System args array
* @param width Window width
* @param height Window height
* @param x Window location
* @param y Window location
* @param title Window title
*/
void init(int *argc, char *argv[], int width, int height, int x, int y, char* title);

/**
* Implementation Variant: Setup additional OpenGL resources here
*/
void initResources(void);

/**
* Implementation Variant: Cleanup CL & GL Resources
*/
void cleanupResources(void);

/**
* Prepare and execute the OpenCL Kernel
*/
void executeCL(void);

/**
* Stage the OpenCL Kernel
*/
void stageExecuteCL(void);

/**
* Collate the OpenCL Kernel
*/
void collateExecuteCL(void);

/**
* Draw additional OpenGL data to the frame
*/
void drawAdditional(void);

/**
* Fails with a message
*
* @param str The cString of the error message
*/
void fail(char *str);

// GLut Callbacks

/**
* Implementation Variant: Fires when a new frame is requested
*/
void onUpdateFrame(void);

/**
* Implementation Variant: Fires when the Mouse is clicked on the Window
*
* @param button The id of the button clicked
* @param state Whether the button is up or down
* @param The x coord of the click
* @param The y coord of the click
*/
void onMouseClick(int button, int state, int x, int y);

/**
* Implementation Variant: Fires when the Mouse is moved over the Window
*
* @param x The x coord of the mouse
* @param y The y coord of the mouse
*/
void onMouseMove(int x, int y);

/**
* Implementation Variant: Fires when a Key is pressed on the Window
*
* @param key The character of the key press
* @param x The x coord of the mouse
* @param y The y coord of the mouse
*/
void onKeyPress(unsigned char key, int x, int y);