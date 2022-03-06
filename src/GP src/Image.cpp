#include <iostream>
#include <functional>
#include "Image.h"
#include "Blueprints.h"
#include "../NMC/glfw/glfw3.h"
#include "../NMC/stb_image_handler/std_image.h"
#include "../NMC/stb_image_handler/stb_image_write.h"

#define SDF_IMPLEMENTATION
//#include "../NMC/sdf/sdf.h"

// ---------------- image access with ranged access(x,y)
// _x = width acess, _y = height acess, _f = formate(one channel, two channel..etc), _edge_limit = -1 no edge limit
static int _x, _y,_f, _edge_limit=-1; 
static void** _pix_y_map;
//Start PIXEL ACCESS, MUST end with PIXEL_ACCESS_END
#define PIXEL_ACCESS_BEGIN(w,h,f,pix_y_map) _f = f; auto&& type = pix_y_map; _pix_y_map = (void**)pix_y_map;\
for (int y__ = 0; y__ < h; y__++) {\
    for (int x__ = 0; x__ < w; x__++) {\
if (x__ > _edge_limit && y__ > _edge_limit && x__ < w - _edge_limit && y__ < h - _edge_limit) {

#define SET_EDGE_LIMIT(l) _edge_limit = l

// current pxiel set
#define SET_PIXEL(p1,p2) _x = (x__)+(p1); _y = (y__)+(p2); 
// default pixel position, for custom position use SET_PIXEL(x,y)
#define SET_DEFAULT _x = x__; _y = y__;

// _x from 0 to (w*h)
#define PIXEL ((decltype(type))_pix_y_map)[_y]
#define RED   _x * _f
#define GREEN _x * _f + 1
#define BLUE  _x * _f + 2
#define ALPHA _x * _f + 3

#define PIXEL_ACCESS_END }}}\
_edge_limit=-1;


enum class Color_Channel {
    CL_R = 1, 
    CL_RG,
    CL_RGB,
    CL_RGBA
};

//--------------------------------------------------------------------
//---------------------------   FUNCTIONS   --------------------------
//--------------------------------------------------------------------

// blueprint implementations--
template <> void SaveImage<Image_Format::PNG>(const char* path, const Image& image)
{
    char fpath[255] = ""; strcpy_s(fpath, path); strcat_s(fpath, ".png");
    int w = image.Get_Width(), h = image.Get_Height(), f = image.Get_Format();
    stbi_write_png(fpath, w, h, f, image.Get_Data(), w * f);
}

template <> void SaveImage<Image_Format::JPG>(const char* path, const Image& image)
{
    char fpath[255] = ""; strcpy_s(fpath, path); strcat_s(fpath, ".jpg");
    int w = image.Get_Width(), h = image.Get_Height(), f = image.Get_Format();
    stbi_write_jpg(fpath, w, h, f, image.Get_Data(), 100);
}

template <> void SaveImage<Image_Format::BMP>(const char* path, const Image& image)
{
    char fpath[255] = ""; strcpy_s(fpath, path); strcat_s(fpath, ".bmp");
    int w = image.Get_Width(), h = image.Get_Height(), f = image.Get_Format();
    stbi_write_bmp(fpath, w, h, f, image.Get_Data());
}

template <> void SaveImage<Image_Format::TGA>(const char* path, const Image& image)
{
    char fpath[255] = ""; strcpy_s(fpath, path); strcat_s(fpath, ".tga");
    int w = image.Get_Width(), h = image.Get_Height(), f = image.Get_Format();
    stbi_write_tga(fpath, w, h, f, image.Get_Data());
}


//transform 3 color chanels of 0-255 to float 0-1
Vec3 To01_fVec3(pixel_uc r, pixel_uc g, pixel_uc b) {
    float p_r = r; p_r = COL8_TO_F(p_r);
    float p_g = g; p_g = COL8_TO_F(p_g);
    float p_b = b; p_b = COL8_TO_F(p_b);
    return Vec3(p_r, p_g, p_b);
}


 uint LoadImageToGPU(Image& image)
{
     if (image.image_id) glDeleteTextures(1, &image.image_id);

    GLint target_format=0, internalFormat = image.Get_Format();
    switch (internalFormat)
    {
    case 1:
        target_format  = GL_ALPHA;
        internalFormat = GL_ALPHA;
        break;
    case 2:
        target_format  = GL_LUMINANCE_ALPHA;
        internalFormat = GL_LUMINANCE4_ALPHA4;
        break;
    case 3:
        target_format  = GL_RGB;
        internalFormat = GL_RGB4;
        break;
    case 4:
        target_format  = GL_RGBA;
        internalFormat = GL_RGBA4;
        break;
    }

    GLint prev_tex;
    uint id;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_tex);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.Get_Width(), image.Get_Height(), 0, target_format, GL_UNSIGNED_BYTE, image.Get_Data());
    //Debug(id);
    //glBindTexture(GL_TEXTURE_2D, 0);
    // Restore state
    image.image_id = id;
    glBindTexture(GL_TEXTURE_2D, prev_tex);
    return id;
}



 void Image::WhiteToAlpha()
 {
     To_RBGA();
     PIXEL_ACCESS_BEGIN(width, height, m_format, PixelMapPointes);

     SET_DEFAULT;

     int sumcols = PIXEL[RED] + PIXEL[GREEN] + PIXEL[BLUE];
     int res = sumcols / 3;
     PIXEL[RED]   = 0;
     PIXEL[GREEN] = 0;
     PIXEL[BLUE]  = 0;
     PIXEL[ALPHA] = 255 - res;

     PIXEL_ACCESS_END;

 }
 void Image::BlackToAlpha()
 {
     To_RBGA();
     PIXEL_ACCESS_BEGIN(width, height, m_format, PixelMapPointes);

     SET_DEFAULT;

     int sumcols = PIXEL[RED] + PIXEL[GREEN] + PIXEL[BLUE];
     int res = sumcols / 3;
     PIXEL[RED]   = 255;
     PIXEL[GREEN] = 255;
     PIXEL[BLUE]  = 255;
     PIXEL[ALPHA] = res;

     PIXEL_ACCESS_END;

 }

 void Image::FullAlpha()
 {
     To_RBGA();
     PIXEL_ACCESS_BEGIN(width, height, m_format, PixelMapPointes);
     //int sumcols = PIXEL[RED] + PIXEL[GREEN] + PIXEL[BLUE];
             //int res = sumcols / 3;
     SET_DEFAULT;
     PIXEL[RED]   = PIXEL[ALPHA];
     PIXEL[GREEN] = PIXEL[ALPHA];
     PIXEL[BLUE]  = PIXEL[ALPHA];
     PIXEL[ALPHA] = 255;
     PIXEL_ACCESS_END;

 }



 void Image::SetColor(color_t& gl_color)
 {
     To_RBGA();
     PIXEL_ACCESS_BEGIN(width, height, m_format, PixelMapPointes);
     SET_DEFAULT;

     Vec3 p1 = To01_fVec3(PIXEL[RED],PIXEL[GREEN],PIXEL[BLUE]);
     Vec3 p2 = To01_fVec3(Get_Red(gl_color), Get_Green(gl_color), Get_Blue(gl_color));
    
     PIXEL[RED]   = (p1.r * p2.r) * 255.0f;
     PIXEL[GREEN] = (p1.g * p2.g) * 255.0f;
     PIXEL[BLUE]  = (p1.b * p2.b) * 255.0f;

     PIXEL_ACCESS_END;
 }

 void Image::SetColor2(color_t& gl_color, color_t& color2)
 {
     To_RBGA();// make sure we are deeling with three channels
     PIXEL_ACCESS_BEGIN(width, height, m_format, PixelMapPointes);
     SET_DEFAULT;

     Vec3 p = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
     Vec3 p1 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);

     p.r = 1 - p1.r;
     p.g = 1 - p1.g;
     p.b = 1 - p1.b;

     Vec3 out =  To01_fVec3(Get_Red(gl_color), Get_Green(gl_color), Get_Blue(gl_color));
     Vec3 out2 = To01_fVec3(Get_Red(color2), Get_Green(color2), Get_Blue(color2));

     p.r = (p.r * out.r);
     p.g = (p.g * out.g);
     p.b = (p.b * out.b);

     p1.r = (p1.r * out2.r);
     p1.g = (p1.g * out2.g);
     p1.b = (p1.b * out2.b);

     PIXEL[RED]   = (p.r + p1.r) * 255.0f;
     PIXEL[GREEN] = (p.g + p1.g) * 255.0f;
     PIXEL[BLUE]  = (p.b + p1.b) * 255.0f;

     PIXEL_ACCESS_END;

 }

 color_t config_Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
 {
     color_t col = 0;
     uint8_t* c = (uint8_t*)&col;
     c[0] = red;
     c[1] = green;
     c[2] = blue;
     c[3] = alpha;
     return col;
 }


 void Image::To_RBGA()
 {
     if (m_format == (int)Color_Channel::CL_RGBA) 
         return;
     
     pixel_uc* newData = new pixel_uc[(width + 0) * (height + 0) * 4];

     for (size_t i = 0; i < width * height; i++)
     {
         pixel_uc* inp = data + (i * 1);
         pixel_uc* newdataP = newData + (i * 4);
         newdataP[0] = inp[m_format == (int)Color_Channel::CL_RGB ? 1 : 0];
         newdataP[1] = inp[m_format == (int)Color_Channel::CL_RGB ? 2 : 0];
         newdataP[2] = inp[m_format == (int)Color_Channel::CL_RGB ? 3 : 0];
         newdataP[3] = m_format == (int)Color_Channel::CL_RG ? inp[1] : 255;
     }

     Clean();
     
     data = newData;
     m_format = (int)Color_Channel::CL_RGBA;
     PixelMapPointes = new pixel_uc * [height];   // allocate space for row pointers
     for (int i = 0; i < height; i++)
         PixelMapPointes[i] = data + (i * width * m_format);
 }


 void Image::Col_Reverse()
 {

     for (size_t i = 0; i < width * height; i++)
     {
         pixel_uc* dataP = data + (i * m_format);
         for (size_t i = 0; i < m_format; i++)
         {
                pixel_uc* col = dataP + i;
                *col = 255 - *(col);
         }
     }
 }


 void Image::Vertical_Flip()
 {
     stbi__vertical_flip(data, width, height,m_format);
 }

Image& CaptureViewport()
{
    //GP_Print("Viewport Captured!!!");
    int format = 4; int w, h;
    GLFWwindow* cur_win = glfwGetCurrentContext();
    glfwGetFramebufferSize(cur_win, &w, &h);
    glViewport(0, 0, w, h);
    pixel_uc *data = new pixel_uc[w * h * format];
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi__vertical_flip(data, w, h, format);
    //stbi_write_png("dvsvs.png", w, h, format, data, w * format);
    static Image fim(data, w, h, format);
    return fim;
}

//--------------------------------------------------------------------
//---------------------------  CONSTRUCTOR  --------------------------
//--------------------------------------------------------------------
 Image::Image() : Raw_Image(64,64,3), ImagePath("")
{
     PIXEL_ACCESS_BEGIN(width, height, m_format, PixelMapPointes);
     SET_DEFAULT;
     // set default image as white
     PIXEL[RED] = 255;
     PIXEL[GREEN] = 255;
     PIXEL[BLUE] = 255;
     PIXEL_ACCESS_END;
}
 Image::Image(const char* path, bool flip) : ImagePath(path) {
     Clean();
    stbi_set_flip_vertically_on_load(flip);
    data = stbi_load(ImagePath, &width, &height, &m_format, 0);
    PixelMapPointes = new pixel_uc * [height];   // allocate space for row pointers
    for (int i = 0; i < height; i++)
        PixelMapPointes[i] = data + (i * width * m_format);
 }
 Image::Image(pixel_uc* d, int w, int h, int f) : ImagePath("")
{
     m_format = f;
     data = std::move(d); d=nullptr;
     init(w, h);
}

//--------------------------------------------------------------------
//-------------------------  COPY OPERATIONS  ------------------------
//--------------------------------------------------------------------
 Image::Image(const Image& other)  {
     int w = other.Get_Width();
     int h = other.Get_Height();

     m_format = other.Get_Format();
     ImagePath = other.ImagePath;
     init(w, h);
     memcpy(data, other.data, w * h * other.Get_Format() * sizeof(pixel_uc));
 }
 Image& Image::operator=(const Image& other)
{
     int w = other.Get_Width();
     int h = other.Get_Height();

    m_format = other.Get_Format();
    ImagePath = other.ImagePath;
    init(w, h);
    memcpy(data, other.data, w * h * other.Get_Format() * sizeof(pixel_uc));
    return *this;
}

 //--------------------------------------------------------------------
//-------------------------  MOVE OPERATIONS  ------------------------
//--------------------------------------------------------------------
 Image::Image(Image&& other) noexcept :
     ImagePath(std::move(other.ImagePath))
 {
     Clean(); // delete old data for a new ones
     width = std::move(other.width);
     height = std::move(other.height);
     m_format = std::move(other.m_format);
     image_id = std::move(other.image_id); other.image_id=0;
     data = std::move(other.data); other.data=nullptr;
     PixelMapPointes = std::move(other.PixelMapPointes); other.PixelMapPointes=nullptr;
 }

 Image& Image::operator=(Image&& other) noexcept
 {
     Clean(); // delete old data for a new ones
     width = std::move(other.width);
     height = std::move(other.height);
     m_format = std::move(other.m_format);
     ImagePath = std::move(other.ImagePath);
     image_id = std::move(other.image_id);other.image_id=0;
     data = std::move(other.data);other.data=nullptr;
     PixelMapPointes = std::move(other.PixelMapPointes);
     return *this;
 }

//--------------------------------------------------------------------
//-------------------------  DESTRUCTION  ----------------------------
//--------------------------------------------------------------------
Image::~Image() {
    Clean();
}
void Image::Clean()
{
    DE_NUL_A(data);
    DE_NUL_A(PixelMapPointes);
    if (image_id) glDeleteTextures(1, &image_id);
    
}


//--------------------------------------------------------------------
//----------------------------  S D F  -------------------------------
//--------------------------------------------------------------------


void Image::To_SDF(float rad)
{
    //sdfCoverageToDistanceField(data, width, data, width, height, width);
    //sdfBuildDistanceField(data, width, rad, data, width, height, width);

}

namespace IPP {

    float Outline_kernel[9]{ // outline
    -1, -1, -1,
    -1,  8, -1,
    -1, -1, -1
    };

    float Blure_kernel[9]{
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    };

    Raw_Image<pixel_uc>* kernalProcess(const Raw_Image<pixel_uc>& im, float kernel1[9]) {
        //sample--
        Image* img = new Image(im.Get_Width(), im.Get_Height() ,3,true);
        SET_EDGE_LIMIT(1);
        PIXEL_ACCESS_BEGIN(im.Get_Width(), im.Get_Height(), im.Get_Format(), im.PixelMapPointes);

        //int limit = 1;
        Vec3 col = Vec3();

            SET_PIXEL(-1, 1);
            Vec3 p1 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
            SET_PIXEL(0, 1);
            Vec3 p2 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
            SET_PIXEL(1, 1);
            Vec3 p3 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
            SET_PIXEL(-1, 0);
            Vec3 p4 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
            SET_DEFAULT;
            Vec3 p5 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);

            SET_PIXEL(1, 0);
            Vec3 p6 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
            SET_PIXEL(-1, -1);
            Vec3 p7 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
            SET_PIXEL(0, -1);
            Vec3 p8 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
            SET_PIXEL(1, -1);
            Vec3 p9 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
            SET_DEFAULT;

            Vec3 samples[] = {p1,p2,p3,p4,p5,p6,p7,p8,p9};

            for (int i = 0; i < 9; i++) {
                col.r += samples[i].r * kernel1[i];
                col.g += samples[i].g * kernel1[i];
                col.b += samples[i].b * kernel1[i];
            }

            Img_Ac_s(img, x__, y__, 0) = (pixel_uc)(Clamp01(col.r) * UCHAR_MAX);
            Img_Ac_s(img, x__, y__, 1) = (pixel_uc)(Clamp01(col.g) * UCHAR_MAX);
            Img_Ac_s(img, x__, y__, 2) = (pixel_uc)(Clamp01(col.b) * UCHAR_MAX);        

        PIXEL_ACCESS_END;

        return img;
    }

    Raw_Image<pixel_uc>* Anti_Aliasing(Raw_Image<pixel_uc>& im) {

        Raw_Image <pixel_uc>* mask = kernalProcess(im, Outline_kernel);
        //sample--

        Raw_Image<pixel_uc>* out = new Raw_Image<pixel_uc>(im.Get_Width(), im.Get_Height(), 3, true);

        SET_EDGE_LIMIT(1);
        PIXEL_ACCESS_BEGIN(im.Get_Width(), im.Get_Height(), im.Get_Format(), im.PixelMapPointes);

        //int limit = 1;
        Vec3 col = Vec3();

        SET_PIXEL(-1, 1);
        Vec3 p1 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
        SET_PIXEL(0, 1);
        Vec3 p2 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
        SET_PIXEL(1, 1);
        Vec3 p3 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
        SET_PIXEL(-1, 0);
        Vec3 p4 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
        SET_DEFAULT;
        Vec3 p5 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);

        SET_PIXEL(1, 0);
        Vec3 p6 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
        SET_PIXEL(-1, -1);
        Vec3 p7 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
        SET_PIXEL(0, -1);
        Vec3 p8 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
        SET_PIXEL(1, -1);
        Vec3 p9 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
        SET_DEFAULT;

        Vec3 samples[] = { p1,p2,p3,p4,p5,p6,p7,p8,p9 };
        float masking = To01_fVec3(Img_Ac_s(mask, x__, y__, 0), 0, 0).x;

        if (masking > 0.65f) {

            for (int i = 0; i < 9; i++) {
                col.r += samples[i].r * Blure_kernel[i];
                col.g += samples[i].g * Blure_kernel[i];
                col.b += samples[i].b * Blure_kernel[i];
            }
        }
        else {
            col.r = p5.r;
            col.g = p5.g;
            col.b = p5.b;
        }

        Img_Ac_s(out, x__, y__, 0) = (pixel_uc)(Clamp01(col.r) * UCHAR_MAX);
        Img_Ac_s(out, x__, y__, 1) = (pixel_uc)(Clamp01(col.g) * UCHAR_MAX);
        Img_Ac_s(out, x__, y__, 2) = (pixel_uc)(Clamp01(col.b) * UCHAR_MAX);

        PIXEL_ACCESS_END;

        delete mask;

        return out;
    }


    Raw_Image<pixel_uc>* Trim(Raw_Image<pixel_uc>& im) {

        //sample--

        Vec<2,int> p1(20000, 20000);
        Vec<2,int> p2;
        
        int hw = im.Get_Width() / 2;
        int hh = im.Get_Height() / 2;


        SET_EDGE_LIMIT(1);
        PIXEL_ACCESS_BEGIN(im.Get_Width(), im.Get_Height(), im.Get_Format(), im.PixelMapPointes);
        SET_DEFAULT;
        if (PIXEL[RED] != 0 || PIXEL[GREEN] != 0 || PIXEL[BLUE] != 0) {

            if (x__ < hw) 
                if (x__ < p1.x)
                    p1.x = x__;
                
            if (x__ > hw) 
                if (x__ > p2.x)
                    p2.x = x__;

            if (y__ < hh)
                if (y__ < p1.y)
                    p1.y = y__;

            if (y__ > hh)
                if (y__ > p2.y)
                    p2.y = y__;
        } //-------------
        PIXEL_ACCESS_END;

        Vec<2, int > newRes(p2.x - p1.x, p2.y - p1.y);
        Raw_Image<pixel_uc>* out = new Raw_Image<pixel_uc>(newRes.x, newRes.y, im.Get_Format(), true);
        {
            PIXEL_ACCESS_BEGIN(im.Get_Width(), im.Get_Height(), im.Get_Format(), im.PixelMapPointes);
            SET_DEFAULT;



            PIXEL_ACCESS_END; 
        }


        return out;
    }


}