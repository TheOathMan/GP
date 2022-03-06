#include "GP src/Blueprints.h"
#include "GP src/App_window/Main_Win.h"
#include "GP src/Event.h"
//#pragma comment(lib, "glfw3")

void* operator new (std::size_t sz) {
    //Debug("Allocationg: " << sz);
    // avoid std::malloc(0) which may return nullptr on success
    if (sz == 0)
        ++sz;

    if (void* ptr = std::malloc(sz))
        return ptr;

    throw std::bad_alloc{};
    // normally throw bad alloc
}
auto a =0x15B98;

//void* operator new(size_t size)
//{
//    void* ptr;
//    ptr = malloc(size);
//    return ptr;
//}

void operator delete(void* ptr) noexcept {
    std::free(ptr);
}


int main(int, char**)
{
    glfwSetErrorCallback([](int error, const char* description) {fprintf(stderr, "Glfw Error %d: %s\n", error, description); });
    if (!glfwInit())
        return 1;

    Main_Win* MainWindow = new Main_Win("font Printer", 1300, 700);
    App_Window* ImagePreviewWin = new App_Window("glyph preview", 500, 500, MainWindow->get_window());
    glfwHideWindow(ImagePreviewWin->get_window());

    App_Window::wins32.push_back(MainWindow);
    App_Window::wins32.push_back(ImagePreviewWin);
    glfwSwapInterval(1);

    // windows start
    //for (auto& WINS : App_Window::wins32)
    MainWindow->OnWindowAwake();

    while (MainWindow->IsWindowOpen())
    {
        // windows update
        for (auto& WINS : App_Window::wins32) {
            WINS->OnUpdate();
            glfwSwapBuffers(WINS->get_window());
        }

        // windows input
        for (auto& WINS : App_Window::wins32) {
            WINS->OnInput();
        }

        if (!ImagePreviewWin->IsWindowOpen()) {
            glfwHideWindow(ImagePreviewWin->get_window());
            
        }
        
        // wake up sec win
        Event::Notify_Once(OnPreviewWinInitialized(ImagePreviewWin));

        glfwPollEvents();
    }

    //destruction
    for (auto& WINS : App_Window::wins32) {
        delete WINS;
    }

    glfwTerminate();

    return 0;
}
