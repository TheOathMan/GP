#pragma once
#include "vector_c.h"


typedef unsigned char pixel_uc,binery_t,uchar;
typedef unsigned long long U64;
typedef unsigned short U16,ushort;
typedef unsigned int uint, GLuint,color_t;
typedef int Window_Resize_Dir_Flags;

#define CELLS_NUMBER 100
#define USHORT_MAX 0xffff
#define COL8_TO_F(x) (1.0f / 255.0f) * x
#ifdef Debug
//#define GP_Print(msg,...){ LogTrace(__LINE__, __FILE__, msg, args...); }
#define GP_Print(x) std::cout << LogInfo(__LINE__, __FILE__) << x << '\n'
#define ASSERT(msg,cond) if(cond) __debugbreak()
#else 
#define DebugPrint(x) ((void)0)
//#define GP_PrintA(msg, ...) ((void)0)
#define GP_Print(x) ((void)0)
//#define GP_Print(msg, ...) LogTrace(__LINE__, __FILE__, msg, __VA_ARGS__)
#define ASSERT(msg,cond) ((void)0)
#endif												   

#define DE_NUL(x)   {delete x; x = nullptr;}
#define DE_NUL_A(x) {delete[] x; x = nullptr;}

#define WTF  GP_Print("wtf");

#define GUI_TEST               \
static int a, b, c, d;         \
ImGui::Begin("test");          \
ImGui::DragInt("a", &a, 0.05f);\
ImGui::DragInt("b", &b, 0.05f);\
ImGui::DragInt("c", &c, 0.05f);\
ImGui::DragInt("d", &d, 0.05f);\
ImGui::End();



// free gui text, doesn't add to the scroll value
#define DRAW_TEXT(draw_list,font, size, color,posx,posy,...)\
{char cordbuf[252];\
sprintf_s(cordbuf, __VA_ARGS__);\
draw_list->AddText(font,size, ImVec2(posx, posy),color, cordbuf);\
}

#define BIG_TEXT(...)\
ImGui::SetCurrentFont(gui_spc->_main_font_Big);\
ImGui::Text(__VA_ARGS__);\
ImGui::SetCurrentFont(ImGui::GetDefaultFont())



// Global references
//class App_Window;
//struct stbtt_bakedchar;
//typedef int ImGuiWindowFlags; 
struct stbtt_pack_range;
//struct ImFont;
struct ImGuiIO;
class FontData;
class Image;
struct stbtt_vertex;

template<int, class> struct Vec;
using Vec2 = Vec <2, float>;

struct Glyph_data { // FontData.cpp & Main_Win.cpp

	pixel_uc* pixels=nullptr;
	int width,height,bitmap_size;

	Glyph_data(): pixels(nullptr), width(0),height(0),bitmap_size(0){}
	Glyph_data(pixel_uc* p, int w, int h, int bs): pixels(p), width(w),height(h),bitmap_size(bs){}

};

// Enums----------
enum class Image_Format { // Image.cpp & Main_Win.cpp & Blueprints.h
	PNG,
	BMP,
	TGA,
	JPG
};

enum class Gui_Handle { // FontData.cpp & Main_Win.cpp
	Default   = 0,
	Visible   = 1 << 0,
	Delete    = 1 << 1,
	Duplicate = 1 << 2,
};

enum class Image_Type { // FontData.cpp & Main_Win.cpp
	Background,
	Alpha,
	SDF
};

enum class Window_Resize_Dir { // Main_Win.cpp & Imgui.cpp
	WRD_NONE  = 0,
	WRD_UP    = 1 << 0,
	WRD_RIGHT = 1 << 1,
	WRD_DOWN  = 1 << 2,
	WRD_LEFT  = 1 << 3,
	WRD_FULL  = 0x000F
};

//read = only return bitmab data, write = return a data and a rendred bitmap 
enum class Glyph_Data_Access {// FontData.cpp && Main_Win.cpp
	Bitmap_Write,
	Bitmab_Read
};

enum class FontLoadSource{Drop,Open,None};

struct Glyph_Vertices { // FontData.cpp & Main_Win.cpp & imstb_truetype.h
	Vec2 origin;
	Vec2 dimension_width;
	Vec2 dimension_hight;
	int ver_size=0;             // size of vertex info
	stbtt_vertex* verts=nullptr;	 //vertex info
	short control_points = 1;
	char max_type=0;
};

// used for logging
void LogTrace(int line, const char* fileName, const char* msg, ...);
int gcd(int a, int b);
float distance(Vec2 v1, Vec2 v2);
std::string LogInfo(int line, const char* fileName);