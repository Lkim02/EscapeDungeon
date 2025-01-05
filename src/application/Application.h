#ifndef APPLICATION_H
#define APPLICATION_H

#include "common.h"

#define APP (Application::getInstance())


/* Application  
 *	- for windows encapsulation
 *  - Singleton
 */
class Application {
public:
	~Application();
	static Application* getInstance();

	uint32_t getWidth()  const { return _width; }
	uint32_t getHeight() const { return _height; }
	
	/* Main Operation */
	bool init(const int&, const int&);
	bool update();
	void destroy();

	/* Set Event Callback */
	void setResizeCallback(ResizeCB cb) { _resize_cb = cb; }
	void setKeyboardCallback(KeyboardCB cb) { _keyboard_cb = cb; }
	void setMouseMoveCallback(MouseMoveCB cb) { _mouse_move_cb = cb; }
	void setMouseScrollCallback(MouseScrollCB cb) { _mouse_scroll_cb = cb; }

private:
	uint32_t _width  = 0;
	uint32_t _height = 0;
	GLFWwindow* _window = nullptr;

	/* Event Callback */
	ResizeCB _resize_cb = nullptr;
	static void fbSizeCallback(GLFWwindow* window, int width, int height);
	KeyboardCB _keyboard_cb = nullptr;
	static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	MouseMoveCB _mouse_move_cb = nullptr;
	static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
	MouseScrollCB _mouse_scroll_cb = nullptr;
	static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	static Application* mInstance;
	Application();
};

#endif // !APPLICATION_H