#ifndef PROJ_UI_HPP
#define PROJ_UI_HPP


#include <vector>
#include <functional>
#include <GL/glcorearb.h>
#include "shader.hpp"
#include "textureloader.hpp"

//class UIControl {
//public:
//    enum ControlType {
//        BUTTON
//    };
//
//    UIControl() = delete;
//    UIControl(UIControl&) = delete;
//    UIControl(UIControl&&) = delete;
//
//    UIControl(std::function<void()>& clickCallback);
//};

class UI {
public:
    UI() = delete;
    UI(UI&) = delete;
    UI(UI&&) = delete;
    UI(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture);


    /**
     * Renders the UI in its current state
     */
    void render();

    /**
     * Recomputes the positions and scales of UI elements to keep them looking consistent.
     * To be called when the window is resized.
     *
     * @param windowSize
     */
    void recomputeLayout(glm::vec2 windowSize);

    /**
     *
     * @param mouseCoordinates
     * @return true if the UI "caught" the mouse click
     */
//    bool processMouseClick(glm::vec2 mouseCoordinates);

    /**
     * Changes the UI state based on the given keycode
     * @param keyCode
     * @return true if the UI "caught" the mouse click
     */
    bool processKeyboardEvent(int keyCode);

private:
//    std::vector<UIControl> controls;

    // Layout stuff
    glm::vec2 origin;
    glm::mat4 transformationMatrix;
    glm::vec2 windowSize;

    // Rendering stuff
    GLuint vao;
    GLuint vbo;
    GLuint ibo;

    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> texture;
    GLuint positionAttribute;
    GLuint textureCoordinateAttribute;
    GLuint timeElapsedUniform;
    GLuint modelViewProjectionUniform;

    glm::vec3 position;
    glm::vec3 direction;

};


#endif //PROJ_UI_HPP
