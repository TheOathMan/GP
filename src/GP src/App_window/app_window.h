#pragma once
#include <vector>
#include "../AppCore.h"
#include "../Event.h"
#include "../../NMC/GLFW/glfw3.h"


class App_Window {
public:
	App_Window(const char* window_name,int width, int height, GLFWwindow* srd_window = NULL);
	virtual ~App_Window();

	static std::vector<App_Window*> wins32;
	virtual void OnWindowAwake();
	virtual void OnUpdate();
	virtual void OnInput();

	// Default raw Quad drawn on the full window
	// set colors of
	void SetBackgroundColor(uint8_t red, uint8_t green, uint8_t blue);
	//set window textuer, id is a gpu uploaded texture id. use LoadImageToGPU(Image im)..
	void Set_texture_id(Image* im);

	inline int get_width()				  const { return width;}
	inline int get_height()				  const { return height;}
	inline Vec2i get_screenSize()		  const { return ScreenSize;}
	inline GLFWwindow* get_window()		  const { return window;}
	inline const char* get_window_name()  const { return windowName;}
	inline bool IsWindowOpen()					{ return !glfwWindowShouldClose(window); }
	inline bool IsWindowVisible()               { int a; glfwGetWindowSize(window,&a,&a); return a;}
private:
	Image* bg_image;
	void RawQuadTexture();
	color_t background_col;
	int width, height;
	Vec2i ScreenSize;
	const char* windowName;
protected:
	GLFWwindow* window = nullptr;
};