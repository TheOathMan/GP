#include "GP src/Blueprints.h"
#include "GP src/App_window/Main_Win.h"
#include "GP src/Event.h"
#include "GP src/Input.h"

// void* operator new (std::size_t sz) {
//     if (sz == 0)
//         ++sz;

//     if (void* ptr = std::malloc(sz))
//         return ptr;

//     throw std::bad_alloc{};
//     // normally throw bad alloc
// }
// void operator delete(void* ptr) noexcept {
//     std::free(ptr);
// }


int main(int, char**)
{
    glfwSetErrorCallback([](int error, const char* description) {fprintf(stderr, "Glfw Error %d: %s\n", error, description); });
    if (!glfwInit())
        return 1;

    Main_Win* MainWindow = new Main_Win("Glyph Printer", 1300, 700);
    App_Window* ImagePreviewWin = new App_Window("Glyph Preview", 500, 500, MainWindow->get_window());
    glfwHideWindow(ImagePreviewWin->get_window());

    App_Window::wins32.push_back(MainWindow);
    App_Window::wins32.push_back(ImagePreviewWin);
    glfwSwapInterval(1);

    // windows start
    //for (auto& WINS : App_Window::wins32)
   // MainWindow->OnWindowAwake();
    for (auto& WINS : App_Window::wins32) {
        WINS->OnWindowAwake();
    }

    while (MainWindow->IsWindowOpen())
    {
        //windows update
        for (auto& WINS : App_Window::wins32) {
            WINS->OnUpdate();
            glfwSwapBuffers(WINS->get_window());
        }

        //windows input
        if(Input::is_new_input())
            for (auto& WINS : App_Window::wins32) {
                WINS->OnInput();
            }

        if (!ImagePreviewWin->IsWindowOpen()) {
            glfwHideWindow(ImagePreviewWin->get_window());
        }
        
        //wake up sec win
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
