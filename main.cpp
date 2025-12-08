#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
// #include "camera_rotation.hpp" // for rotation camera
#include "camera.hpp" // for fps camera
#include "model.hpp"

#include <iostream>

glm::mat4 createProjectionMatrix(int projection_select, int width, int height, float zoom_deg);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f)); // Y + 1 to see the plane
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// projection: 0 - perspective | 1 - ortho
int projection_select = 0;

// build: 
// g++ main.cpp glad/glad.c stb_image.cpp -o program \
    -I./ \
    -I./glad \
    -I./glm \
    -I./assimp/include \
    -L./assimp/lib \
    -lassimp \
    -lminizip \
    -lz \
    -lglfw -lGL -ldl -lpthread

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // use the core-profile ==> we'll get access to a smaller subset of OpenGL features without backwards-compatible features we no longer need
    
    // ------- CREATE WINDOW ------- 

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);

    // allow window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

    // allow glfw to work with mouse movement
    glfwSetCursorPosCallback(window, mouse_callback);

    // allow glfw to work with mouse scroll
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture! (keep inside the window) our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ------------------------------
    

    // ------ CONFIGURE OPENGL ------

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))  
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    
    
    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // uncomment to use depth testing (z-buffer created automatically with GLFW)
    glEnable(GL_DEPTH_TEST);

    // -------------------------------


    // ----------- SHADERS -----------

    Shader ourShader("./shader.vert", "./shader.frag");

    // -------------------------------


    // --- COORDS: VERTICES, TEXTURES, COLORS ---

    Model torus((char*)"blender_assets/torus.obj");
    Model dodecahedron((char*)"blender_assets/dodecahedron.obj");
    Model plane((char*)"blender_assets/plane.obj");
    Model func;

    // -------------------------------


    // -------- LOAD TEXTURES --------

    // all done in Model class

    // -------------------------------


    // // -------- SET TEXTURES --------

    // all done in Model class

    // // -------------------------------


    // render loop
    while(!glfwWindowShouldClose(window))
    {
        // track frame time to set same speed of camera movement with different fps
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;  

        processInput(window);

        // render here ...
        {
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // 0.2f, 0.3f, 0.3f, 1.0f
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color buffer and the depth buffer 

            ourShader.use(); // render use of shader (update frame-dependent uniforms)

            // pass projection matrix to shader (note that in this case it could change every frame)
            glm::mat4 projection = createProjectionMatrix(projection_select, SCR_WIDTH, SCR_HEIGHT, camera.Zoom);
            ourShader.setMat4("projection", projection);

            // camera/view transformation
            glm::mat4 view = camera.GetViewMatrix();
            ourShader.setMat4("view", view);

            // --- render loaded models ---

            glm::mat4 modelTorus = glm::mat4(1.0f);
            modelTorus = glm::translate(modelTorus, glm::vec3(1.5f, 0.3f, 0.0f)); // move 
            modelTorus = glm::scale(modelTorus, glm::vec3(1.0f, 1.0f, 1.0f)); // scale
            ourShader.setMat4("model", modelTorus);

            torus.Draw(ourShader, glm::vec3(1.0f, 0.0f, 0.0f), false);

            glm::mat4 modelDodecahedron = glm::mat4(1.0f);
            modelDodecahedron = glm::translate(modelDodecahedron, glm::vec3(-1.5f, 0.7f, 0.0f)); // move
            modelDodecahedron = glm::scale(modelDodecahedron, glm::vec3(1.0f, 1.0f, 1.0f));	// scale
            ourShader.setMat4("model", modelDodecahedron);

            dodecahedron.Draw(ourShader, glm::vec3(0.0f, 1.0f, 0.0f), true);

            glm::mat4 modelPlane = glm::mat4(1.0f);
            modelPlane = glm::translate(modelPlane, glm::vec3(0.0f, 0.0f, 0.0f)); // move
            modelPlane = glm::scale(modelPlane, glm::vec3(5.0f, 5.0f, 5.0f)); // scale
            ourShader.setMat4("model", modelPlane);

            plane.Draw(ourShader, glm::vec3(0.0f, 0.0f, 1.0f), false);

            glm::mat4 modelFunc = glm::mat4(1.0f);
            modelFunc = glm::translate(modelFunc, glm::vec3(0.0f, 3.0f, 0.0f)); // move
            modelFunc = glm::rotate(modelFunc, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate
            modelFunc = glm::scale(modelFunc, glm::vec3(1.0f, 1.0f, 1.0f)); // scale
            ourShader.setMat4("model", modelFunc);

            func.Draw(ourShader, glm::vec3(1.0f, 0.0f, 1.0f), true);

            // --- --- ---
        }    
        // ...

        glfwSwapBuffers(window); // double buffering | swaping drawn image to go to the screen
        glfwPollEvents();    
    }

    // cleanup

    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        std::cout << "X: " << camera.Position.x << " " << "Y: " << camera.Position.y << " " << "Z: " << camera.Position.z << " " << "Zoom: " << camera.Zoom << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        std::cout << "X: " << camera.Position.x << " " << "Y: " << camera.Position.y << " " << "Z: " << camera.Position.z << " " << "Zoom: " << camera.Zoom << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        std::cout << "X: " << camera.Position.x << " " << "Y: " << camera.Position.y << " " << "Z: " << camera.Position.z << " " << "Zoom: " << camera.Zoom << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
        std::cout << "X: " << camera.Position.x << " " << "Y: " << camera.Position.y << " " << "Z: " << camera.Position.z << " " << "Zoom: " << camera.Zoom << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        std::cout << "X: " << camera.Position.x << " " << "Y: " << camera.Position.y << " " << "Z: " << camera.Position.z << " " << "Zoom: " << camera.Zoom << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        projection_select = 1;
        std::cout << "X: " << camera.Position.x << " " << "Y: " << camera.Position.y << " " << "Z: " << camera.Position.z << " " << "Zoom: " << camera.Zoom << std::endl;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        projection_select = 0;
        std::cout << "X: " << camera.Position.x << " " << "Y: " << camera.Position.y << " " << "Z: " << camera.Position.z << " " << "Zoom: " << camera.Zoom << std::endl;
    }

}

glm::mat4 createProjectionMatrix(int projection_select, int width, int height, float zoom_deg)
{
    float aspect = (float)width / (float)height;
    float fov = (float)glm::radians(zoom_deg);

    if (projection_select == 1)
    {
        return glm::ortho(-fov * aspect, fov * aspect, -fov, fov, 0.1f, 100.0f);
    }
    else
    {
        return glm::perspective(fov, aspect, 0.1f, 100.0f);
    }
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //         lower left corner of the window
    //         v  v 
    glViewport(0, 0, width, height);
}  


// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = (float)(xposIn);
    float ypos = (float)(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}