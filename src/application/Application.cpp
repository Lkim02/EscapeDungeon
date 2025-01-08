
#include "Application.h"

#include "utils.h"

/* Constructor & Destructor */
Application* Application::mInstance = nullptr;
Application* Application::getInstance() {
	if (mInstance == nullptr) mInstance = new Application();
	return mInstance;
}

Application::Application() {

}

Application::~Application() {

}


/* Main Operation */
bool Application::init(const int& w = 800, const int& h = 600) {
	_width = w; _height = h;

	/* GLFW initialization */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // OpenGL 4.6
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // use core mode

	/* Create window */
	_window = glfwCreateWindow(_width, _height, "EscapeDungeon", nullptr, nullptr);
	if (_window == nullptr) {
		perror("Create window");
		return false;
	}
	glfwMakeContextCurrent(_window);

	/* GLAD load functions */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		perror("Glad init");
		return false;
	}

	/* Set Event Callback */
	glfwSetFramebufferSizeCallback(_window, fbSizeCallback);
	glfwSetKeyCallback(_window, keyboardCallback);
	glfwSetCursorPosCallback(_window, mouseMoveCallback);
	glfwSetScrollCallback(_window, mouseScrollCallback);

	/* GLFW settings */
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // capture the cursor
	glfwSetWindowUserPointer(_window, this);	// store 'this' as glfw user pointer
	glEnable(GL_DEPTH_TEST);

	/* Init Windows Viewport and Background Color */
	GL_CALL(glViewport(0, 0, _width, _height));
	GL_CALL(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));

	return true;
}

bool Application::update() {
	if (glfwWindowShouldClose(_window)) return false;

	/* Handle events */
	glfwPollEvents(); // using message queue IPC mechanism ?

	/* Swap buffers: Double buffers */
	GL_CALL(glfwSwapBuffers(_window));

	return true;
}

void Application::destroy() {
	glfwTerminate();
	delete mInstance;
}


/* Event Callback Handlding: All static */
void Application::fbSizeCallback(GLFWwindow* window, int width, int height) {
#ifdef DEBUG
	std::cout << "[Renew window] " << width << ", " << height << std::endl;
#endif
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->_resize_cb != nullptr)
		self->_resize_cb(width, height);
}

void Application::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
#ifdef DEBUG
	// std::cout << "[Keyboard event]" << std::endl;
#endif
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->_keyboard_cb != nullptr)
		self->_keyboard_cb(key, scancode, action, mods);
}

void Application::mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
#ifdef DEBUG
	//std::cout << "[Mouse move event]" << std::endl;
#endif
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->_mouse_move_cb != nullptr)
		self->_mouse_move_cb(xpos, ypos);
}

void Application::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
#ifdef DEBUG
	std::cout << "[Mouse scroll event]" << std::endl;
#endif
	Application* self = (Application*)glfwGetWindowUserPointer(window);
	if (self->_mouse_scroll_cb != nullptr)
		self->_mouse_scroll_cb(yoffset);
}