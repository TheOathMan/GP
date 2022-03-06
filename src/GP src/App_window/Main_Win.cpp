/*
[INCLUDE]
[STATIC GLOBAL DATA]
[FUNCTIONS]
[EVENTS IMPL]
[GUI WIN CONTENT]
[MAIN WIN CLASS IMPL]
*/ 
//------ TODO:
// I might need to add async load bars for image rendering 
// make loading gui more flex 
// g++ static lincage 

#include "../defines.h"

#if defined ASYNC_METHOD_THREADING && defined LOCALTHREAD_METHOD_THREADING
    static_assert(false, " conflicting defines");
#endif

//---------------------------------- [SECTION]: INCLUDE----------------------------------
#include "Main_Win.h"

#include "../Image.h"
#include "../AppCore.h"
#include "../Blueprints.h"
#include "../FontData.h"
#include "../Job.h"
#include "../FileHandle.h"
#include "../../NMC/imgui/imgui.h"
#include "../../NMC/GLFW/glfw3.h"
#include "../../NMC/tinydialog/tinyfiledialogs.h"
#include "../../NMC/imgui/imgui_internal.h"

std::string LogInfo(int line, const char* fileName);


#define GUI_SEPERATION ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing()
// current selected font
# define Current_Font         LoadList.loaded_fonts.at(fs.SelectedFont)
# define Current_Glyph_Vertix Current_Font->get_glyph_listCC().at(fs.SelectedGlyph)



 //----------------------------------[SECTION]: STATIC GLOBAL DATA----------------------------------


// windows size/pos variables
struct Win_spec {
    ImGuiWindowFlags window_flags = 0;
    bool no_close = true;
    ImVec2 fgws; // final gui window size
    ImVec2 cgws_LHS;// current gui window size (left hand side window)
    ImVec2 cgws_RHS;// current gui window size (right hand side window)
    Vec2i cws;    // current window sizw        (whole window)
    bool resized = false;
    bool IsFocused = true;
    bool minimized = false;
} GPWins;

// font/glyph loading functions and listing
struct font_loading {
    int Successful_laods = 0;
    int Failed_Loads = 0;
    std::vector<std::string> TargetFontsNames; // last font file names that was attmpted to load
    void NewLoad(){TargetFontsNames.clear(); Successful_laods=0; Failed_Loads=0;   }

    std::vector<std::shared_ptr<FontData>> loaded_fonts;
    std::vector<std::shared_ptr<Image>> loaded_bitmaps; //list of glyphs bitmaps for preview 
#if defined ASYNC_METHOD_THREADING || defined LOCALTHREAD_METHOD_THREADING
    Job<void> loading_job;
#else
struct { bool is_working(){return false;}} loading_job ;
#endif    
    int Total_Font_Load=0;
    bool open_log =false;
    int SubWinsflags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
   // bool thread_working = false;
} LoadList;



// font/glyph selections properities --------------
struct font_selections {
    Image *SelectedImage=nullptr;
    int SelectedFont = 0;
    int SelectedGlyph = 0;
    int CurrentPage = 0;
    int glyph_setting = 0;
    bool Disable_Glyph_Selection= 0;
}fs;


struct glypg_shape_edit {
    const float glyphs_tex_Scla = 150.0f;
    float gl_color[3]{};
    float BG_color[3]={ 1,1,1 };
    int SDF_edgeOffset = 4;
    int  final_scale = 200;
    bool glyph_edit=false;
} gs;

// mouse in controller in 2d space and grid
struct glyph_edit_mouse_control {
    bool in_boarder=false;
     float scroll_v = 1.f; // scrolling value
     Vec2 Start_pos, updated_pos;
     Vec2 mouse_drag, acom_vec;
     Vec2 ZoomPos = Vec2(0.5f,0.5f);
    // bool vertex_move = false;
}sc;

struct glyph_vertex_edit {
    bool gui_xy=true, gui_cxy=true, gui_cxy1=true;
    bool vertex_move = false;
    int edit_index = -1;
    char type = {};
}ve;

struct Enterted {
    bool EditPage;
    bool GlyphPage;
    bool SavePage;
}entred;

struct Loading_GUI {
    float waitAbit = 0.0f;                       // for not closing the bar immediately..
    int Current_Font_Load = 0;

    void Log( const char* log_name, bool* open, const char* msg, const std::vector<std::string>& log_lists) {
  
        if (*open) {
            ImVec2 win_size = ImGui::GetWindowSize();
            ImVec2 nws(500,300);
            ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2((win_size.x / 2.0f) - nws.x/2.0f, (win_size.y / 2.0f) - nws.y/2.0f), ImGuiCond_Once);
            ImGui::Begin(log_name, open, LoadList.SubWinsflags, (int)Window_Resize_Dir::WRD_NONE);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

            ImVec2 this_GwinSize = ImGui::GetWindowSize();

            ImGui::BeginChild("childnames",ImVec2(this_GwinSize.x - ImGui::GetStyle().WindowPadding.x*2.0 ,this_GwinSize.y-105),true,LoadList.SubWinsflags);
            for (size_t i = 0; i < log_lists.size(); i++)
            {
                auto str = log_lists.at(i).c_str();
                if(str[0] == '1')
                    ImGui::Text("%s - Successfully Loaded", str + 1);
                if(str[0] == '0')
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s - Failed To Load", str + 1);
            }
            ImGui::EndChild();
            ImGui::TextUnformatted(msg);

            ImGui::PopStyleVar();
            if(LoadList.Total_Font_Load) 
                ImGui::SetScrollY(ImGui::GetScrollMaxY());
            ImGui::End();
        }
    }

    bool ProcessLoadingLists(const char* loading_text,int& total_loaded, bool load_is_processing,int list_size) 
    {
        bool result = false;
        if (total_loaded>0) {
            ImGui::OpenPopup("Loading Fonts");
            ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        }
        
        // open loading window once the loading thread has started 
        if (ImGui::BeginPopupModal("Loading Fonts", NULL, LoadList.SubWinsflags))
        {
            Current_Font_Load = list_size; //- loaded_fonts_size;

            ImGui::TextUnformatted(loading_text);
            ImGui::Separator();
            //float totf = Total_Font_Load;
            float out_val = (1.0f / total_loaded) * (float)Current_Font_Load;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            GpGUI::BufferingBar("##buffer_bar", load_is_processing ? out_val : 1, ImVec2(300, 6));
            ImGui::PopStyleVar();
            

            // thread ended its loading job, bar is full and we're waiting for a bit..
            if (out_val >= 1 || !load_is_processing) {
                waitAbit += 5.0f * ImGui::GetIO().DeltaTime;
                //Total_Font_Load = Current_Font_Load;
                if (waitAbit > 1.8f) {
                    total_loaded=0;
                    ImGui::CloseCurrentPopup();
                    waitAbit = 0.0f;
                    Current_Font_Load = 0;
                    result = true;
                }
            }
            ImGui::EndPopup();
        }
        return result;
    }

} LoadingGUI;

// GUI WRAPPER
struct GUI_WINDOW {
    //static int linked
    ImVec2 pos; ImVec2 size;
    GUI_WINDOW(const char* nnme, const ImVec2& windowPos, const ImVec2& windowSiz, Window_Resize_Dir df = Window_Resize_Dir::WRD_NONE) :
        pos(windowPos), size(windowSiz)
    {
        ImGui::Begin(nnme, &GPWins.no_close, GPWins.window_flags, (int)df);
        ImGui::SetWindowPos(windowPos);
        ImGui::SetWindowSize(windowSiz);
    }

    ~GUI_WINDOW() {
        ImGui::End();
    }
};

 //---------------------------------- [SECTION]: FUNCTIONS ----------------------------------

bool Selection_Gaurd() {
    if (!LoadList.loading_job.is_working() && !LoadList.loaded_fonts.empty() && fs.SelectedFont < LoadList.loaded_fonts.size() ) {
        //gs.SelectedFont = loaded_fonts.size()-1;
        if ( fs.SelectedGlyph >= Current_Font->get_glyph_listCC().size()) {
            fs.SelectedGlyph = Current_Font->get_glyph_listCC().size() - 1;
        }
        return true;
    }
    return false;
}

int LoadFont_Path(const char* path) {
   // GP_Print("---------------- 1 ----------------");
    if (path[0] == '\n') return NULL;
    const char* formats[] = { ".ttf",".otf",".ttc",".woff",".eot"}; // accepted formates
 
    const char* file_name;
    for (file_name = path + strlen(path); file_name > path && file_name[-1] != '/' && file_name[-1] != '\\'; file_name--) {}

    char lfile_name[252]; strcpy(lfile_name, file_name); 
    Tolower(lfile_name);


    for (int i=0;i< IM_ARRAYSIZE(formats);i++)
        if (!strstr(lfile_name,formats[i])) {
            auto fd = std::make_shared<FontData>(path, file_name);
            if (fd->gui_handle != Gui_Handle::Delete) { // meaning loading the font was successful
                LoadList.loaded_fonts.push_back(fd);
                LoadList.Successful_laods++;
                LoadList.TargetFontsNames.push_back(fd->get_font_name());
                auto& b = LoadList.TargetFontsNames.back();
                b = '1' + b;
                return 1;
            }
            else
            {
                LoadList.Failed_Loads++;
                LoadList.TargetFontsNames.push_back(fd->get_font_name());
                auto& b = LoadList.TargetFontsNames.back();
                b = '0' + b;

                LoadList.Total_Font_Load--;
            }
            break;
        }

    return NULL;
}


void FontLoadingJob(std::vector<std::string>&& fonst_paths, const int count) {
    //for(auto& t: fonst_paths) GP_Print(t.c_str());
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    if (fonst_paths.size() && count) {
        LoadList.NewLoad();
        LoadList.Total_Font_Load = count;
        for (int i = 0; i < count; i++) {
            LoadFont_Path(fonst_paths[i].c_str());
        }
    }
}


// process Image
void ImageProcess(Image* (&glyphTex), float scale, const Image_Type imgeType,bool GPU_Update, const  float gl_color[3] = {}, const  float bgColor[3] = {}) {
    fs.Disable_Glyph_Selection = true;
    DE_NUL(glyphTex);
    Glyph_data ft;
    if (Selection_Gaurd()) {
        if (imgeType == Image_Type::Alpha) {
            ft = Current_Font->GetGlyphBitmap_V(fs.SelectedGlyph, scale);
            glyphTex = new Image(ft.pixels, ft.width, ft.height, 1);
            glyphTex->BlackToAlpha();
        }

        if (imgeType == Image_Type::Background) {
            ft = Current_Font->GetGlyphBitmap_V(fs.SelectedGlyph, scale, gs.SDF_edgeOffset);
            glyphTex = new Image(ft.pixels, ft.width, ft.height, 1);
            // turn it to 4 channles when we want to send it to the GPU only.
            // other than that, make it 4 channles only when we adjust colors at SetColor2(col2, col)
            if (GPU_Update) glyphTex->To_RBGA();

            //IPP::Trim(*glyphTex);
        }

        if (imgeType == Image_Type::SDF) {
            ft = Current_Font->GetGlyphBitmap_SDF_V(fs.SelectedGlyph, scale);
            glyphTex = new Image(ft.pixels, ft.width, ft.height, 1);
            if(GPU_Update) glyphTex->To_RBGA();
        }
        

        if (imgeType != Image_Type::SDF && bgColor && gl_color) {
            color_t col = config_Color(bgColor[0] * 255, bgColor[1] * 255, bgColor[2] * 255, 255);
            color_t col2 = config_Color(gl_color[0] * 255, gl_color[1] * 255, gl_color[2] * 255, 255);
            glyphTex->SetColor2(col2, col);
        }
        if (GPU_Update) LoadImageToGPU(*glyphTex);
    }
}

// update glyphs texture upon new page selected or new font selected and update GlyphPreviewRender  
void Process_Glyph_Textures(bool trigger = false) {
    static int Prev_CurrentPage = 0;
    static int Prev_SelectedFont = 0;
    static int Prev_CurGlyphPos = 0;

    // process all glyphs textures
    if (fs.SelectedFont != Prev_SelectedFont || Prev_CurrentPage != fs.CurrentPage || trigger)
    {
        LoadList.loaded_bitmaps.clear();
        Prev_CurrentPage = fs.CurrentPage;
        Prev_SelectedFont = fs.SelectedFont;
        Event::Notify(OnRenderingPageGlyphs());
    }

    // process the selected glyphs textures
    if (Prev_CurGlyphPos != fs.SelectedGlyph) {
        Prev_CurGlyphPos = fs.SelectedGlyph;
        Event::Notify(OnGlyphPreviewRender());
    }
}


//-------------------------------------[SECTION]:EVENTS IMPL-------------------------------------

void DeletFontDataCallback(const EventType& ev) {
    auto b = LoadList.loaded_fonts.begin();
    auto e = LoadList.loaded_fonts.end();
    auto d = std::remove_if(b, e, [](std::shared_ptr<FontData> a) { return a->gui_handle == Gui_Handle::Delete; });
    LoadList.loaded_fonts.erase(d, e);   
    Process_Glyph_Textures(true);
   // Event::Notify(OnRenderingPageGlyphs());
}

void FontsLoadingCallback(const EventType& e) {
    LoadingGUI = Loading_GUI();
    auto db_e = static_cast<const OnFontsLoading&>(e);
    int count = db_e.count;
    //GP_Print("hello");
    if (!count)return;
    const char** paths = db_e.paths;
    
    if (LoadList.loading_job.is_working()) return;
    std::vector<std::string> g_spath; // hard copy needed for laoding thread
    for (size_t i = 0; i < count; i++) { g_spath.push_back(paths[i]); }


#if defined ASYNC_METHOD_THREADING
    LoadList.loading_job.give(FontLoadingJob, move(g_spath), count);
#elif defined LOCALTHREAD_METHOD_THREADING
    LoadList.loading_job.Start([=]() 
        mutable {FontLoadingJob(std::move(g_spath), count); });

#else
    FontLoadingJob(std::move(g_spath), count);
#endif
    db_e.count = 0;
}

void WindowResizeCallback(const EventType& e) {
    auto wr_e = static_cast<const OnWindowResize&>(e);
    GPWins.cws.x = wr_e.n_width;
    GPWins.cws.y = wr_e.n_height;
    GPWins.resized = true;
    GPWins.minimized = !wr_e.n_width && !wr_e.n_height;
}

void WindowFocusCallback(const EventType& e) {
    auto wr_e = static_cast<const OnWindowFocus&>(e);
    GPWins.IsFocused = wr_e.isFocused;
    
    //static int a,b;
    //glfwGetWindowSize(  ,&a,&b);
    //GP_Print("S1 " << a << "S2 " << b);
    //GPWins.minimized = !wr_e.n_width && !glfwGetWindowSize().y; 
}

void GlyphPreviewRenderCallback(const EventType& e) {
    sc = glyph_edit_mouse_control(); // reset
    ImageProcess(fs.SelectedImage, gs.glyphs_tex_Scla, (Image_Type)fs.glyph_setting, true, gs.gl_color, gs.BG_color); // default setting
}

void RenderingPageGlyphsCallback(const EventType& e) {
    //CELLS_NUMBER = fo.loaded_fonts.at(fs.SelectedFont)->get_visible_unique_indicesC().size();
    int CurGlyphPos = fs.CurrentPage * CELLS_NUMBER;

    if (fs.SelectedFont < LoadList.loaded_fonts.size()) {
        for (size_t i = CurGlyphPos; i < Current_Font->get_visible_unique_indicesC().size(); i++)
        {
            //GP_Print("%i", i);
            if (i - CurGlyphPos > CELLS_NUMBER) break;
            Glyph_data ft = Current_Font->GetGlyphBitmap_V(i, 150.0f);
            LoadList.loaded_bitmaps.push_back(std::make_shared<Image>(ft.pixels, ft.width, ft.height, 1));
            Image& im = *LoadList.loaded_bitmaps.back();
            im.BlackToAlpha(); LoadImageToGPU(im);
        }
        //GP_Print("Hello");
        Event::Notify(OnGlyphPreviewRender());
    }
}

void PreviewWinInitilizedCallback(const EventType& e) {
    //Recognize event
    auto win = static_cast<const OnPreviewWinInitialized&>(e);
    App_Window* s_win = reinterpret_cast<App_Window*>(win.window);
    // setup
    auto w = s_win->get_window();
    Image* img = nullptr;
    ImageProcess(img, gs.final_scale, (Image_Type)fs.glyph_setting, false, gs.gl_color, gs.BG_color);

    //update window size, view port size, close flag, visibility.
    glfwSetWindowSize(w, img->Get_Width(), img->Get_Height());
    glViewport(0, 0, img->Get_Width(), img->Get_Height());
    glfwSetWindowShouldClose(w, 0);
    glfwShowWindow(w);

    //move preview window to the middle.
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int ww = mode->width, hh = mode->height;
    glfwSetWindowPos(w, (ww / 2) - (img->Get_Width() / 2), (hh / 2) - (img->Get_Height() / 2));

    img->To_RBGA();
    LoadImageToGPU(*img);
    s_win->Set_texture_id(img);
    //App_Window::wins32.push_back(SecWindow);
}

void ClearFontsCallback(const EventType& e) {
    for (auto& glyph_setting : LoadList.loaded_fonts)
        glyph_setting->gui_handle = Gui_Handle::Delete; 
    fs.SelectedFont = 0;
    LoadList.loaded_bitmaps.clear();
    gs.glyph_edit = false;
    Event::Connect<OnDeleteFontData>(DeletFontDataCallback);
}

void ScrollingCallback(const EventType& e) {
    auto wr_e = static_cast<const OnScrolling&>(e);

    if (gs.glyph_edit && sc.in_boarder) {
        sc.ZoomPos = ImGui::GetMousePos();
        sc.scroll_v += 0.05f * wr_e.vertical;
        sc.scroll_v = Clamp(sc.scroll_v, 0.3f, 3.0f);
    }
    //if (sc.scr_val == 0) sc.scr_val = 1;
}

void EditPageEnterCallback(const EventType& e) {
    auto ep = static_cast<const OnEditPageEntered&>(e);
    GP_Print("Edit Page entered");
    Event::Notify(OnGlyphPreviewRender());
}
void SavePageEnterCallback(const EventType& e) {
    auto ep = static_cast<const OnSavePageEntered&>(e);
    GP_Print("Save Page entered");
}
void GlyphPageEnterCallback(const EventType& e) {
    auto ep = static_cast<const OnGlyphPageEntered&>(e);
    GP_Print("Glyph Page entered");
}

 //----------------------------------[SECTION]: GUI WIN CONTENT----------------------------------

void Main_Win::MainCanvesGUIWin() {
    if (!gs.glyph_edit) {
        // update glyphs texture upon new page or new font
        Process_Glyph_Textures();

        // combo of glypg pages page
        if (Selection_Gaurd()) {
            if (Current_Font->gui_handle != Gui_Handle::Delete && ImGui::Combo("Page", &fs.CurrentPage, Current_Font->get_Combo_Pages().data(), 10)) {
                fs.Disable_Glyph_Selection = true;
            }
            else fs.Disable_Glyph_Selection = false;
        }
        //else ImGui::Dummy(ImVec2(0, 2));


        ImGui::Spacing(); ImGui::Separator();
        ImVec2 ofst_dat = ImGui::GetCursorScreenPos();


        // organizing glyph textures, glyph boxes and align it accordingly-------------------------------
        ImGui::BeginChild("child window");

        float cell_size = 150.0f;
        float cell_spacing = 20.0f;
        ImVec2 base_pos = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        int cel_num_raw = GPWins.fgws.x / (cell_size + cell_spacing);
        float remaining = (GPWins.fgws.x - cel_num_raw * (cell_size + cell_spacing + 5.0f)) / (cel_num_raw - 1);
        float last_hight = 0;
        for (uint id = 0; id < CELLS_NUMBER; id++)
        {

            int CurGlyphPos = id + fs.CurrentPage * CELLS_NUMBER;
            static int cellpicked = 0;
            // Cell coloring 
            ImU32 cel_col = cellpicked == id ? IM_COL32(255, 255, 255, 200) : IM_COL32(255, 255, 255, 80);
            if (Selection_Gaurd())
                if (CurGlyphPos >= Current_Font->get_glyph_listCC().size()) // FIXME: Possible thread related error
                    cel_col = IM_COL32(255, 255, 255, 30);

            // Organizing cells horizontally
            ImVec2 cell_p1(base_pos.x + (id % cel_num_raw) * (cell_size + cell_spacing + remaining), base_pos.y + (id / cel_num_raw) * (cell_size + cell_spacing));
            ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
            draw_list->AddRect(cell_p1, cell_p2, cel_col);
            // when we have a glyph texture presented, we fit it to each cell accordingly, taking it's width and hight 
            Image* thisImage = LoadList.loaded_bitmaps.empty() || id >= LoadList.loaded_bitmaps.size() ? nullptr : LoadList.loaded_bitmaps.at(id).get();

            if (thisImage) {
                last_hight = thisImage->Get_Height();
                ImVec2 offst(ofst_dat.x, ofst_dat.y - ImGui::GetScrollY());     // BeginChild() addes an un-accounted for offset to the the imges. We cancel it here.
                offst.x -= ((cell_p2.x - cell_p1.x) - thisImage->Get_Width()) / 2.0f;
                offst.y -= ((cell_p2.y - cell_p1.y) - thisImage->Get_Height()) / 2.0f;
                ImGui::PushClipRect(cell_p1, cell_p2, true);
                ImGui::SetCursorPos(ImVec2(cell_p1.x - offst.x, cell_p1.y - offst.y));
                ImVec2 imagesize(thisImage->Get_Width(), thisImage->Get_Height());
                if (cel_col != IM_COL32(255, 255, 255, 30) && Selection_Gaurd()) ImGui::Image((ImTextureID)(intptr_t)thisImage->Get_GPU_ID(), imagesize);

                ImGui::PopClipRect();
                // are we hovering over this cell ?
                if (ImGui::IsMouseHoveringRect(cell_p1, cell_p2) && !LoadList.loading_job.is_working())
                {
                    // left mouse click on a glyph
                    if (ImGui::IsMouseClicked(0) && !fs.Disable_Glyph_Selection) {
                        cellpicked = id;
                        fs.SelectedGlyph = CurGlyphPos;
                        GP_Print(fs.SelectedGlyph << "/" << (LoadList.loaded_fonts.size() ? Current_Font->get_glyph_listCC().size() - 1 : 0));
                        //ImageProcess(fs.SelectedImage, gs.glyphs_tex_Scla, (Image_Type)gs.glyph_setting, true, color, BG_color);
                    }

                    // right mouse click on a glyph
                    if (ImGui::IsMouseDown(1) && !fs.Disable_Glyph_Selection) {
                        ImGui::BeginTooltip();
                        ImGui::Image((ImTextureID)(intptr_t)thisImage->Get_GPU_ID(), imagesize);
                        ImGui::Text("Index Number: %i", CurGlyphPos);
                        ImGui::EndTooltip();

                    }
                }
            }
        }
        ImGui::Dummy(ImVec2(0, 150 - last_hight));
        ImGui::EndChild();
    }
    else {
        /*
            Edit logic
        */

        if (Selection_Gaurd()) {

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 cursor_p = ImGui::GetCursorScreenPos();      // draw Cursor position
            ImVec2 avi_size = ImGui::GetContentRegionAvail();                      // LHS window size
            if (avi_size.x < 50.0f) avi_size.x = 50.0f;
            if (avi_size.y < 50.0f) avi_size.y = 50.0f;
            Vec2i canvas_p(cursor_p.x + avi_size.x, cursor_p.y + avi_size.y);

            Glyph_Vertices& CurrentGlyphVert = Current_Glyph_Vertix;
            ImVec2 mous_pos = ImGui::GetMousePos();
            sc.in_boarder = (mous_pos.x < canvas_p.x) && (mous_pos.x > cursor_p.x) && (mous_pos.y < canvas_p.y) && (mous_pos.y > cursor_p.y);


            static bool in = false;
            if (ImGui::IsMouseClicked(0) && sc.in_boarder) {
                in = true;
                sc.Start_pos = Vec2(mous_pos);
            }
            if (ImGui::IsMouseDown(0) && in && !ve.vertex_move) {
                sc.updated_pos = Vec2(mous_pos);
                sc.mouse_drag = (sc.updated_pos - sc.Start_pos) / sc.scroll_v + sc.acom_vec;
            }
            if (ImGui::IsMouseReleased(0) && in) {
                ve.vertex_move = false;
                sc.acom_vec = sc.mouse_drag;
                in = false;
            }


            const auto Projuction = [&](float x, float y) {
                float X = (x - CurrentGlyphVert.origin.x) * sc.scroll_v;
                float Y = (-y + CurrentGlyphVert.origin.y) * sc.scroll_v;
                return Vec2(X, Y);
            };

            // center verts to the middle
            Vec2i to_op(canvas_p.x >> 1, canvas_p.y >> 1);

            const auto view = [&](float x, float y) {
                Vec2 r = Projuction(x, y);
                return ImVec2(r.x + to_op.x, r.y + to_op.y);
            };

            const auto GlyphToScreen = [&](float x, float y) {
                Vec2 r = view(x + sc.mouse_drag.x, y - sc.mouse_drag.y);
                return ImVec2(r.x, r.y);
            };

            // Transfom vec2 from screen space to glyph space
            const auto ScreenToGlyph = [&](float x, float y) {
                float descaler = (1.0f / sc.scroll_v);
                float X = CurrentGlyphVert.origin.x - sc.mouse_drag.x + (x - to_op.x) * descaler;
                float Y = CurrentGlyphVert.origin.y + sc.mouse_drag.y - (y - to_op.y) * descaler;
                return ImVec2(X, Y);
            };

            draw_list->AddRectFilled(cursor_p, ImVec2(canvas_p.x, canvas_p.y), IM_COL32(50, 50, 50, 255));
            draw_list->AddRect(cursor_p, ImVec2(canvas_p.x, canvas_p.y), IM_COL32(255, 255, 255, 255));
            draw_list->PushClipRect(cursor_p, ImVec2(canvas_p.x, canvas_p.y), true);


            // align scrolling value with centeric
            ImVec2 scrolling(sc.mouse_drag.x * sc.scroll_v - (to_op.x * sc.scroll_v - to_op.x), sc.mouse_drag.y * sc.scroll_v - (to_op.y * sc.scroll_v - to_op.y));

            const float GRID_STEP = 64.0f * sc.scroll_v;

            for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_p.x; x += GRID_STEP)
            {
                draw_list->AddLine(ImVec2(cursor_p.x + x, cursor_p.y), ImVec2(cursor_p.x + x, canvas_p.y), IM_COL32(200, 200, 200, 40));
            }
            for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_p.y; y += GRID_STEP)
            {
                draw_list->AddLine(ImVec2(cursor_p.x, cursor_p.y + y), ImVec2(canvas_p.x, cursor_p.y + y), IM_COL32(200, 200, 200, 40));
            }

            ImVec2 origin = view(CurrentGlyphVert.origin.x, CurrentGlyphVert.origin.y);
            DRAW_TEXT(draw_list, gui_spc->_main_font_Big, 35, IM_COL32(50, 255, 50, 200), origin.x, origin.y, "X");

            ImVec2 mousPonInGlyphPos = ScreenToGlyph(mous_pos.x, mous_pos.y);
            if (GPWins.IsFocused)
                DRAW_TEXT(draw_list, gui_spc->_main_font_Big, 35, IM_COL32(200, 200, 200, 200), cursor_p.x + 8, cursor_p.y + 8, "(%0.1f, %0.1f)", mousPonInGlyphPos.x, mousPonInGlyphPos.y);

            enum verts_data : short { xy = 0, cxy = 2, cxy1 = 4 };
            bool SegmentEntred = false;
            ImVec2 StartSegpos;

            int grabed_index = 0;
            for (int i = 0; i < CurrentGlyphVert.ver_size; i++)
            {

                float line_thickness = 0.5f;
                const int segment_numC = 8;             // circle
                int resulotion = 15, segment_numL = 0;   // line

                ImU32 LineColor = IM_COL32(255, 155, 100, 200);
                ImU32 v_Color = IM_COL32(255, 255, 255, 200); // control vertix color.
                ImU32 c_Color = IM_COL32(255, 0, 0, 200); // control vertix color.
                ImU32 c1_Color = IM_COL32(0, 0, 255, 200); // control vertix color.

                const auto is_empty = [](stbtt_vertex& v, verts_data d) {  return ((short*)&v)[d] == 0 && ((short*)&v)[d + 1] == 0; };

                stbtt_vertex& thisVert = CurrentGlyphVert.verts[i];
                stbtt_vertex& nextVert = (i < CurrentGlyphVert.ver_size - 1) ? CurrentGlyphVert.verts[i + 1] : CurrentGlyphVert.verts[0];
                stbtt_vertex& prev_v = (i > 0) ? CurrentGlyphVert.verts[i - 1] : CurrentGlyphVert.verts[i];

                // current vertex data in glyph space
                ImVec2 v_xy = GlyphToScreen(thisVert.x, thisVert.y);
                ImVec2 v_cxy = GlyphToScreen(thisVert.cx, thisVert.cy);
                ImVec2 v_cxy1 = GlyphToScreen(thisVert.cx1, thisVert.cy1);

                // next vertex data in glyph space
                ImVec2 nv_xy = GlyphToScreen(nextVert.x, nextVert.y);
                ImVec2 nv_cxy = GlyphToScreen(nextVert.cx, nextVert.cy);
                ImVec2 nv_cxy1 = GlyphToScreen(nextVert.cx1, nextVert.cy1);

                // previous vertex data in glyph space
                ImVec2 pv_xy = GlyphToScreen(prev_v.x, prev_v.y);

                // calculate segment numbers based on two, current vertex and next vertex beased on the next controll point (disable for fixed segemnt number).
                Vec2 p1 = v_xy, p = nv_cxy, p2 = nv_xy, p3 = nv_cxy1;
                Vec2 dir1 = p1 - p, dir2 = p2 - p, dir3 = p1 - p3, dir4 = p2 - p3;
                float ang1 = acos(Dot(dir1.Normalize(), dir2.Normalize())) * RAD2DEG;
                float ang2 = acos(Dot(dir3.Normalize(), dir4.Normalize())) * RAD2DEG;
                ang1 = ang1 / 180.0f, ang1 = (1 - ang1) * resulotion;
                segment_numL += static_cast<int> (Max(ceilf(ang1), 1.0f));
                ang2 = ang2 / 180.0f, ang2 = (1 - ang2) * resulotion;
                segment_numL += static_cast<int> (Max(ceilf(ang2), 1.0f));

                //show cords for verts/ connect control lines
                if (distance(v_xy.x, v_xy.y, mous_pos.x, mous_pos.y) < 4.5f && !ve.vertex_move && ve.gui_xy) {
                    DRAW_TEXT(draw_list, gui_spc->_main_font_Big, 0, v_Color, mous_pos.x - 30.0f, mous_pos.y - 30.0f, "V_XY: (%i, %i) this type = %i next type = %i", thisVert.x, thisVert.y, thisVert.type, nextVert.type);
                    LineColor = IM_COL32(238, 255, 0, 255);
                    line_thickness = 1.5f;
                    //grab a vert upon click
                    if (ImGui::IsMouseDown(0))
                    {
                        ve.type = 1;
                        ve.edit_index = i;
                        ve.vertex_move = true;
                    }
                }
                if (distance(v_cxy.x, v_cxy.y, mous_pos.x, mous_pos.y) < 4.5f && !ve.vertex_move && ve.gui_cxy) {
                    DRAW_TEXT(draw_list, gui_spc->_main_font_Big, 0, c_Color, mous_pos.x - 30.0f, mous_pos.y - 30.0f, "C_XY: (%i, %i)", thisVert.cx, thisVert.cy);
                    draw_list->AddLine(v_cxy, pv_xy, c_Color, line_thickness);
                    ///grab a vert upon click
                    if (ImGui::IsMouseDown(0)) {
                        ve.type = 2;
                        ve.edit_index = i;
                        ve.vertex_move = true;
                    }
                }
                if (distance(v_cxy1.x, v_cxy1.y, mous_pos.x, mous_pos.y) < 4.5f && !ve.vertex_move && ve.gui_cxy1) {
                    DRAW_TEXT(draw_list, gui_spc->_main_font_Big, 0, c1_Color, mous_pos.x - 30.0f, mous_pos.y - 30.0f, "C1_XY: (%i, %i)", thisVert.cx1, thisVert.cy1);
                    draw_list->AddLine(v_cxy1, nv_xy, c1_Color, line_thickness);
                    //grab a vert upon click
                    if (ImGui::IsMouseDown(0))
                    {
                        ve.type = 3;
                        ve.edit_index = i;
                        ve.vertex_move = true;
                    }
                }

                // vertex edit
                if (ve.vertex_move && i == ve.edit_index) {
                    if (ve.type == 1) {
                        thisVert.x = mousPonInGlyphPos.x;
                        thisVert.y = mousPonInGlyphPos.y;
                    }
                    if (ve.type == 2) {
                        thisVert.cx = mousPonInGlyphPos.x;
                        thisVert.cy = mousPonInGlyphPos.y;
                    }
                    if (ve.type == 3) {
                        thisVert.cx1 = mousPonInGlyphPos.x;
                        thisVert.cy1 = mousPonInGlyphPos.y;
                    }
                }

                //start & end segemnt process
                char type = nextVert.type;
                if (thisVert.type == STBTT_vmove) {
                    StartSegpos.x = v_xy.x;
                    StartSegpos.y = v_xy.y;
                    SegmentEntred = true;
                }
                if (nextVert.type == STBTT_vmove) {
                    nv_xy.x = StartSegpos.x;
                    nv_xy.y = StartSegpos.y;
                    SegmentEntred = false;
                    type = thisVert.type;
                }

                // draw Bezier
                switch (type) {
                case STBTT_vline:
                    draw_list->AddLine(v_xy, nv_xy, LineColor, line_thickness);
                    break;
                case STBTT_vcurve:

                    draw_list->AddBezierCurve(v_xy,
                        is_empty(nextVert, cxy) ? nv_xy : nv_cxy,
                        nv_xy,
                        nv_xy,
                        LineColor, line_thickness, segment_numL);
                    break;
                case STBTT_vcubic:
                    draw_list->AddBezierCurve(v_xy,
                        is_empty(nextVert, cxy) ? nv_xy : nv_cxy,
                        is_empty(nextVert, cxy1) ? nv_xy : nv_cxy1, // -12851
                        nv_xy,
                        LineColor, line_thickness, segment_numL);
                    break;
                }

                // draw verts circle positions
                if (!is_empty(thisVert, xy) && ve.gui_xy)
                    gui_spc->_main_font_Big->RenderChar(draw_list, 29, ImVec2(v_xy.x - 6, v_xy.y - 16), v_Color, 'o');
                if (!is_empty(thisVert, cxy) && (thisVert.type == STBTT_vcurve || thisVert.type == STBTT_vcubic) && ve.gui_cxy)
                    gui_spc->_main_font_Big->RenderChar(draw_list, 29, ImVec2(v_cxy.x - 6, v_cxy.y - 16), c_Color, 'o');
                if (!is_empty(thisVert, cxy1) && (thisVert.type == STBTT_vcubic) && ve.gui_cxy1)
                    gui_spc->_main_font_Big->RenderChar(draw_list, 29, ImVec2(v_cxy1.x - 6, v_cxy1.y - 16), c1_Color, 'o');
            }

            // To Do, 
            //1 - options at glyph edit
            //2 - update glyohs upon edit
            //3 - fix vertex grabing multiples

        }

    }

}
void Main_Win::GlyphProcessPages() {

    char stuf[100];
    sprintf_s(stuf, "%0.1f", 1.5f);
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        // font info tap
        if (ImGui::BeginTabItem("      Edit      "))
        {
            if (entred.EditPage) { entred.EditPage = false; Event::Notify(OnEditPageEntered());}
            BIG_TEXT("Edit Glyph Image: "); GpGUI::SPINE();
            //ImGui::SameLine(ImGui::GetWindowSize().x - 90.0f, 0.0f); ImGui::Button("Preview");

            // Checker baord for image previewing. ---------------------------
            ImVec2 ChickerBoardPos = ImGui::GetCursorScreenPos();
            static float ChickerBoardHight = 120;
            ImGuiColorEditFlags chickerBoardflags = ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop;
            ImGui::ColorButton("##current", ImVec4(0.0f, 0.0f, 0.0f, 0.0f), chickerBoardflags, ImVec2(4000, ChickerBoardHight));
            GUI_SEPERATION;

            // preview glyph image ------------------ && gs.SelectedGlyph < gs.Glyphs_bitmap.size()
            float spacing = 40.0f;
            if (Selection_Gaurd() && fs.SelectedImage) {
                int w = fs.SelectedImage->Get_Width(), h = fs.SelectedImage->Get_Height();
                ImVec2 drawPos = ImVec2(ChickerBoardPos.x - (w / 2.0f) + GPWins.cgws_RHS.x / 2.0f, ChickerBoardPos.y + spacing / 2);
                ImGui::GetWindowDrawList()->AddImage((ImTextureID)(intptr_t)fs.SelectedImage->Get_GPU_ID(), drawPos, ImVec2(drawPos.x + w, drawPos.y + h));
                ChickerBoardHight = h + spacing;
            }
            else ChickerBoardHight = 120;

            if (ImGui::BeginChild("child windodsw2"))
            {

                // image pre-settins-------------------------
                //ImGui::PushItemWidth(-80.0f);
                if (GpGUI::LinkedCheckBox("Background", &fs.glyph_setting, (int)Image_Type::Background)) { ImageProcess(fs.SelectedImage, gs.glyphs_tex_Scla, (Image_Type)fs.glyph_setting, true, gs.gl_color, gs.BG_color); };
                if (GpGUI::LinkedCheckBox("Alpha", &fs.glyph_setting, (int)Image_Type::Alpha)) { ImageProcess(fs.SelectedImage, gs.glyphs_tex_Scla, (Image_Type)fs.glyph_setting, true, gs.gl_color, gs.BG_color); };
                if (GpGUI::LinkedCheckBox("SDF", &fs.glyph_setting, (int)Image_Type::SDF)) { ImageProcess(fs.SelectedImage, gs.glyphs_tex_Scla, (Image_Type)fs.glyph_setting, true, gs.gl_color, gs.BG_color); };

                // Edge Spacing ---------------------------
                if (fs.glyph_setting == 0 && ImGui::SliderInt("Edge Spacing", &gs.SDF_edgeOffset, 1, 100, "%i"))
                    ImageProcess(fs.SelectedImage, gs.glyphs_tex_Scla, (Image_Type)fs.glyph_setting, true, gs.gl_color, gs.BG_color); ;

                // Color picker-----------------------------
                if (fs.glyph_setting < 2 && ImGui::ColorEdit3("GL color ", gs.BG_color, ImGuiColorEditFlags_PickerHueWheel)) {
                    ImageProcess(fs.SelectedImage, gs.glyphs_tex_Scla, (Image_Type)fs.glyph_setting, true, gs.gl_color, gs.BG_color);
                }
                if (fs.glyph_setting == 0 && ImGui::ColorEdit3("BG color", gs.gl_color, ImGuiColorEditFlags_PickerHueWheel)) {
                    ImageProcess(fs.SelectedImage, gs.glyphs_tex_Scla, (Image_Type)fs.glyph_setting, true, gs.gl_color, gs.BG_color);
                }


                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        } else entred.EditPage = true;

        if (ImGui::BeginTabItem("      Save      "))
        {
            if (entred.SavePage) { entred.SavePage = false; Event::Notify(OnSavePageEntered()); }

            // firs char used as an option. 1=bad log,  0=good log.
            static const char* LogResult = " ";

            if (ImGui::BeginChild("child windodsw1"))
            {
                BIG_TEXT("Save Glyph Image: "); GpGUI::SPINE();
                 //gui_spc->style->ItemSpacing.y = 14;

                // glyph resolution--------------------
                int res_min = 100, res_max = 10000; Glyph_data res_data;
                if (Selection_Gaurd()) {
                    res_data = Current_Font->GetGlyphPreData(fs.SelectedGlyph, gs.final_scale, (Image_Type)fs.glyph_setting, gs.SDF_edgeOffset);
                    // FIXME: Possible rare thread related error
                    for (; Current_Font->GetGlyphPreData(fs.SelectedGlyph, res_max, (Image_Type)fs.glyph_setting, gs.SDF_edgeOffset).width > 4000 ||
                           Current_Font->GetGlyphPreData(fs.SelectedGlyph, res_max, (Image_Type)fs.glyph_setting, gs.SDF_edgeOffset).height> 4000;
                        res_max -= 20) { }
                }
                ImGui::TextUnformatted("Resolution"); ImGui::SameLine(); ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2.55f);ImGui::PushItemWidth(-4.4f);

                static char buf[200];
                sprintf_s(buf, "%u X %u", res_data.width, res_data.height);
                if (ImGui::SliderScalar("  ", ImGuiDataType_S32, &gs.final_scale, &res_min, &res_max, buf, ImGuiSliderFlags_Logarithmic)) { 
                    if (!res_data.width)
                        LogResult = "1no Image"; 
                }

                // preview button ------------------ && gs.SelectedGlyph < gs.Glyphs_bitmap.size()
                if (Selection_Gaurd() && ImGui::Button("Preview", ImVec2(ImGui::GetWindowSize().x - gui_spc->style->FramePadding.x, 35))) {
                    static GLFWwindow* win = window;
                    Event::Connect<OnPreviewWinInitialized>(PreviewWinInitilizedCallback); // window to preview glyph texture
                }

                ImVec2 buttonsize(70, 30);

                // browse for a file path. ------------------
                static char filePAthBuff[512];
                static char imageNameBuf[164];
                static int GUI_cur_format = 0;                    // GUI_Combo data
                static const char* fn;
                if (ImGui::Button(" Browse ", buttonsize))
                {
                    fn = fn = tinyfd_selectFolderDialog("Browse For Folder", NULL);
                    if (fn) {
                        LogResult = "0Path found";
                        strcpy_s(filePAthBuff, fn);
                    }
                    else
                        LogResult = "1No path Found!";
                }
                ImGui::SameLine();
                //ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.55f);
                static ImVec2 size(100.0f, 100.0f);
                ImGui::PushItemWidth(-4.4f);
                ImGui::InputText(" ", filePAthBuff, IM_ARRAYSIZE(filePAthBuff));

                // Save image ------------------
                if (ImGui::Button("Save As ", buttonsize)) {
                    if (strlen(imageNameBuf) && strlen(filePAthBuff)) {

                        //fs.glyph_setting; (int)Image_Type::Background;

                        Image* img = nullptr;
                        ImageProcess(img, gs.final_scale, (Image_Type)fs.glyph_setting, false, gs.gl_color, gs.BG_color);
                        char tempStr[512];
                        strcpy_s(tempStr,filePAthBuff);
                        strcat_s(tempStr, "\\");
                        strcat_s(tempStr, imageNameBuf);
                        if (img) {
                            switch (GUI_cur_format)
                            {
                                case (0):SaveImage<Image_Format::PNG>(tempStr, *img); break;
                                case (1):SaveImage<Image_Format::JPG>(tempStr, *img); break; //
                                case (2):SaveImage<Image_Format::BMP>(tempStr, *img); break; //
                                case (3):SaveImage<Image_Format::TGA>(tempStr, *img); break;
                            }
                            DE_NUL(img);
                            LogResult = "0Image Saved";
                        }
                        else { LogResult = "1No Image Selected"; }
                    }else
                        LogResult = "1No image path, or no image name was found!";
                }
                ImGui::SameLine();
                ImGui::PushID(1);
                ImGui::PushItemWidth(-120.4f);
                ImGui::InputText(" ", imageNameBuf, IM_ARRAYSIZE(imageNameBuf));


                // Begin Combo to pick image formate ------------------
                ImGui::SameLine();
                ImGui::PushItemWidth(-5.4f);
                const char* items[] = { ".png", ".jpg", ".bmp", ".tga" };
                static ImGuiComboFlags flags = ImGuiComboFlags_HeightSmall;
                const char* combo_label = items[GUI_cur_format];  // Label to preview before opening the combo (technically it could be anything)
                if (ImGui::BeginCombo("", combo_label, flags))
                {
                    for (int id = 0; id < IM_ARRAYSIZE(items); id++)
                    {
                        //unsupported formates for 4 channels
                        if (fs.glyph_setting == (int)Image_Type::Alpha && ( strcmp(items[id],".jpg") || strcmp(items[id] , ".bmp") )) continue;

                        const bool is_selected = (GUI_cur_format == id);
                        if (ImGui::Selectable(items[id], is_selected))
                            GUI_cur_format = id;

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::PopID();
                ImGui::PopItemWidth();

                // Result of image save:

                GpGUI::SPINE(0, 0, 0, GPWins.cgws_RHS.y - 380);
                ImVec2 curOus = ImGui::GetCursorScreenPos();
                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(curOus.x, curOus.y), ImVec2(GPWins.cws.x, curOus.y + 150), IM_COL32(38, 38, 38, 255));
                ImGui::TextColored(LogResult[0] == '1' ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.0f, 1.0f, 0.0f, 1.0f),"%s", (LogResult+1));


                ImGui::EndChild();
            }
            ImGui::EndTabItem();
        } else entred.SavePage = true;
        // contain glyph shape info with the ability to copy data
        if (ImGui::BeginTabItem("      Glyph      "))
        {
            int debugin = 0;
            if (entred.GlyphPage) { entred.GlyphPage = false; Event::Notify(OnGlyphPageEntered()); }

            BIG_TEXT("Glyph Shape Data: "); GpGUI::SPINE();

            if (Selection_Gaurd()) {

                const Glyph_Vertices& CurrentGlyphVert = Current_Glyph_Vertix;

                if (ImGui::Button(gs.glyph_edit ? "End Editing" : "Edit Vertices", ImVec2(ImGui::GetWindowSize().x - gui_spc->style->FramePadding.x, 35))) {
                    if (gs.glyph_edit) {
                        Event::Notify(OnRenderingPageGlyphs());
                    }
                    gs.glyph_edit = !gs.glyph_edit;
                    sc = glyph_edit_mouse_control(); // reset TODO:
                }

                if (gs.glyph_edit) {
                    GpGUI::CheckBox("XY", &ve.gui_xy);
                    GpGUI::CheckBox("CXY1", &ve.gui_cxy);
                    if (CurrentGlyphVert.max_type == STBTT_vcubic) GpGUI::CheckBox("CXY2", &ve.gui_cxy1);        
                }
                ImGuiStyle& style = ImGui::GetStyle();
                int avi_types = CurrentGlyphVert.max_type - 1; // availiable control points
                float child_w = (ImGui::GetContentRegionAvail().x - 1 * style.ItemSpacing.x) / avi_types;
                static int cpyVertz = -1; // -1: don't copy, 0 first group, 1 second group
                static std::string copyDis;
                static Glyph_Vertices Null;

                // ImGui::Text("Glyph type: %i", sel_glyph.verts->type);
                ImGui::Text("Number of verticies: %i", CurrentGlyphVert.ver_size);

                for (int i = 0; i < avi_types; i++)
                {
                     //GP_Print("pass: " << ++debugin);
                    if (i > 0) ImGui::SameLine();
                    ImGui::BeginGroup();
                    const char* names[] = { "(x,y)","(cx1,cy1)","(cx2,cy2)" };
                    ImGui::TextUnformatted(names[i]);
                    ImGui::PushID(i);
                    ImGui::BeginChild("Disply", ImVec2(child_w, 200.0f), true);
                    //
                    for (int item = 0; item < CurrentGlyphVert.ver_size; item++)
                    {
                        const stbtt_vertex& verts = CurrentGlyphVert.verts[item]; Vec2i p1;
                        switch (i)
                        {
                        case (0): p1 = Vec2i(verts.x, verts.y);   break;
                        case (1): p1 = Vec2i(verts.cx, verts.cy);  break;
                        case (2): p1 = Vec2i(verts.cx1, verts.cy1); break;
                        }

                        static char buf[20];
                        sprintf_s(buf, "(%d,%d) ", p1.x, p1.y);

                        // get string for copy clipboard
                        ImGui::TextUnformatted(buf);
                        if (cpyVertz == i) copyDis.append(buf);
                    }
                    // copy command
                    if (cpyVertz == i) {
                        glfwSetClipboardString(window, copyDis.c_str());
                        copyDis.clear();
                        cpyVertz = -1;
                    }
                    ImGui::EndChild();

                    if (ImGui::BeginPopupContextItem(" COPY context menu"))
                    {
                        if (ImGui::Button("Copy")) { cpyVertz = i; ImGui::CloseCurrentPopup(); }
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();

                //GP_Print("pass: " << ++debugin);


                    ImGui::EndGroup();
                }
            }
            //if (Selection_Gaurd()) {
            //    ImGui::Text("Number Of Visibale Glyphs: %i", fo.loaded_fonts.at(fs.SelectedFont)->get_visibleGlyphsN());
            //    ImGui::Text("Number Of Invisiable Glyphs: %i", fo.loaded_fonts.at(fs.SelectedFont)->get_invisibleGlyphsN());
            //    ImGui::Text("Number Of Duplicated Glyphs: %i", fo.loaded_fonts.at(fs.SelectedFont)->get_Duplicated_GlyphsN());
            //}
                            //GP_Print("pass: " << ++debugin);

            ImGui::EndTabItem();
        }else entred.GlyphPage = true;
        ImGui::EndTabBar();
    }


}
void Main_Win::FontSelections() {

    // window with list of font loaded ---------------------
    ImGui::CollapsingHeader("Fonts", ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_FramePadding);
    if (ImGui::BeginChild("child window"))
    {
        static int selected = -1;
        for (uint id = 0; id < LoadList.loaded_fonts.size(); id++)
        {
            if (Selection_Gaurd() && LoadList.loaded_fonts.at(id)->gui_handle == Gui_Handle::Visible) { // - erorr
                ImGui::PushID(id);
                // get selecttion value
                if (ImGui::Selectable(LoadList.loaded_fonts.at(id)->get_font_name().c_str(), selected == id)) {
                    //Selection_Gaurd();
                    fs.SelectedFont = id;
                    selected = id;
                    fs.CurrentPage = 0;
                }

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::Button("Remove")) {
                        auto TargetFont = LoadList.loaded_fonts.at(id);
                        TargetFont->gui_handle = Gui_Handle::Delete;
                        ImGui::CloseCurrentPopup();
                        Event::Connect<OnDeleteFontData>(DeletFontDataCallback);
                    }
                    ImGui::EndPopup();
                }
                ImGui::PopID();
                ImGui::SameLine(GPWins.cgws_RHS.x - 105.f);
                // Add file size on the side
                float fn = Selection_Gaurd() ? LoadList.loaded_fonts.at(id)->get_font_file_sizeN() / 1e+6 : 0;
                ImGui::Text(fn > 99 ? "%0.1f MB" : fn > 10 ? "%0.2f MB" : "%0.3f MB", fn);
            }
            //else
            //    // handle deleted fonts and move them to the back
            //    if (LoadList.loaded_fonts.at(id)->gui_handle == Gui_Handle::Delete)
            //        moveItemToBack(LoadList.loaded_fonts, id);

        }
        ImGui::EndChild();
    }


}
void Main_Win::MenuBar() {

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowMenuFile();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("about"))
        {
            ShowAbout();

            ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

 //----------------------------------[SECTION]: MAIN WIN CLASS IMPL----------------------------------

Main_Win::Main_Win(const char* window_name, int width, int height) : App_Window(window_name, width, height)
{
   // float dis = distance(Vec2(-1, 1), Vec2(-1, -2));
    //ImRect rec1(0, 0, 5, 5);
    //auto g = rec1.Overlaps(ImRect(4, 4, 10, 10));

    GPWins.cws.x = width;
    GPWins.cws.y = height;
}

Main_Win::~Main_Win()
{
    // Cleanup
    gui_spc->GUI_OnDestruction();
}

std::vector<int> nums;

void Main_Win::OnWindowAwake()
{
    App_Window::OnWindowAwake();
    SetBackgroundColor(15, 15, 15);
    glfwSetWindowMaximizeCallback(window, [](GLFWwindow*,int i) { GP_Print(i); }  );

    ///---------------- Register Window Events ---------------------------
    Event::Connect<OnFontsLoading>(FontsLoadingCallback);
    Event::Connect<OnWindowResize>(WindowResizeCallback);
    Event::Connect<OnWindowFocus>(WindowFocusCallback);
    Event::Connect<OnGlyphPreviewRender>(GlyphPreviewRenderCallback);
    Event::Connect<OnRenderingPageGlyphs>(RenderingPageGlyphsCallback);
    Event::Connect<OnAllFontsClear>(ClearFontsCallback);
    Event::Connect<OnScrolling>(ScrollingCallback);
    Event::Connect<OnEditPageEntered>(EditPageEnterCallback);
    Event::Connect<OnSavePageEntered>(SavePageEnterCallback);
    Event::Connect<OnGlyphPageEntered>(GlyphPageEnterCallback);
    Event::Connect<OnDeleteFontData>(DeletFontDataCallback);

    gui_spc = new GUIspec(window);
    GPWins.window_flags = gui_spc->window_flags;
    //initial gui windows parts.
    GPWins.fgws = ImVec2(get_width() / 1.4f, get_height()/ 1.42f);
}

void Main_Win::OnUpdate()
{
    //GP_Print(LoadList.loading_job.m_used);
	App_Window::OnUpdate();
    ImGuiMouseCursor current = ImGui::GetMouseCursor();
    gui_spc->GUI_OnFrameStart();
    ImGui::NewFrame();
    guiWindowsResizes(current);
	//-----------------------------------------------------------VV
    if (IsWindowVisible()) {
        {
            GUI_WINDOW window1("WINDOW LHS", ImVec2(0.0f, 0.0f), ImVec2(GPWins.fgws.x, GPWins.cws.y), Window_Resize_Dir::WRD_RIGHT);
            GPWins.cgws_LHS = ImGui::GetWindowSize();
            ImGui::SetCursorPos(ImVec2(8.0f, 43.0f));
            ImGui::SetNextItemWidth(70.0f);
            MainCanvesGUIWin();
            
            const char* loading_str = LoadList.loaded_fonts.empty() ? "Loading Font..." : LoadList.loaded_fonts.back()->get_font_name().c_str();
            if (LoadingGUI.ProcessLoadingLists(loading_str,LoadList.Total_Font_Load, LoadList.loading_job.is_working(), LoadList.loaded_fonts.size())) {
                Process_Glyph_Textures(true);
                LoadList.open_log = true;
            }
            char logmsg[254];
            sprintf(logmsg,"%i font files was successfully loaded \n%i font files failed to load",LoadList.Successful_laods,LoadList.Failed_Loads);
            LoadingGUI.Log("Log", &LoadList.open_log,logmsg, LoadList.TargetFontsNames);
        }

        {
            GUI_WINDOW window2("WINDOW RHS TOP", ImVec2(GPWins.fgws.x, 0), ImVec2(GPWins.fgws.x + (GPWins.cws.x - GPWins.fgws.x * 2), GPWins.fgws.y), Window_Resize_Dir::WRD_DOWN);
            ImGui::Dummy(ImVec2(0, 30));
            GPWins.cgws_RHS = ImGui::GetWindowSize();
            GlyphProcessPages();
        }

        {
            GUI_WINDOW window3("WINDOW RHS DOWN", GPWins.fgws, ImVec2(GPWins.fgws.x + (GPWins.cws.x - GPWins.fgws.x * 2), GPWins.fgws.y + (GPWins.cws.y - GPWins.fgws.y * 2)), Window_Resize_Dir::WRD_NONE);
            FontSelections();
        }

        {
            GUI_WINDOW windowMen("WINDOW MENU BAR", ImVec2(0, 0), ImVec2(GPWins.cws.x, 20));
            MenuBar();
        }

        Event::Notify_Once(OnDeleteFontData());
    }

#if defined Debug
    //ImGui::ShowDemoWindow();
#endif

    ImGui::Render();
    gui_spc->GUI_OnFrameEnd();

}


void Main_Win::OnInput()
{

}


void Main_Win::guiWindowsResizes(ImGuiMouseCursor current)
{
    if ((current == ImGuiMouseCursor_ResizeNS || current == ImGuiMouseCursor_ResizeEW || GPWins.resized) && GPWins.IsFocused) {
        float b = Clamp((GPWins.cws.x / GPWins.cgws_LHS.x), 1.1f, 3.2f);
        float a = Clamp((GPWins.cws.y / GPWins.cgws_RHS.y), 1.1f, 4.5f);
        GPWins.fgws = ImVec2(GPWins.cws.x / b, GPWins.cws.y / a);
        GPWins.resized = false;
    }
}

