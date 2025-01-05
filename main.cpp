
#include <string>
#include <vector>

#include "common.h"
#include "utils.h"

#include "application/Application.h"
#include "world/World.h"

/*=== Application Global ===*/
uint32_t viewport_width = 800, viewport_height = 600;
bool keyboard[128] = { false };

bool first_mouse = true;
float last_x = (float)viewport_width / 2.0f;
float last_y = (float)viewport_height / 2.0f;

World world;

irrklang::ISoundEngine* sound_engine = irrklang::createIrrKlangDevice();

/*=== Callback Handler ===*/
void onResize(int width, int height);
void onKeyChange(int key, int scancode, int action, int mods);
void onMouseMove(double xpos_in, double ypos_in);
void onMouseScroll(/* double xoffset, */double yoffset);

/*=== Main Logic ===*/
int main() {
    bool status = APP->init(viewport_width, viewport_height);
    if (!status) return -1;

    /* World Init */
    world.init();

    /* Set Callback Monitor */
    APP->setResizeCallback(onResize);
    APP->setKeyboardCallback(onKeyChange);
    APP->setMouseMoveCallback(onMouseMove);
    APP->setMouseScrollCallback(onMouseScroll);
   
    /* Window Loop */
    world.start();
    while (APP->update()) {
        world.update();
        world.render();
    }

    APP->destroy();
    sound_engine->drop(); // Maybe can be placed in APP, but maybe troublesome
    std::cout << "cleared" << std::endl;
    return 0;
}


/* Callback Handler Implement */
void onResize(int width, int height) {
    std::cout << "onResize" << std::endl;
    GL_CALL(glViewport(0, 0, width, height));
    viewport_width = width;
    viewport_height = height;
}

void onKeyChange(int key, int scancode, int action, int mods) {
    if (0 <= key && key < 128) {
        if (action == GLFW_PRESS) {
            keyboard[key] = true;
            world.processKeyboardPress();
        }
        else if (action == GLFW_RELEASE) {
            keyboard[key] = false;
            world.processKeyboardRelease();
        }
    }
}

void onMouseMove(double xpos_in, double ypos_in) {
    float xpos = static_cast<float>(xpos_in);
    float ypos = static_cast<float>(ypos_in);

    // first in, then do not react
    if (first_mouse) {
        last_x = xpos; last_y = ypos;
        first_mouse = false;
        return;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos; // reversed, because y-axis goes from bottom to top
    last_x = xpos; last_y = ypos;

    world.processMouseMovement(xoffset, yoffset);
}

void onMouseScroll(/* double xoffset, */double yoffset) {
    world.processMouseScroll(yoffset);
}