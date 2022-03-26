
#include <string>
#include "Blueprints.h"
#include "FontData.h"
#include "FileHandle.h"
#include "../NMC/GLFW/glfw3.h"
#include "../NMC/stb_image_handler/stb_image_write.h"
#include "../NMC/imgui/imstb_truetype.h"
#include "../NMC/imgui/imgui.h"



// takes, apth to a font, a UTF8 codepoint, then return its drawing sign distance field data
Glyph_data FontData::GetGlyphBitmap_SDF(int glyph,float scale)
{

    int width, height, xoff, yoff;
    float sdf_scale = stbtt_ScaleForPixelHeight(&font, scale);
    //int advance, lsb;
    //stbtt_GetGlyphHMetrics(&font, glyph, &advance, &lsb);

    int sdf_padding = 12;
    int  sdf_onedge_value = 170;
    float pixel_dist_scale = (sdf_onedge_value / sdf_padding);
    pixel_uc* pixels = stbtt_GetGlyphSDF(&font, sdf_scale, glyph, sdf_padding, sdf_onedge_value, pixel_dist_scale, &width, &height, &xoff, &yoff);
    Glyph_data rs { pixels,width ,height, width * height };
    return rs;
}

Glyph_data FontData::GetGlyphBitmap_SDF_V(int glyph_index, float scale, Glyph_Data_Access Info_Inquiry)
{
    int width, height, xoff, yoff;
    float sdf_scale = stbtt_ScaleForPixelHeight(&font, scale);
        //Glyph_Vertices gv = glyph_list.at(glyph_index);
    //int advance, lsb;
    //stbtt_GetGlyphHMetrics(&font, glyph, &advance, &lsb);
    pixel_uc* pixels = nullptr;
        int sdf_padding = 12;
        int  sdf_onedge_value = 170;
        float pixel_dist_scale = (sdf_onedge_value / sdf_padding);
    if (Info_Inquiry == Glyph_Data_Access::Bitmap_Write) {
        Glyph_Vertices &gv = glyph_list.at(glyph_index);
        pixels = stbtt_GetGlyphSDF(&font, sdf_scale, visible_unique_indices.at(glyph_index), sdf_padding, 
            sdf_onedge_value, pixel_dist_scale, &width, &height, &xoff, &yoff, gv.verts,gv.ver_size);
    }
    else {
        int ix0, iy0, ix1, iy1;
        stbtt_GetGlyphBitmapBoxSubpixel(&font, visible_unique_indices.at(glyph_index), sdf_scale, sdf_scale, 0.0f, 0.0f, &ix0, &iy0, &ix1, &iy1);

        ix0 -= sdf_padding;
        iy0 -= sdf_padding;
        ix1 += sdf_padding;
        iy1 += sdf_padding;

        //// now we get the size
        width  = (ix1 - ix0);
        height = (iy1 - iy0);
    }

    Glyph_data rs = { pixels,width ,height, width * height };
    return rs;
}

// takes, a path to a font, a UTF8 codepoint, then return its drawing data
Glyph_data FontData::GetGlyphBitmap(int glyph, float scale)
{
    int width, height, xoff, yoff;
    //int advance_width, left_side_bearing;

    float _scale = stbtt_ScaleForPixelHeight(&font, scale);
    //int x0, x1, y0, y1;
    //stbtt_GetGlyphBox(&font, glyph, &x0, &y0, &x1, &y1);

    pixel_uc* pixels = stbtt_GetGlyphBitmap(&font, _scale, _scale, glyph, &width, &height, &xoff, &yoff);

    Glyph_data rs = { pixels,width ,height, width * height };
    return rs;
}


Glyph_data FontData::GetGlyphBitmap_V(int glyph_index, float scale, int EdgeOffset, Glyph_Data_Access Info_Inquiry)
{
    float _scale = stbtt_ScaleForPixelHeight(&font, scale);
    int ix0, iy0, ix1, iy1;
    stbtt__bitmap gbm;
    Glyph_Vertices gv = glyph_list.at(glyph_index);

    stbtt_GetGlyphBitmapBoxSubpixel(&font, visible_unique_indices.at(glyph_index), _scale, _scale, 0.0f, 0.0f, &ix0, &iy0, &ix1, &iy1);

    //// now we get the size
    gbm.w = (ix1 - ix0) + EdgeOffset;
    gbm.h = (iy1 - iy0) + EdgeOffset;
    gbm.pixels = NULL; // in case we error

    if (!(int)Info_Inquiry)
        if (gbm.w && gbm.h) {
            gbm.pixels = new pixel_uc[gbm.w * gbm.h];
            if (gbm.pixels) {
                gbm.stride = gbm.w;

                stbtt_Rasterize(&gbm, 0.35f, gv.verts, gv.ver_size, _scale, _scale, EdgeOffset / 2.0f, EdgeOffset / 2.0f, ix0, iy0, 1, NULL);
            }
        }
    Glyph_data rs = { gbm.pixels,gbm.w ,gbm.h, (gbm.w * gbm.h) };
    return rs;
}

Glyph_data FontData::GetGlyphPreData(int glyph_index, float scale, Image_Type type, int SDF_edgeOffset) {
    Glyph_data dt;
    switch (type)
    {
    case Image_Type::Background:
        dt = GetGlyphBitmap_V(glyph_index, scale, SDF_edgeOffset,Glyph_Data_Access::Bitmab_Read);
        break;
    case Image_Type::Alpha:
        dt = GetGlyphBitmap_V(glyph_index, scale, SDF_edgeOffset,Glyph_Data_Access::Bitmab_Read);
        break;
    case Image_Type::SDF:
        dt = GetGlyphBitmap_SDF_V(glyph_index, scale, Glyph_Data_Access::Bitmab_Read);
        break;
    default:
        break;
    }
    return dt;
}

size_t FontData::LastSizePushed = 1;

void FontData::init()
{

    FileHandle font_file(font_path.c_str(), FileBinAccess::READ);
    m_fsize = font_file.file_size;
    LastSizePushed =m_fsize; 
    if (font_file.file_data) {
        font_binery_data = (uint8_t*)font_file.file_data;
        gui_handle = Gui_Handle::Visible;
    }
    else {
        gui_handle = Gui_Handle::Delete;
        return;
    }
    if (font_binery_data == nullptr || !stbtt_InitFont(&font, font_binery_data, 0)) {
        gui_handle = Gui_Handle::Delete;
        return;
    }

    TotalGlyphs = font.numGlyphs;
    for (size_t i = 0; i < USHORT_MAX; i++)
    {
        if (i >= TotalGlyphs) break;

        int  x0, y0, x1, y1;
        stbtt_GetGlyphBitmapBoxSubpixel(&font, i, 5, 5, 2, 0, &x0, &y0, &x1, &y1);
        int Glyph_Visibilty = stbtt_IsGlyphEmpty(&font, i); 
        //int Glyph_Visibilty = (x0 != x1) && (y0 != y1); // safe less accurate method

        if (Glyph_Visibilty == 0) {

            Glyph_Vertices gv;

            stbtt_vertex* vertices = NULL;
            gv.ver_size = stbtt_GetGlyphShape(&font, i, &vertices);
            
            gv.verts = vertices;
            visibleGlyphs++;

            // check duplication. 
            bool isNew = true;
            for (size_t i = 0; i < glyph_list.size(); i++)
            {
                if (glyph_list.at(i).ver_size == gv.ver_size)
                {
                    for (size_t glyph_setting = 0; glyph_setting < gv.ver_size; glyph_setting++)
                    {
                        stbtt_vertex& vc1 = glyph_list.at(i).verts[glyph_setting];
                        stbtt_vertex& vc2 = gv.verts[glyph_setting];

                        if (vc1.x != vc2.x && vc1.y != vc2.y && vc1.cx != vc2.cx && vc1.cy != vc2.cy) //|| vc1.cx != vc2.cx || vc1.cy != vc2.cy) 
                        {
                            isNew = true;
                            break;
                        }
                        isNew = false;
                    }
                }
                if (!isNew) break;
            }

            if (isNew) {
                visible_unique_indices.push_back(i);
                
                // shape origin point and dimenstions.
                Vec2i mins(SHRT_MAX, SHRT_MAX);
                Vec2i maxs(SHRT_MIN, SHRT_MIN);
                for (size_t i = 0; i < gv.ver_size; i++)
                {
                    auto& vert = gv.verts[i];
                    if (vert.x < mins.x)
                        mins.x = vert.x;
                    if (vert.y < mins.y)
                        mins.y = vert.y;

                    if (vert.x > maxs.x)
                        maxs.x = vert.x;
                    if (vert.y > maxs.y)
                        maxs.y = vert.y;

                    if (vert.type >= gv.max_type)
                        gv.max_type = vert.type;
                }


                gv.origin = Vec2i( (mins.x + maxs.x)/2, (mins.y + maxs.y) / 2);
                gv.dimension_width = Vec2i(mins.x, maxs.x );
                gv.dimension_hight = Vec2i(mins.y, maxs.y);

                //gv += 
                glyph_list.push_back(gv);
            }
            else {
                stbtt_FreeShape(&font,vertices);
                DuplicatedGlyphs++;
            }


        }
    }
    invisibleGlyphs = TotalGlyphs - visibleGlyphs;



    int pagesLimits = ceil( (float)visible_unique_indices.size() / (float)CELLS_NUMBER);
    for (int i = 1; i < pagesLimits + 1; i++)
    {
        static char cr[20];
        sprintf_s(cr, "%i", i); 
        for (char* c = cr; *c != '\0'; c++) { ComboPages.push_back(*c); }
        ComboPages.push_back(0);
    }
    ComboPages.push_back('\0');

}

// Brute Force:
//read every index with no discrimination.
FontData::FontData(const std::string& font_path) : 
    font_path(font_path) {init();}

FontData::FontData(const std::string& font_path, const std::string& font_name) :
    font_path(font_path),
    font_name(font_name) {init();}

FontData::FontData(const FontData& other) : 
    font_name(other.font_name) { init(); }

FontData::FontData(FontData&& other) noexcept:
font_name(std::move(other.font_name)),
font_path(std::move(other.font_path)),
visibleGlyphs(other.visibleGlyphs),
TotalGlyphs(other.TotalGlyphs),
DuplicatedGlyphs(other.DuplicatedGlyphs),
invisibleGlyphs(other.invisibleGlyphs),
m_fsize(other.m_fsize),
font(other.font),
font_binery_data(std::move(other.font_binery_data)),
visible_unique_indices(std::move(other.visible_unique_indices)),
glyph_list(std::move(other.glyph_list)),
gui_handle(other.gui_handle)
{
}

FontData::FontData(std::string&& font_path) :
    font_path(std::move(font_path)) { init(); }

FontData::FontData(std::string&& font_path, std::string&& font_name):
    font_path(std::move(font_path)), 
    font_name(std::move(font_name)) { init(); }

FontData::~FontData() 
{
    for (size_t i = 0; i < glyph_list.size(); i++)
    {
        stbtt_FreeShape(&font,glyph_list.at(i).verts);
    }
    delete[] font_binery_data;
}

//prints a range of  glyph on a pre-determind canves. truditional way.. 
Ranged_Font_Bitmap_data RangedBitmapDraw(const FontData& font_data, int firstchar,int charnums,float glyphPosH,int bitmap_size)
{
    pixel_uc* temp_bitmap = new pixel_uc[bitmap_size*bitmap_size];
    GLuint ftex = 0;

    Ranged_Font_Bitmap_data rfbd;
    rfbd.bitmap_size = bitmap_size * bitmap_size;
    rfbd.width = bitmap_size;
    rfbd.height = bitmap_size;
    rfbd.pixels = temp_bitmap;
    rfbd.bc.reserve(charnums - firstchar);

    stbtt_BakeFontBitmap(font_data.get_font_binery_dataB(), 0, glyphPosH, temp_bitmap, bitmap_size, bitmap_size, firstchar , charnums, rfbd.bc.data()); // no guarantee this fits!


    return rfbd;
}

