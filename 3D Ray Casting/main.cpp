#include "Globals.h"

const int WIDTH = 1280, HEIGHT = 720, XPOS = 500, YPOS = 300;
const char *TITLE = "OpenCL / GL", 
	*SOURCE = "#include \"kernel.cl\"\n\0",
	*FUNCTION = "runK";

using namespace cl;

// OpenCL Objects
Context m_context;
CommandQueue m_queue;
Kernel m_kernel;
NDRange m_global, m_local;

// OpenGL Buffers
GLuint m_frameBuffer, m_depthBuffer;

// OpenCL/GL Render Texture Buffers 
GLuint m_frameTextureGL;
vector<Memory> m_sharedCL;

CLArray<Camera> m_camera;
float m_rot = 0.0f, m_dist = -7.0f;

int p_time, m_time = 0;

/**
* Entry Point
*/
int main(int argc, char *argv[]) {
	init(&argc, argv, WIDTH, HEIGHT, XPOS, YPOS, (char*)TITLE);
}

/**
* Implementation Variant: Setup additional OpenGL resources here
*/
void initResources(void) {
	std::cout << "Init Resources" << std::endl;
	cl_int err;

	// Set Kernel Params
	m_global = NDRange(WIDTH * HEIGHT);
	m_local = NullRange;

	// Capture GL Texture
	m_sharedCL.push_back(ImageGL(m_context, CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0, m_frameTextureGL, &err));
	clPrintErr(err, "Failed to create CL/GL Image -> ", stdout);
	m_kernel.setArg(0, m_sharedCL[0]);

	// Build Camera
	m_camera.init(1, m_context, CL_MEM_READ_ONLY);
	setFloat3(m_camera[0].pos,  0.0f, 0.0f, 0.0f);
	setFloat3(m_camera[0].norm, 0.0f, 0.0f, 0.0f);
	setFloat3(m_camera[0].left, 0.0f, 0.0f, 0.0f);
	setFloat3(m_camera[0].up,   0.0f, 0.0f, 0.0f);

	m_camera.setAsArg(m_kernel, 1);
}

/**
* Implementation Variant: Cleanup CL & GL Resources
*/
void cleanupResources(void) {
	std::cout << "Cleanup Resources" << std::endl;
	// Free Up Memory
	// ...
}

/**
* Stage the OpenCL Kernel
*/
void stageExecuteCL(void) {
	if (m_sharedCL.size() > 0) {
		m_queue.enqueueAcquireGLObjects(&m_sharedCL);
	}
	// Buffer Writes
	// ...

	p_time = m_time;
	m_time = glutGet(GLUT_ELAPSED_TIME);
	float time = (float)(m_time - p_time);

	// Update Camera
	m_rot += (0.05f * time);
	if (m_rot > 360.0f) {
		m_rot -= 360.0f;
	}
	if (m_rot < 0.0f) {
		m_rot += 360.0f;
	}

	setTranCam(m_camera[0], 0.0f, 0.0f, m_dist);
	setRotCam(m_camera[0], -20.0f, 0.0f, 0.0f);
	rotCam(m_camera[0], 0.0f, m_rot, 0.0f);
	makeLUCam(m_camera[0]);
	m_camera.writeBuffer(m_queue);
}

/**
* Collate the OpenCL Kernel
*/
void collateExecuteCL(void) {
	if (m_sharedCL.size() > 0) {
		m_queue.enqueueReleaseGLObjects(&m_sharedCL);
	}
	// Buffer Reads
	// ...
}

/**
* Draw additional OpenGL data to the frame
*/
void drawAdditional(void) {

	glColor3f(1.f, 1.f, 1.f);
	
	glPrintString(5, 1000-35, std::string("Joss Whittle"));

}

/**
* Fires when the Mouse is clicked on the Window
*
* @param button The id of the button clicked
* @param state Whether the button is up or down
* @param The x coord of the click
* @param The y coord of the click
*/
void onMouseClick(int button, int state, int x, int y) {

}

/**
* Fires when the Mouse is moved over the Window
*
* @param x The x coord of the mouse
* @param y The y coord of the mouse
*/
void onMouseMove(int x, int y) {

}

/**
* Fires when a Key is pressed on the Window
*
* @param key The character of the key press
* @param x The x coord of the mouse
* @param y The y coord of the mouse
*/
void onKeyPress(unsigned char key, int x, int y) {
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		//exit(1); // ESCAPE key
		break;
	case 's':
		m_dist -= 0.05f;
		break;
	case 'w':
		m_dist += 0.05f;
		break;
	}
}

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
void init(int *argc, char *argv[], int width, int height, int x, int y, char* title) {

	// Setup OpenGL
	std::cout << "Building GL Window\n";
	glutInit(argc, argv);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(x,y);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(title);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	// Init Glew
	glewInit();

	// GLut Callbacks
	glutDisplayFunc(onUpdateFrame);
	glutKeyboardFunc(onKeyPress);
	glutMouseFunc(onMouseClick);
	glutMotionFunc(onMouseMove);

	// Setup CL
	std::cout << "Setting up CL\n";
	try {
		std::cout << "Platforms\n";
		vector<Platform> platforms;
		Platform::get(&platforms);

#define USE_CLGL
#ifdef USE_CLGL
		std::cout << "CLGL Properties\n";
		cl_context_properties properties[7] = {
			CL_GL_CONTEXT_KHR,
			(cl_context_properties) wglGetCurrentContext(),
			CL_WGL_HDC_KHR,
			(cl_context_properties) wglGetCurrentDC(),
			CL_CONTEXT_PLATFORM,
			(cl_context_properties)(platforms[0])(),
			0
		};
#else
		std::cout << "Properties\n";
		cl_context_properties properties[3] = { 
			CL_CONTEXT_PLATFORM, 
			(cl_context_properties)(platforms[0])(), 
			0 
		};
#endif

		std::cout << "Context\n";
		m_context = Context(CL_DEVICE_TYPE_GPU, properties);

		std::cout << "Devices\n";
		vector<Device> devices = m_context.getInfo<CL_CONTEXT_DEVICES>();

		std::cout << "Queue\n";
		m_queue = CommandQueue(m_context, devices[0]);

		std::cout << "Source\n";
		Program::Sources source(1, std::make_pair(SOURCE, strlen(SOURCE)));

		std::cout << "Program\n";
		Program program(m_context, source);

		std::cout << "Build w/ Includes\n";
		program.build(devices, "-I ./");

		std::cout << "Kernel\n";
		m_kernel = Kernel(program, FUNCTION);

	} catch(Error error) {
		std::cout << std::endl << error.what() << "(" << error.err() << ")" << std::endl;
		fail("OpenCL Error");
	}

	// Setup GL
	std::cout << "Building GL Frame Buffer\n";
	glGenFramebuffers(1, &m_frameBuffer);
	glGenRenderbuffers(1, &m_depthBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);

	glGenTextures(1, &m_frameTextureGL);
	glBindTexture(GL_TEXTURE_2D, m_frameTextureGL);   
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);   
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_frameTextureGL, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		fail("FrameBuffer is not complete.");
	}

	// Implementation Variant: Setup additional GL resources
	initResources();

	// Run
	glutMainLoop();

	// Clean up
	cleanupResources();
}

/**
* Fires when a new frame is requested
*/
void onUpdateFrame(void) {

	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	// ?
	glFinish();

	// Execute OpenCL
	executeCL();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	int vx = glutGet(GLUT_WINDOW_WIDTH);
	int vy = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0,0, vx, vy);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0., 1000., 0., 1000.);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_frameTextureGL);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Screen UV's
	glBegin(GL_QUADS);
	glTexCoord2f(0., 0.);
	glVertex2f(0., 0.);
	glTexCoord2f(1., 0.);
	glVertex2f(1000., 0.);
	glTexCoord2f(1., 1.);
	glVertex2f(1000., 1000.);
	glTexCoord2f(0., 1.);
	glVertex2f(0., 1000.);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	drawAdditional();

	glutSwapBuffers();
	glutPostRedisplay();
}

/**
* Prepare and execute the OpenCL Kernel
*/
void executeCL(void) {
	Event wait;

	try {
		// Stage
		stageExecuteCL();

		// Execute
		cl_int err = m_queue.enqueueNDRangeKernel(m_kernel, NullRange, m_global, m_local, NULL, &wait);
		clPrintErr(err, "Execute Error -> ", stdout);
		wait.wait();
		m_queue.finish();

		// Collate
		collateExecuteCL();

	} catch(Error error) {
		std::cout << std::endl << error.what() << "(" << error.err() << ")" << std::endl;
		fail("OpenCL Error");
	}
}

/**
* Fails with a message
*
* @param str The cString of the error message
*/
void fail(char *str) {
	fprintf(stderr, "%s\n", str);
	system("PAUSE");
	exit(1);
}