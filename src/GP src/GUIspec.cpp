#include "GUIspec.h"
#include "FontData.h"
#include "Event.h"
#include "defines.h"
#include "FileHandle.h"
#include "../NMC/GLFW/glfw3.h"
#include "../NMC/imgui/imgui_impl_glfw.h"
#include "../NMC/imgui/imgui_impl_opengl2.h"
#include "../NMC/tinydialog/tinyfiledialogs.h"
#include "../NMC/imgui/imgui_internal.h"
#ifdef NO_STATIC_FONT
#else
#define INSER_RESOURCES
#include "../Resources/res.h"
#endif
//#include "../NMC/dirent/dirent.h"

namespace GpGUI {
    using namespace ImGui;
    bool BufferingBar(const char* label, float value, const ImVec2& size_arg) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size = size_arg;
        size.x -= style.FramePadding.x * 2;

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
        const ImU32 bg  = ImGui::GetColorU32(ImGuiCol_Button);
        // Render
        const float circleStart = size.x;
        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg);
        window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), col);
        return true;
    }

    bool Spinner(const char* label, float radius, int thickness, const ImU32& gl_color) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        // Render
        window->DrawList->PathClear();

        float t_time = glfwGetTime();

        int num_segments = 30;
        int start = abs(ImSin(t_time * 1.8f) * (num_segments - 5));

        const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
        const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

        const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

        for (int i = 0; i < num_segments; i++) {
            const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
            window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + t_time * 8) * radius , centre.y + ImSin(a + t_time * 8) * radius));
        }
        window->DrawList->PathStroke(gl_color, false, thickness);
        return true;
    }

    bool CheckBox(const char* name, bool* val) {
    
        TextUnformatted(name); 			
        SameLine(GetWindowSize().x / 2.0f);		

        ImVec2 avR = GetContentRegionAvail();
        ImVec2 curPos = GetCursorScreenPos();

       GetWindowDrawList()->AddRectFilled(curPos, ImVec2(curPos.x + avR.x, curPos.y + 30.0f) ,val ? IM_COL32(28, 28, 28, 200) : IM_COL32(19, 19, 19, 200)); 			
       PushID(name);  													
       bool rtval = Checkbox("ON", val);											
       PopID();
       return rtval;
    }

    bool LinkedCheckBox(const char* name, int* val, int LinkVal) {

        bool tr = false;
        tr = *val == LinkVal ? true : false;

        TextUnformatted(name);
        SameLine(GetWindowSize().x / 2.0f);

        ImVec2 avR = GetContentRegionAvail();
        ImVec2 curPos = GetCursorScreenPos();

        GetWindowDrawList()->AddRectFilled(curPos, ImVec2(curPos.x + avR.x, curPos.y + 30.0f), tr ? IM_COL32(28, 28, 28, 200) : IM_COL32(19, 19, 19, 200));
        PushID(name);
        bool rtval = Checkbox("ON", &tr);
        if (rtval) *val = LinkVal;
        PopID();
        return rtval;
    }

    void SPINE(float x, float y, float bx, float by) {
        ImGui::ItemSize(ImVec2(bx, by)); ImGui::Separator(); ImGui::ItemSize(ImVec2(x, y));
    }
}


ImGuiWindowFlags WindowSetting(
    bool no_titlebar = true,
    bool no_scrollbar = true,
    bool no_menu = true,
    bool no_move = true,
    bool no_resize = false,
    bool no_collapse = true,
    bool no_nav = true,
    bool no_background = false,
    bool no_bring_to_front = true,
    bool no_save_load = true)
{
    ImGuiWindowFlags window_flags = 0;
    //ImGuiWindowFlags sasdxs = 0;
    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (no_save_load)       window_flags |= ImGuiWindowFlags_NoSavedSettings;
    //ImGui::ImGuiWindow
    return window_flags;
}




GUIspec::GUIspec(GLFWwindow* window) {
   // event::Connect<Drop_Callback_Handeler>()

    //GP_Print("%i", styledata_bin[0]);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();

    io = ImGui::GetIO(); //(void)io;
    
#ifdef NO_STATIC_FONT
    size_t fsize;
    FileHandle f("Font/mainfont.ttf","rb",&fsize);
    if(!f.file_data)
        tinyfd_messageBox("Missing files","Font file is missing in Font/mainfont.ttf","ok","error",0);
        //assert(false);

    _main_font = io.Fonts->AddFontFromMemoryTTF(f.file_data, fsize, 24.0f);
    _main_font_Big = io.Fonts->AddFontFromMemoryTTF(f.file_data, fsize, 30.0f);

#else
    void* mainfontData1 = malloc(l_10646_size);
    void* mainfontData2 = malloc(l_10646_size);
    if (mainfontData1) memcpy(mainfontData1, _l_10646_data, l_10646_size);
    if (mainfontData2) memcpy(mainfontData2, _l_10646_data, l_10646_size);

    _main_font = io.Fonts->AddFontFromMemoryTTF(mainfontData1, l_10646_size, 24.0f);
    _main_font_Big = io.Fonts->AddFontFromMemoryTTF(mainfontData2, l_10646_size, 30.0f);
#endif
    window_flags = WindowSetting();
    ImGui::GetStyle() = * ((ImGuiStyle*)_styledata_data);
    style = &GImGui->Style;
}

void GUIspec::GUI_OnFrameStart()
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    //ImGui::GetStyle() = *style;
}

void GUIspec::GUI_OnFrameEnd()
{
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void GUIspec::GUI_OnDestruction()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

//std::qui<char*> recentFonts;
std::deque<std::string> OpenRecentP;
std::deque<std::string> DropRecentP;

void ShowMenuFile()
{
    if (ImGui::MenuItem("Clean")) { 
        Event::Notify(OnAllFontsClear());

    }
    if (ImGui::MenuItem("Open", "Ctrl+O")) {
        char const* lFilterPatterns[2] = { "*.ttf", "*.otf" };
        const char* selection = tinyfd_openFileDialog("Select Font File", "C:\\", 2, lFilterPatterns, NULL, true);


        if (!selection) return;
        size_t f_sz = strlen(selection) + 1;
        char* tempstr = (char*)malloc(f_sz);
        if (tempstr) strcpy_s(tempstr, f_sz, selection);

        char* next_p = nullptr;
        char* tkn = strtok_s(tempstr, "|", &next_p);

        int count = 0;
        std::vector<const char*> paths;

        while (tkn) {
            paths.push_back(tkn);
            tkn = strtok_s(NULL, "|", &next_p);
            count++;
        }
        Event::Notify(OnFontsLoading(count, paths.data() ));
        free(tempstr);
    }
    if (ImGui::BeginMenu("Open Recent")) 
    {
        if (OpenRecentP.size()==0)
            ImGui::MenuItem("(EMPTY)", NULL, false, false);
        else
        {      
            for (size_t i = 0; i < OpenRecentP.size(); i++)
            {
                const char* fn;    
                const char* cctrfp =OpenRecentP.at(i).data();
                for (fn = cctrfp + strlen(cctrfp); fn > cctrfp && fn[-1] != '\\' && fn[-1] != '/'; fn--) {};
                if(ImGui::MenuItem(fn))
                { 
                     Event::Notify(OnFontsLoading(1, &cctrfp ));
                }
            }
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Drop Recent")) 
    {
        if (DropRecentP.size()==0)
            ImGui::MenuItem("(EMPTY)", NULL, false, false);
        else
        {      
            for (size_t i = 0; i < DropRecentP.size(); i++)
            {
                const char* fn;    
                const char* cctrfp =DropRecentP.at(i).data();
                for (fn = cctrfp + strlen(cctrfp); fn > cctrfp && fn[-1] != '\\' && fn[-1] != '/'; fn--) {};
                if(ImGui::MenuItem(fn))
                { 
                     Event::Notify(OnFontsLoading(1, &cctrfp ));
                }
            }
        }
        ImGui::EndMenu();
    }
}




