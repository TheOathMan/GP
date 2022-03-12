#pragma once
#include "AppCore.h"
#include "../NMC/imgui/imgui.h"
#include <queue>

//#include "App_window/Main_Win.h"
struct  GLFWwindow;

class GUIspec {
public:
	ImGuiWindowFlags window_flags = 0;

	ImFont* _main_font;
	ImFont* _main_font_Big;
	ImGuiIO io;
	ImGuiStyle* style;
	ImDrawList* draw_list;


	GUIspec(GLFWwindow* window);
	void GUI_OnFrameStart();
	void GUI_OnFrameEnd();
	void GUI_OnDestruction();

private:
	GLFWwindow* window;
};
void ShowMenuFile();
void ShowStyleEditor(ImGuiStyle* ref = NULL);
namespace GpGUI {
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg);
	bool Spinner(const char* label, float radius, int thickness, const ImU32& gl_color);
	bool CheckBox(const char* name, bool* val);
	bool LinkedCheckBox(const char* name, int* v, int val);
	void SPINE(float x = 0, float y = 0, float x1 = 0, float y1 = 0);
}

extern std::deque<std::string> OpenRecentP;
extern std::deque<std::string> DropRecentP;
