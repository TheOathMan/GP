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


void  Image::SetColores(color_t colors){
     To_RBGA();
     PIXEL_ACCESS_BEGIN(width, height, m_format, PixelMapPointes);
     SET_DEFAULT;
     PIXEL[RED]   = Get_Red(colors);
     PIXEL[GREEN] = Get_Green(colors);
     PIXEL[BLUE]  = Get_Blue(colors);
     PIXEL[ALPHA] = Get_Alpha(colors);
     PIXEL_ACCESS_END;
}

void Image::ColReset(){
    memset(data, 0, width * height * m_format);
}

//  void Image::FullAlpha()
//  {
//      To_RBGA();
//      PIXEL_ACCESS_BEGIN(width, height, m_format, PixelMapPointes);
//      //int sumcols = PIXEL[RED] + PIXEL[GREEN] + PIXEL[BLUE];
//              //int res = sumcols / 3;
//      SET_DEFAULT;
//      //PIXEL[RED]   = PIXEL[ALPHA];
//      //PIXEL[GREEN] = PIXEL[ALPHA];
//      //PIXEL[BLUE]  = PIXEL[ALPHA];
//      PIXEL[ALPHA] = 0;
//      PIXEL_ACCESS_END;

//  }
 void Image::overrideColors(const color_t& on_black,const  color_t& on_white)
 {
    To_RBGA();// make sure we are deeling with three channels
    PIXEL_ACCESS_BEGIN(width, height, m_format, PixelMapPointes);
    SET_DEFAULT;
    color_t b = on_black;
    color_t w = on_white;

    Vec3 p , p1 = To01_fVec3(PIXEL[RED], PIXEL[GREEN], PIXEL[BLUE]);
    p.r = 1 - p1.r;
    p.g = 1 - p1.g;
    p.b = 1 - p1.b;

    Vec3 out =  To01_fVec3(Get_Red(b), Get_Green(b), Get_Blue(b));
    Vec3 out2 = To01_fVec3(Get_Red(w), Get_Green(w), Get_Blue(w));

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

void Image::AlphaToCheckerboard(){
    To_RBGA();
    //float scale=Get_Height()/2.0f;
    float scale=24.0f;
    PIXEL_ACCESS_BEGIN(width, height, m_format, PixelMapPointes);
    SET_DEFAULT;
    float cx = Repeat<float>(_x,scale);
    float cy = Repeat<float>(_y,scale);
    float fc = 0;
    int col1 = 188, col2 = 95;
    float hfscl = scale /2.0f;
    if(cy < hfscl) fc = cx < hfscl ? col2 : col1;
    if(cy > hfscl) fc = cx < hfscl ? col1 : col2;
    if(cy == hfscl)fc =  (col1 + col2) >> 1;
    float inalph = 1.0f -  COL8_TO_F(PIXEL[ALPHA]);
    float alpha  = COL8_TO_F(PIXEL[ALPHA]);

    PIXEL[RED]   = (fc * inalph) + PIXEL[RED]   *  alpha;
    PIXEL[GREEN] = (fc * inalph) + PIXEL[GREEN] *  alpha;
    PIXEL[BLUE]  = (fc * inalph) + PIXEL[BLUE]  *  alpha;
    PIXEL[ALPHA]  = 255;
    PIXEL_ACCESS_END;
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
    static Image fim(std::move(data), w, h, format);
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
 Image::Image(const pixel_uc* d, int w, int h, int f) : ImagePath("")
{
    m_format = f;
    init(w, h);
    memcpy(data, d, w * h * m_format * sizeof(pixel_uc));
}
 Image::Image(pixel_uc *(&&d), int w, int h, int f) : ImagePath("")
{
    m_format = f;
    data = std::move(d); d=nullptr;
    init(w, h);
}

 Image::Image(pixel_uc *imd,int len, int f) : ImagePath("")
{
    //STBIDEF stbi_uc* stbi_load_from_memory(stbi_uc           const* buffer, int len, int* x, int* y, int* channels_in_file, int desired_channels);
    int channelInImageData=0;
    data = stbi_load_from_memory(imd,len,&width,&height,&channelInImageData,f);
    m_format = f;
    init(width, height);
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
     PixelMapPointes = std::move(other.PixelMapPointes);other.PixelMapPointes=nullptr;
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

