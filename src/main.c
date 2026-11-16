// Template application: opens a window with an OpenGL 4.1 core context and
// draws a rotating triangle.
//
//   F11 or Alt+Enter   toggle windowed / fullscreen
//   Escape             quit

#include <glad/gl.h>

// glad already provides the OpenGL declarations; stop GLFW from including the
// system OpenGL header as well.
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_TITLE  "3D App"
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

// State that the GLFW callbacks need. A pointer to it is attached to the
// window with glfwSetWindowUserPointer().
typedef struct App {
    bool fullscreen;
    // Position and size of the window before it went fullscreen, so that it
    // can be restored afterwards.
    int windowed_x, windowed_y;
    int windowed_width, windowed_height;
} App;

// ---------------------------------------------------------------------------
// Shaders
// ---------------------------------------------------------------------------

static const char *const VERTEX_SHADER_SOURCE =
    "#version 410 core\n"
    "layout(location = 0) in vec2 a_position;\n"
    "layout(location = 1) in vec3 a_color;\n"
    "uniform float u_angle;\n"
    "uniform float u_aspect;\n"
    "out vec3 v_color;\n"
    "void main() {\n"
    "    float c = cos(u_angle);\n"
    "    float s = sin(u_angle);\n"
    "    vec2 p = vec2(a_position.x * c - a_position.y * s,\n"
    "                  a_position.x * s + a_position.y * c);\n"
    "    p.x /= u_aspect;\n" // keep the triangle undistorted in a wide window
    "    gl_Position = vec4(p, 0.0, 1.0);\n"
    "    v_color = a_color;\n"
    "}\n";

static const char *const FRAGMENT_SHADER_SOURCE =
    "#version 410 core\n"
    "in vec3 v_color;\n"
    "out vec4 frag_color;\n"
    "void main() {\n"
    "    frag_color = vec4(v_color, 1.0);\n"
    "}\n";

static GLuint compile_shader(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof log, NULL, log);
        fprintf(stderr, "Shader compilation failed:\n%s\n", log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint create_program(const char *vertex_source, const char *fragment_source) {
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    if (!vertex_shader || !fragment_shader) {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // The program keeps the compiled code; the shader objects are no longer needed.
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLint ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(program, sizeof log, NULL, log);
        fprintf(stderr, "Shader program linking failed:\n%s\n", log);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

// ---------------------------------------------------------------------------
// Window handling
// ---------------------------------------------------------------------------

static void toggle_fullscreen(GLFWwindow *window) {
    App *app = glfwGetWindowUserPointer(window);

    if (!app->fullscreen) {
        // Wayland does not let applications query or set the window position.
        if (glfwGetPlatform() != GLFW_PLATFORM_WAYLAND) {
            glfwGetWindowPos(window, &app->windowed_x, &app->windowed_y);
        }
        glfwGetWindowSize(window, &app->windowed_width, &app->windowed_height);

        // Use the monitor's current video mode, so the display does not have
        // to switch resolution ("borderless fullscreen").
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        glfwSetWindowMonitor(window, NULL, app->windowed_x, app->windowed_y,
                             app->windowed_width, app->windowed_height, 0);
    }
    app->fullscreen = !app->fullscreen;

    // Some drivers reset the swap interval when the window mode changes.
    glfwSwapInterval(1);
}

static void on_key(GLFWwindow *window, int key, int scancode, int action, int mods) {
    (void)scancode;
    if (action != GLFW_PRESS) {
        return;
    }

    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else if (key == GLFW_KEY_F11 || (key == GLFW_KEY_ENTER && (mods & GLFW_MOD_ALT))) {
        toggle_fullscreen(window);
    }
}

static void on_glfw_error(int code, const char *description) {
    fprintf(stderr, "GLFW error 0x%X: %s\n", (unsigned)code, description);
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

int main(void) {
    glfwSetErrorCallback(on_glfw_error);
    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    // OpenGL 4.1 core is the newest version macOS offers, so it is the common
    // denominator of all three platforms. macOS also insists on the
    // forward-compatible flag.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    App app = {0};
    glfwSetWindowUserPointer(window, &app);
    glfwSetKeyCallback(window, on_key);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    if (!gladLoadGL(glfwGetProcAddress)) {
        fprintf(stderr, "Failed to load the OpenGL functions\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    printf("OpenGL %s\n", (const char *)glGetString(GL_VERSION));
    printf("Renderer: %s\n", (const char *)glGetString(GL_RENDERER));

    GLuint program = create_program(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);
    if (!program) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    GLint angle_location = glGetUniformLocation(program, "u_angle");
    GLint aspect_location = glGetUniformLocation(program, "u_aspect");

    // An equilateral triangle centred on the origin. Each vertex is
    // x, y, red, green, blue.
    static const float vertices[] = {
         0.000f,  0.70f,   1.0f, 0.2f, 0.2f,
        -0.606f, -0.35f,   0.2f, 1.0f, 0.2f,
         0.606f, -0.35f,   0.2f, 0.4f, 1.0f,
    };

    // The vertex array object remembers the vertex layout set up below.
    GLuint vertex_array = 0;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    GLuint vertex_buffer = 0;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

    const GLsizei stride = 5 * sizeof(float);
    glEnableVertexAttribArray(0); // a_position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(1); // a_color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)(2 * sizeof(float)));

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // The framebuffer is measured in pixels and can be larger than the
        // window size on high-DPI displays. It is 0x0 while minimised.
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        if (width > 0 && height > 0) {
            glViewport(0, 0, width, height);
            glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(program);
            glUniform1f(angle_location, (float)glfwGetTime());
            glUniform1f(aspect_location, (float)width / (float)height);
            glBindVertexArray(vertex_array);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &vertex_buffer);
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
