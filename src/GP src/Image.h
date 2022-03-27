#pragma once
#include "AppCore.h"
//#include <iostream>

// ---------------- image access with custom access(x,y)

//image access, im = BaseImage type, x = width access, y height access
#define Img_Ac(Raw_Image, x, y) (Raw_Image->PixelMapPointes[y][x * Raw_Image->m_format])
//image access, im = BaseImage type, x = width access, y height access, offset color channel access
#define Img_Ac_s(Raw_Image, x, y,offset) (Raw_Image->PixelMapPointes[y][x * Raw_Image->m_format + offset])

#define Get_Red(col)   ((uint8_t*)&col)[0]
#define Get_Green(col) ((uint8_t*)&col)[1]
#define Get_Blue(col)  ((uint8_t*)&col)[2]
#define Get_Alpha(col) ((uint8_t*)&col)[3]

#define IMID(x) (ImTextureID)(intptr_t)x

#define SET_COLS(col,r,g,b,a) do{\
((uint8_t*)&col)[0] = r;\
((uint8_t*)&col)[1] = g;\
((uint8_t*)&col)[2] = b;\
((uint8_t*)&col)[3] = a;}while(false)


template <typename T>
struct Raw_Image {
protected: int width, height;
public:
    int m_format = 1;
    T* data = nullptr;
    T** PixelMapPointes = nullptr; // given an image width and height, first pointers access data's y, second pointers access x. 
    Raw_Image() : width(0), height(0){}
    Raw_Image(int w,int h,int f,bool setTo0 = false ) :width(w),height(h), m_format(f){
        init(w,h);
        if (setTo0 && data) memset(data, 0, w * h * m_format * sizeof(T) );
    }

    void init(int w,int h) {
        width = w;
        height = h;

        if(data == nullptr)
           data = new T[w * h * m_format];  // allocate space for image data  

        PixelMapPointes = new T*[h];   // allocate space for row pointers
        for (int i = 0; i < h; i++)
            PixelMapPointes[i] = data + (i * w * m_format);

    }
    // copy colors based on available channels
    void copy_colors(const Raw_Image<T>& other,int channels) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                for (int f = 0; f < channels; f++) {
                    if (f < other.Get_Format())
                        PixelMapPointes[y][x * m_format + f] = other.PixelMapPointes[y][x * other.m_format + f];
                }
            }
        }
     
    }

    inline int Get_Width()      const { return width; }
    inline int Get_Height()     const { return height; }
    inline int Get_Format()     const { return m_format; }

};




//Accepted image format:
// one channel   : Red OR Alpha
// two channels  : Red/Alpha
// three channels: Red/Green/blue
// four channels : Red/Green/blue/Alpha

class Image : public Raw_Image<pixel_uc> {
private:
    const char* ImagePath = "No Path";
    uint image_id = 0;
public:

    // default image. 64*64 white
	Image();
    // load from path
	Image(const char* path, bool flip = false);
	Image(const pixel_uc* Rawdata, int width,int hight,int format);
	Image(pixel_uc *(&&Rawdata), int width,int hight,int format);
	Image(pixel_uc *ImageData,int len,int desired_format);
    Image(int w, int h, int f, bool setTo0 = false) : Raw_Image<pixel_uc>(w, h, f, setTo0) {};

    // copy operations
	Image(const Image& other);
    Image& operator= (const Image& other);

    // move operations
    Image(Image&& other) noexcept;
    Image& operator= (Image&& other) noexcept;
    //Image(pixel_t* (&&data), int width, int hight, int format) = delete;


    void WhiteToAlpha();
    void BlackToAlpha();
    //void FullAlpha();
    void Col_Reverse();
    void Vertical_Flip();
    void To_RBGA();
    void To_SDF(float radius);
    void AlphaToCheckerboard();

    // multiply colors with black and white
    void overrideColors(const  color_t& on_black,const color_t& on_white);
    void SetColores(color_t colors);
    void ColReset();
    void Clean();


	~Image();
    inline const char* Get_Image_Path()   const { return ImagePath;}
    inline uint Get_GPU_ID()              const { return image_id; }
    inline pixel_uc* Get_Data()            const { return data; }
    friend uint LoadImageToGPU(Image& image);
};


// capture the current active viewport into a texure.
Image& CaptureViewport();
// upload texture to the gpu.
uint LoadImageToGPU(Image& image);
color_t config_Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
#define COLOR_WHITE config_Color(255,255,255,255)
#define COLOR_BLACK config_Color(0,0,0,255)
#define COLOR_RED   config_Color(255,0,0,255)
#define COLOR_GREEN config_Color(0,255,0,255)
#define COLOR_BLUE  config_Color(0,0,255,255)
