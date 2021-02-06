
// Recommended reading:
//
// https://www.lighthouse3d.com/tutorials/glsl-tutorial/uniform-blocks/

#include <cstdio>
#include <cstdlib>

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

const float pi2 = 6.28318530718f;

const char* vshaderSource = R"(

#version 300 es

precision highp float;

layout (std140) uniform Uniforms{
    int n_instances;
    float rotation;
    float radius;
    float size;
};

const vec2[3] vertices=vec2[](
vec2(-1.0, -1.0),
vec2( 0.0,  2.0),
vec2( 1.0, -1.0));

const float pi2=6.28318530718;

out float color;

void main() {

    float r = float(gl_InstanceID) * pi2 / float(n_instances) + rotation;

    float c = cos(r), s = sin(r);
    mat2 m = mat2(c, s, -s, c);

    vec2 v = vec2(c, s) * radius + m * vertices[gl_VertexID] * size;

    gl_Position = vec4(v.x, v.y, 0.0, 1.0);

    color = sin((r - rotation) / 2.0);
}

)";

const char* fshaderSource = R"(

#version 300 es

precision highp float;

in float color;

out vec4 fragColor;

void main() {

    fragColor=vec4(1.0, color, 1.0 - color,1.0);
}

)";

// Needs to be *exactly* the same as version in shader
struct Uniforms {
    int n_instances = 60;
    float rotation = 0.0f;
    float radius = 0.9f;
    float size = 0.025f;
};

GLuint compileShader(GLenum type, const char* source) {

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) return shader;

    // Failed!
    char log[256];
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    printf("Compile shader failed:%s\n", log);
    exit(1);
}

int main() {

    // Initialize GLFW and create window/GL context
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Awesome Raspberry Pi 400 Demo!", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // Create shader program
    GLuint vshader = compileShader(GL_VERTEX_SHADER, vshaderSource);
    GLuint fshader = compileShader(GL_FRAGMENT_SHADER, fshaderSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);
    glUseProgram(program);

    // Create uniforms buffer
    GLuint uniformsBuf = 0;
    glGenBuffers(1, &uniformsBuf);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniformsBuf);
    GLuint uniformsBlock = glGetUniformBlockIndex(program, "Uniforms");
    glUniformBlockBinding(program, uniformsBlock, 0);

    // Set initial states
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    Uniforms uniforms;
    bool demo = true;

    // Main loop - time to rock and roll!
    while (!glfwWindowShouldClose(window)) {

        // Tell ImGui we're starting a new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Clear the draw buffer
        glClear(GL_COLOR_BUFFER_BIT);

        // Show awesome ImGui demo widget
        if (demo) ImGui::ShowDemoWindow(&demo);

        // Allow user to update uniforms
        ImGui::SliderInt("Instances", &uniforms.n_instances, 6, 360);
        ImGui::SliderFloat("Rotation", &uniforms.rotation, 0.0f, pi2);
        ImGui::SliderFloat("Radius", &uniforms.radius, 0.0f, 1.0f);
        ImGui::SliderFloat("Size", &uniforms.size, 0.0f, 1.0f);

        // Upload uniform data to shader
        glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms), &uniforms, GL_STREAM_DRAW);

        // Render our stuff!
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, uniforms.n_instances);

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Flip
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
