#include "app_window.h"
#include "../AppCore.h"
#include "../Blueprints.h"
#include "../Image.h"
#include "../Input.h"
#include "../../Resources/res.h"


std::vector<App_Window*> App_Window::wins32;

App_Window::App_Window(const char* window_name, int width, int height, GLFWwindow* srd_window):
    windowName(window_name),
	width(width),
	height(height),
    background_col(0),
    bg_image(nullptr)
{
   // glfwSetErrorCallback(glfw_error_callback);
    window = glfwCreateWindow(width, height, windowName, NULL, srd_window);

    if (window == NULL)
        ASSERT("There is no window or your GPU isn't working properly", true);

    glfwMakeContextCurrent(window);
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    ScreenSize.x = mode->width, ScreenSize.y = mode->height;
}

App_Window:: ~App_Window() 
{
    glfwDestroyWindow(window);
    DE_NUL(bg_image);
    //event::Fire_Once(On_Window_Close(this));
}

void App_Window::OnWindowAwake()
{
    // Window Icon
    Image GetIcon((pixel_uc*)_icon_data,icon_size,4);
    GetIcon.overrideColors(COLOR_WHITE,COLOR_RED);
    GLFWimage icons[1];
    icons[0].pixels = GetIcon.Get_Data();
    icons[0].width  = GetIcon.Get_Width();
    icons[0].height = GetIcon.Get_Height();
    glfwSetWindowIcon(window, 1, icons);
}


void App_Window::OnUpdate()
{
    glfwMakeContextCurrent(window);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(COL8_TO_F(((uint8_t*)&background_col)[0]), COL8_TO_F(((uint8_t*)&background_col)[1]), COL8_TO_F(((uint8_t*)&background_col)[2]), 1.0f);
    RawQuadTexture();
}

void App_Window::OnInput()
{

}

void App_Window::SetBackgroundColor(uint8_t red, uint8_t green ,uint8_t blue)
{
    uint8_t* c = (uint8_t*)&background_col;
    c[0] = red;
    c[1] = green;
    c[2] = blue;
    c[3] = 144;
}

void App_Window::Set_texture_id(Image* im)
{
    bg_image = im;
}

// draw textured quad opengl
void App_Window::RawQuadTexture()
{
    if (bg_image) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, bg_image->Get_GPU_ID());

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, 1.0f);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-1.0f, -1.0f);

        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(1.0f, -1.0f);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, 1.0f);
        glEnd();

        //glBegin(GL_LINES);
        //glVertex2f(.25, 0.25);
        //glVertex2f(.75, .75);
        //glEnd();
    }
}
