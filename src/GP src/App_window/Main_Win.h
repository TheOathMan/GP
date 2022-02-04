#pragma once
#include "app_window.h"
#include "../GUIspec.h"

class Main_Win : public App_Window
{
public:
	using ImGuiMouseCursor = int;

	Main_Win(const char* window_name, int width, int height);
	~Main_Win()			  override;

	void OnWindowAwake()  override;
	void OnUpdate()		  override;
	void OnInput()		  override;
	//void OnDestruction()override;


	void guiWindowsResizes(ImGuiMouseCursor current);
	GUIspec* gui_spc = nullptr;

private:
	void MainCanvesGUIWin();
	void GlyphProcessPages();
	void FontSelections();
	void MenuBar();
};


