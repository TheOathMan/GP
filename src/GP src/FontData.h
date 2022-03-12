#pragma once
#include <vector>
#include "Blueprints.h"
#include "AppCore.h"
#include "../NMC/imgui/imstb_truetype.h"
#include <queue>


struct Ranged_Font_Bitmap_data { // FontData.cpp
    pixel_uc* pixels=nullptr;
    int width=0;
    int height=0;
    int bitmap_size=0;
    std::vector<stbtt_bakedchar> bc;
};

class FontData {
public:
    Gui_Handle gui_handle;

    FontData(const std::string& font_path);
    FontData(const std::string& font_path, const std::string& font_name);
    FontData(const FontData& other);
    FontData(FontData&& other) noexcept;
    FontData(std::string&& font_path);
    FontData(std::string&& font_path,std::string&& font_name);
    FontData& operator = (const FontData& other) = delete;
    FontData& operator = (FontData&& other) = delete;

    ~FontData();

    //virtual void ddf() = 0;

    // the glyph index inside the font file 
    Glyph_data GetGlyphBitmap_SDF(int glyph_index, float scale);
    // the glyph index inside the font file 
    Glyph_data GetGlyphBitmap(int glyph_index, float scale);
    // the index of any visiable unrepeatable glyphs inside the collection 
    Glyph_data GetGlyphBitmap_V(int glyph_index, float scale, int EdgeOffset = 4, Glyph_Data_Access Info_Inquiry = Glyph_Data_Access::Bitmap_Write);
    // the index of any visiable unrepeatable glyphs inside the collection 
    Glyph_data GetGlyphBitmap_SDF_V(int glyph_index, float scale, Glyph_Data_Access Info_Inquiry = Glyph_Data_Access::Bitmap_Write);
    // Get glyph pre data
    Glyph_data GetGlyphPreData(int glyph_index, float scale, Image_Type type, int SDF_edgeOffset = 5);

   
    inline const int get_visibleGlyphsN()                 const { return visibleGlyphs; }               // number of visible glyphs  
    inline const int get_TotalGlyphsN()                   const { return TotalGlyphs; }                 // number of Total Glyphs
    inline const int get_Duplicated_GlyphsN()             const { return DuplicatedGlyphs; }            // number of Duplicated Glyphs 
    inline const int get_invisibleGlyphsN()               const { return invisibleGlyphs; }             // number of invisible Glyphs 
    inline const size_t get_font_file_sizeN()             const { return m_fsize; }                     // the size of the font data

    const std::vector<int>& get_visible_unique_indicesC() const { return visible_unique_indices;   }    // vector containg the indices of all visible glyphs.
          std::vector<Glyph_Vertices>& get_glyph_listCC()       { return glyph_list;   }                // vector containg all visible glyphs Vertices.Pair it with get_visible_unique_indicesC
    const std::vector<char>& get_Combo_Pages()            const { return ComboPages;   }                // characters of number for pages. (char)
    const uint8_t* get_font_binery_dataB()                const { return font_binery_data; }            // get reference to the data pointer. 
    const std::string& get_font_path()                    const { return font_path; }                   // the stored path.
    const std::string& get_font_name()                    const { return font_name; }                   // font name if available

private:
    void init();
    std::string font_name;
    std::string font_path;
    int visibleGlyphs = 0;
    int TotalGlyphs = 0;
    int DuplicatedGlyphs = 0;
    int invisibleGlyphs = 0;
    size_t m_fsize;
    stbtt_fontinfo font;
    uint8_t* font_binery_data;
    std::vector<int> visible_unique_indices; // contains list of each visible unique glyph indices that.
    std::vector<Glyph_Vertices> glyph_list; // Contain lists of shapes of every visible available glyph. 
    std::vector<char> ComboPages;
};

Ranged_Font_Bitmap_data RangedBitmapDraw(const FontData& font_data, int firstchar, int charnums, float glyphPosH, int res);

//void DrawRawOpengl(const GLuint id);