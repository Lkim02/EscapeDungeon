
#include <string>
#include <vector>

#include "common.h"
#include "utils.h"

#include "application/Application.h"
#include "shader/Shader.h"
#include "camera/Camera.h"
#include "model/Model.h"


/*=== Application settings ===*/
GLuint vao = 0;
Shader* obj_shader = nullptr;
uint32_t viewport_width = 800, viewport_height = 600;

/*=== Time ===*/
float last_frame = 0.0f;
float delta_time = 0.0f;

/*=== Camera ===*/
Camera* camera = nullptr;
bool first_mouse = true;
float last_x = (float)viewport_width / 2.0f, last_y = (float)viewport_height / 2.0f;

/*=== Input ===*/
bool keyboard[128] = { false };

/*=== Light ===*/
GLuint light_vao = 0;
Shader* light_shader = nullptr;
glm::vec3 light_pos(1.2f, 1.0f, 2.0f);
glm::vec3 light_color;

/*=== Model ===*/
std::vector<Model*> models;

/*=== Callback Handler ===*/
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
        }
        else if (action == GLFW_RELEASE) {
            keyboard[key] = false;
        }
    }
}

void processKeyboardInput() {
    if (keyboard[GLFW_KEY_W]) camera->processKeyboard(FORWARD, delta_time);
    if (keyboard[GLFW_KEY_S]) camera->processKeyboard(BACKWARD, delta_time);
    if (keyboard[GLFW_KEY_A]) camera->processKeyboard(LEFT, delta_time);
    if (keyboard[GLFW_KEY_D]) camera->processKeyboard(RIGHT, delta_time);
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

    camera->processMouseMovement(xoffset, yoffset, true);
}
// not in use
void onMouseScroll(/* double xoffset, */double yoffset) {
    camera->processMouseScroll(yoffset);
}


/*=== Preparations ===*/
void prepareSingleBuffer() {
    float vtx[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    float colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    GLuint pos_vbo = 0, color_vbo = 0;
    GL_CALL(glGenBuffers(1, &pos_vbo));
    GL_CALL(glGenBuffers(1, &color_vbo));
    //GLuint vbos[] = { 0, 0, 0 };
    //GL_CALL(glGenBuffers(3, vbos));

    vao = 0; // 一个vao就可以绑定多个vbo，可以对应一整个mesh
    GL_CALL(glGenVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));

    /* Position buffer */
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pos_vbo)); // 绑定了vbo之后，下面的属性描述才会与此vbo相关
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vtx), vtx, GL_STATIC_DRAW));
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

    /* Color buffer */
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, color_vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW));
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

    /* 操作完成，可以解绑一下 */
    GL_CALL(glBindVertexArray(0)); // TODO:可以写成宏

    //GL_CALL(glDeleteBuffers(3, vbos));
    GL_CALL(glDeleteBuffers(1, &pos_vbo));
}

void prepareInterleavedBuffer() {
    float pos_color[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 0.4f, 0.5f, 0.6f,
         0.8f,  0.8f, 0.0f, 0.2f, 0.3f, 0.4f,
         0.8f,  0.0f, 0.0f, 0.7f, 0.8f, 0.9f
    };

    unsigned int index[] = {
        0, 1, 2,
        2, 3, 4,
        4, 5, 0
    };

    /* Create VBO */
    GLuint pos_color_vbo = 0; // interleaved vbo
    GL_CALL(glGenBuffers(1, &pos_color_vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pos_color_vbo)); // bind vbo
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(pos_color), pos_color, GL_STATIC_DRAW)); // send data

    /* Create EBO */
    GLuint ebo = 0;
    GL_CALL(glGenBuffers(1, &ebo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo)); // bind ebo
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW)); // send data

    /* Create VAO */
    GL_CALL(glGenVertexArrays(1, &vao));

    /* Bind VBO & EBO with VAO */
    setVAO(vao, {
        /* Position vao */
        GL_CALL(glEnableVertexAttribArray(0));
        GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0)); // connect current vao and vbo

        /* Color vao */
        GL_CALL(glEnableVertexAttribArray(1));
        GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))));

        /* ebo */
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo)); // connect ebo and vao
        });

    //GL_CALL(glDeleteBuffers(1, &pos_color_vbo));
}

void prepareCubeBufferEBO() {
    //float cube_vertice[] = {
    //    -0.5f, -0.5f, -0.5f, /* 0: - - - */
    //     0.5f, -0.5f, -0.5f, /* 1: + - - */
    //    -0.5f,  0.5f, -0.5f, /* 2: - + - */
    //     0.5f,  0.5f, -0.5f, /* 3: + + - */
    //    -0.5f, -0.5f,  0.5f, /* 4: - - + */
    //     0.5f, -0.5f,  0.5f, /* 5: + - + */
    //    -0.5f,  0.5f,  0.5f, /* 6: - + + */
    //     0.5f,  0.5f,  0.5f, /* 7: + + + */
    //};
    //float cube_vertice[] = {
    //    // Position            // Normal
    //    -0.5f, -0.5f, -0.5f,  -0.577f, -0.577f, -0.577f,  // 0
    //     0.5f, -0.5f, -0.5f,   0.577f, -0.577f, -0.577f,  // 1
    //    -0.5f,  0.5f, -0.5f,  -0.577f,  0.577f, -0.577f,  // 2
    //     0.5f,  0.5f, -0.5f,   0.577f,  0.577f, -0.577f,  // 3
    //    -0.5f, -0.5f,  0.5f,  -0.577f, -0.577f,  0.577f,  // 4
    //     0.5f, -0.5f,  0.5f,   0.577f, -0.577f,  0.577f,  // 5
    //    -0.5f,  0.5f,  0.5f,  -0.577f,  0.577f,  0.577f,  // 6
    //     0.5f,  0.5f,  0.5f,   0.577f,  0.577f,  0.577f,  // 7
    //};
    float cube_vertice[] = {
        // Position            // Normal
        -0.5f, -0.5f, -0.5f,  -1.0f, -1.0f, -2.0f,  // 0
         0.5f, -0.5f, -0.5f,   2.0f, -2.0f, -1.0f,  // 1
        -0.5f,  0.5f, -0.5f,  -2.0f,  2.0f, -1.0f,  // 2
         0.5f,  0.5f, -0.5f,   1.0f,  1.0f, -2.0f,  // 3
        -0.5f, -0.5f,  0.5f,  -2.0f, -2.0f,  1.0f,  // 4
         0.5f, -0.5f,  0.5f,   1.0f, -1.0f,  2.0f,  // 5
        -0.5f,  0.5f,  0.5f,  -1.0f,  1.0f,  2.0f,  // 6
         0.5f,  0.5f,  0.5f,   2.0f,  2.0f,  1.0f,  // 7
    };
    unsigned int cube_index[] = {
        4, 5, 6,  6, 5, 7, /* +z */
        5, 1, 7,  7, 1, 3, /* +x */
        1, 0, 3,  3, 0, 2, /* -z */
        0, 4, 2,  2, 4, 6, /* -x */
        6, 7, 2,  2, 7, 3, /* +y */
        5, 4, 1,  1, 4, 0, /* -y */
    };

    /* Create VBO */
    GLuint cube_vbo = 0; // interleaved vbo
    GL_CALL(glGenBuffers(1, &cube_vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, cube_vbo)); // bind vbo
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertice), cube_vertice, GL_STATIC_DRAW)); // send data

    /* Create EBO */
    GLuint cube_ebo = 0;
    GL_CALL(glGenBuffers(1, &cube_ebo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo)); // bind ebo
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_index), cube_index, GL_STATIC_DRAW)); // send data

    /* Create VAO */
    GL_CALL(glGenVertexArrays(1, &vao));
    GL_CALL(glGenVertexArrays(1, &light_vao));

    /* Bind VBO & EBO with VAO */
    setVAO(vao, {
        /* Position vao */
        GL_CALL(glEnableVertexAttribArray(0));
        GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0)); // connect current vao with vbo
        /* Normal vao */
        GL_CALL(glEnableVertexAttribArray(1));
        GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))));

        /* ebo */
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo)); // connect current vao with ebo
        });

    setVAO(light_vao, {
        /* Use the same VBO */
        GL_CALL(glEnableVertexAttribArray(0));
        GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));

        /* ebo */
        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo)); // connect current vao with ebo
        });
}

void prepareCubeBuffer() {
    //float cube_vertice[] = {
    //    -0.5f, -0.5f, -0.5f, /* 0: - - - */
    //     0.5f, -0.5f, -0.5f, /* 1: + - - */
    //    -0.5f,  0.5f, -0.5f, /* 2: - + - */
    //     0.5f,  0.5f, -0.5f, /* 3: + + - */
    //    -0.5f, -0.5f,  0.5f, /* 4: - - + */
    //     0.5f, -0.5f,  0.5f, /* 5: + - + */
    //    -0.5f,  0.5f,  0.5f, /* 6: - + + */
    //     0.5f,  0.5f,  0.5f, /* 7: + + + */
    //};
    //float cube_vertice[] = {
    //    // Position            // Normal
    //    -0.5f, -0.5f, -0.5f,  -0.577f, -0.577f, -0.577f,  // 0
    //     0.5f, -0.5f, -0.5f,   0.577f, -0.577f, -0.577f,  // 1
    //    -0.5f,  0.5f, -0.5f,  -0.577f,  0.577f, -0.577f,  // 2
    //     0.5f,  0.5f, -0.5f,   0.577f,  0.577f, -0.577f,  // 3
    //    -0.5f, -0.5f,  0.5f,  -0.577f, -0.577f,  0.577f,  // 4
    //     0.5f, -0.5f,  0.5f,   0.577f, -0.577f,  0.577f,  // 5
    //    -0.5f,  0.5f,  0.5f,  -0.577f,  0.577f,  0.577f,  // 6
    //     0.5f,  0.5f,  0.5f,   0.577f,  0.577f,  0.577f,  // 7
    //};
    float cube_vertice[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    /* Create VBO */
    GLuint cube_vbo = 0; // interleaved vbo
    GL_CALL(glGenBuffers(1, &cube_vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, cube_vbo)); // bind vbo
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertice), cube_vertice, GL_STATIC_DRAW)); // send data

    /* Create VAO */
    GL_CALL(glGenVertexArrays(1, &vao));
    GL_CALL(glGenVertexArrays(1, &light_vao));

    /* Bind VBO & EBO with VAO */
    setVAO(vao, {
        /* Position vao */
        GL_CALL(glEnableVertexAttribArray(0));
        GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0)); // connect current vao with vbo
        /* Normal vao */
        GL_CALL(glEnableVertexAttribArray(1));
        GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
        /* Texture vao */
        GL_CALL(glEnableVertexAttribArray(2));
        GL_CALL(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));
        });

    setVAO(light_vao, {
        /* Use the same VBO */
        GL_CALL(glEnableVertexAttribArray(0));
        GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
        });
}

glm::vec3 object_positon[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f)
};

void prepareModel() {
    Model* backpack = new Model("assets/objects/backpack/backpack.obj");
    models.push_back(backpack);
}

void prepareShader() {
    obj_shader = new Shader("assets/shaders/loadobj_nolight.vert", "assets/shaders/loadobj_nolight.frag");
    light_shader = new Shader("assets/shaders/light_item.vert", "assets/shaders/light_item.frag");
}

void prepareTexture() {

}


/*=== Render ===*/
glm::mat4 model, view, projection;
glm::mat3 norm_model;
void render() {
    /* Calculate time */
    float current_frame = static_cast<float>(glfwGetTime());
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    /* Clear canvas */
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    /* === Global MVP === */
    projection = glm::perspective(glm::radians(camera->zoom), (float)viewport_width / (float)viewport_height, 0.1f, 100.0f);
    view = camera->getViewMatrix();

    /* === Object Rendering === */
    obj_shader->begin();
    // light
 //   light_color = glm::vec3(0.7f);
    //glm::vec3 diffuse_color = light_color * glm::vec3(0.5f); // decrease the influence
    //glm::vec3 ambient_color = diffuse_color * glm::vec3(0.2f); // low influence
    //obj_shader->setVec3f("light.position", glm::value_ptr(light_pos));
    //obj_shader->setVec3f("light.ambient", glm::value_ptr(ambient_color));
    //obj_shader->setVec3f("light.diffuse", glm::value_ptr(diffuse_color));
    //obj_shader->setVec3f("light.specular", 1.0f, 1.0f, 1.0f);
    // camera
    //obj_shader->setVec3f("viewPos", glm::value_ptr(camera->position));
    // object
    //obj_shader->setFloat("material.shininess", 32.0f);
    // 
    // mvp
    obj_shader->setMat4f("view", glm::value_ptr(view));
    obj_shader->setMat4f("projection", glm::value_ptr(projection));

    model = glm::mat4(1.0f); // model
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    obj_shader->setMat4f("model", glm::value_ptr(model));
    //norm_model = glm::mat3(model); // norm_model
    //norm_model = glm::transpose(glm::inverse(norm_model));
    //obj_shader->setMat3f("normModel", glm::value_ptr(norm_model));

    models[0]->draw(obj_shader);

    obj_shader->end();

    /* === Light Rendering === */
 //   light_shader->begin();
    //light_shader->setMat4f("view", glm::value_ptr(view));
    //light_shader->setMat4f("projection", glm::value_ptr(projection));
 //   
 //   GL_CALL(glBindVertexArray(light_vao));
 //   model = glm::mat4(1.0f);
 //   model = glm::translate(model, light_pos);
 //   model = glm::scale(model, glm::vec3(0.2f)); // let the light cube be smaller
 //   light_shader->setMat4f("model", glm::value_ptr(model));
 //   //GL_CALL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));
 //   GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
 //   GL_CALL(glBindVertexArray(0));

 //   light_shader->end();
}

void drawCube(float& current_time, glm::mat4& view, glm::mat4& projection) {
    obj_shader->begin();

    // light
    light_color.x = sin(current_time * 2.0f);
    light_color.y = sin(current_time * 0.7f);
    light_color.z = sin(current_time * 1.3f);
    glm::vec3 diffuse_color = light_color * glm::vec3(0.5f); // decrease the influence
    glm::vec3 ambient_color = diffuse_color * glm::vec3(0.2f); // low influence
    obj_shader->setVec3f("light.position", glm::value_ptr(light_pos));
    obj_shader->setVec3f("light.ambient", glm::value_ptr(ambient_color));
    obj_shader->setVec3f("light.diffuse", glm::value_ptr(diffuse_color));
    obj_shader->setVec3f("light.specular", 1.0f, 1.0f, 1.0f);
    // camera
    obj_shader->setVec3f("viewPos", glm::value_ptr(camera->position));
    // object
    // material
    obj_shader->setVec3f("material.ambient", 1.0f, 0.5f, 0.31f);
    obj_shader->setVec3f("material.diffuse", 1.0f, 0.5f, 0.31f);
    obj_shader->setVec3f("material.specular", 0.5f, 0.5f, 0.5f);
    obj_shader->setFloat("material.shininess", 32.0f);
    // mvp
    obj_shader->setMat4f("view", glm::value_ptr(view));
    obj_shader->setMat4f("projection", glm::value_ptr(projection));

    GL_CALL(glBindVertexArray(vao));
    for (int i = 0; i < 1; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, object_positon[i]);
        obj_shader->setMat4f("model", glm::value_ptr(model));

        norm_model = glm::mat3(model);
        norm_model = glm::transpose(glm::inverse(norm_model));
        obj_shader->setMat3f("normModel", glm::value_ptr(norm_model));

        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
        //GL_CALL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0));
    }
    GL_CALL(glBindVertexArray(0));

    obj_shader->end();
}


int main() {
    bool status = APP->init(viewport_width, viewport_height);
    if (!status) return -1;

    /* Set Callback Monitor */
    APP->setResizeCallback(onResize);
    APP->setKeyboardCallback(onKeyChange);
    APP->setMouseMoveCallback(onMouseMove);
    APP->setMouseScrollCallback(onMouseScroll);

    GL_CALL(glViewport(0, 0, viewport_width, viewport_height));
    GL_CALL(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));

    //prepareInterleavedBuffer();
    //prepareCubeBuffer();
    prepareModel();
    prepareShader();

    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    // window loop
    while (APP->update()) {
        /* React to Input */
        processKeyboardInput();

        /* Render */
        render();
    }

    APP->destroy();
    delete camera;
    delete obj_shader;
    delete light_shader;
    for (auto model : models)
        delete model;

    std::cout << "cleared" << std::endl;

    return 0;
}