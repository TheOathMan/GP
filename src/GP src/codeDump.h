#pragma once

// main

//int fpathplay_w, fpathplay_h;
//glfwGetFramebufferSize(window, &fpathplay_w, &fpathplay_h);
//glViewport(0, 0, fpathplay_w, fpathplay_h);
// If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
// you may need to backup/reset/restore current shader using the commented lines below.
//GLint last_program;
//glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
//glUseProgram(0);

//------------------------------------------------------------------------------------
//------------------------------		CODE HERE		------------------------------
//------------------------------------------------------------------------------------

//ImGui::Begin("debug");
//
//static int firstchar = 0;
//static int charnums = 10;
//static float height = 32;
//static char inp[255] = "";
//static float Angle = 0;
//
//ImGui::InputText("debug", inp, sizeof(inp));
//ImGui::DragFloat("height", &height);
//
//if (ImGui::InputInt("first char", &firstchar, 20)) {
//    //GetCodePoints(&firstchar, charnums, height);
//}
//if (ImGui::DragFloat("char nums", &Angle, 0.2f)) {
//    //my_stbtt_initfont(&firstchar, charnums, height);
//    //Debug("worfd");
//}
//
//
//ImGui::End();


//ImGui::Button("butt", ImGui::GetWindowSize());


//KeyboardButtonReleased(advanceL, GLFW_KEY_A, {
//    AccFileWords(true);
// });

//KeyboardButtonReleased(cleann, GLFW_KEY_D, {

//   tfstr.clear();
//    });

//KeyboardButtonReleased(reset, GLFW_KEY_R, {
//   MyReadFile.seekg(oldpos);

//});

//KeyboardButtonReleased(screenShot, GLFW_KEY_S, {
//
//    });


//ImGui::SetCursorPos(ImVec2(draw_posX, draw_posY));
//ImTextureID my_tex_id = (ImTextureID)(intptr_t)image.Get_GPU_ID();
//ImGui::Image(my_tex_id, size);


//#define READ_BINARY_FILE(path,file_data)	           \
//FILE* file = std::fopen(path, "rb");				   \
//fseek(file, 0, SEEK_END);							   \
//size_t fsize = ftell(file);							   \
//rewind(file);										   \
//file_data= (binery_t*)malloc(fsize * sizeof(binery_t));\
//if (file_data)										   \
//	fread(file_data, 1, fsize, file);				   \
//fclose(file)										   

//pixel_t* FontBaking() {
//    stbtt_fontinfo font;
//    pixel_t* buffer = nullptr;
//    //READ_BINARY_FILE("fonts/l_10646.ttf", buffer);
//
//    stbtt_InitFont(&font, buffer, 0);
//    pixel_t* pixels = new pixel_t[500 * 500];
//    float fontSize = stbtt_ScaleForPixelHeight(&font, 100);
//
//    static stbtt_pack_context spc;
//    static stbtt_packedchar pc[100];
//    int result1 = stbtt_PackBegin(&spc, pixels, 500, 500, 500, 1, NULL);
//    int result2 = stbtt_PackFontRange(&spc, buffer, 0, fontSize, 0, 100, pc);
//    stbi_write_png("fonttest1.png", 500, 500, 1, pixels, 0);
//
//    stbtt_PackEnd(&spc);
//
//    return pixels;
//}
//
//
//
//int Bitmap_Pack_Ex()
//{
//    uint8_t* ttf_buffer = nullptr;
//    //READ_BINARY_FILE("fonts/UthmanTN1 Ver10.otf", ttf_buffer);
//    pixel_t* pixels = new pixel_t[1024 * 1224];
//    
//    static stbtt_pack_context pc;
//    static stbtt_packedchar cd[256];
//    static stbtt_packedchar cd2[5];
//#if 0
//    stbtt_PackBegin(&pc, pixels, 1024, 1224, 1024, 1, NULL);
//    stbtt_PackFontRange(&pc, ttf_buffer, 0, 88.0f, 0, 256, cd);
//    //stbtt_PackFontRange(&pc, ttf_buffer, 0, 88.0f, 256, 5, cd2);
//    stbtt_PackEnd(&pc);
//    stbi_write_png("fonttest1.png", 1024, 1224, 1, pixels, 0);
//#else
//    {
//
//        //auto allcp = GetAllCodepoints("fonts/UthmanTN1 Ver10.otf");
//
//        static stbtt_pack_range pr[2];
//
//        pr[0].chardata_for_range = cd;
//        pr[0].first_unicode_codepoint_in_range = 0;
//        pr[0].num_chars = 256;
//        pr[0].font_size = 88.0f;
//
//        pr[1].chardata_for_range = cd2;
//        pr[1].first_unicode_codepoint_in_range = 256;
//        pr[1].num_chars = 5;
//        pr[1].font_size = 88.0f;
//
//        stbtt_PackBegin(&pc, pixels, 1024, 1224, 1024, 1, NULL);
//        //stbtt_PackSetOversampling(&pc, 2, 2);
//        stbtt_PackFontRanges(&pc, ttf_buffer, 0, pr, 2);
//        stbtt_PackEnd(&pc);
//        stbi_write_png("fonttest2.png", 1024, 1224, 1, pixels, 0);
//    }
//#endif
//    return 0;
//}


//static stbtt_pack_context pc;
//static stbtt_packedchar cd[256];
//static unsigned char atlas[1024 * 1024];
//
//stbtt_PackBegin(&pc, atlas, 1024, 1024, 1024, 1, NULL);
//stbtt_PackSetOversampling(&pc, 4, 2);
//stbtt_PackFontRange(&pc, get_font_binery_dataB(), 0, 100.0, 0, 256, cd);
//stbtt_PackEnd(&pc);
//Image im(atlas, 1024, 1024, 1);
//SaveImage<Image_Format::PNG>("atles.png", im);



//
//#define KeyboardButtonReleased(ButtonName,key,Method)  \
//static char ButtonName = 0; 						   \
//int state##ButtonName = glfwGetKey(window, key);	   \
//if (state##ButtonName == GLFW_PRESS) {				   \
//	ButtonName = 1;									   \
//}													   \
//else {												   \
//	if (ButtonName)									   \
//	{												   \
//		Method;										   \
//		ButtonName = 0;								   \
//	}												   \
//}



//for (size_t i = 0; i < glf.ver_size; i++)
//{
//    auto is_empty_p = [](stbtt_vertex v) { return v.x == 0 && v.y == 0; };
//    auto is_empty_cp = [](stbtt_vertex v) { return v.cx == 0 && v.cy == 0; };
//    auto is_empty_cp1 = [](stbtt_vertex v) { return v.cx1 == 0 && v.cy1 == 0; };
//
//    stbtt_vertex& cur_v = glf.verts[i];
//    stbtt_vertex& nex_v = (i < glf.ver_size - 1) ? glf.verts[i + 1] : glf.verts[i];
//    if (nex_v.x == same_prev.x && nex_v.y == same_prev.y && i != con_ind) {
//        i++;
//        con_ind = i;
//        same_prev = glf.verts[i + 1];
//    }
//
//    // draw verts position
//    if (!is_empty_p(cur_v))
//        DrawList->AddCircleFilled(sapce(ImVec2(cur_v.x, cur_v.y)), 5.0f, IM_COL32(255, 255, 255, 200), 50);
//    if (!is_empty_cp(cur_v))
//        DrawList->AddCircleFilled(sapce(ImVec2(cur_v.cx, cur_v.cy)), 5.0f, IM_COL32(255, 0, 0, 200), 50);
//
//    if (!is_empty_p(cur_v))
//        draw_list->AddBezierCurve(sapce(ImVec2(cur_v.x, cur_v.y)),
//            sapce(is_empty_cp(nex_v) ? ImVec2(nex_v.x, nex_v.y) : ImVec2(nex_v.cx, nex_v.cy)),
//            sapce(is_empty_cp1(nex_v) ? ImVec2(nex_v.x, nex_v.y) : ImVec2(nex_v.cx1, nex_v.cy1)),
//            sapce(ImVec2(nex_v.x, nex_v.y)),
//            IM_COL32(255, 155, 100, 200), 0.5f, 10);
//
//
//    ImVec2 vv = sapce(ImVec2(cur_v.x, cur_v.y));
//    Vec2 vpos(vv.x, vv.y);
//    Vec2 cvpos(cur_v.cx, cur_v.cy);
//    Vec2 mpos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
//
//    if ((vpos - mpos).length() < 4.5f) {
//        ImGui::SetCursorPos(ImVec2(mpos.x - 15, mpos.y - 25.0f));
//        ImGui::PushFont(gui_spc->_main_font_Big);
//        ImGui::Text("(%i, %i)", cur_v.x, cur_v.y);
//        ImGui::PopFont();
//    }
//
//}


//    for (int i = 0; i < wsl; i++)
//    {

//        int _y = Repeat(i + sc.mouse_drag.y, ws.cgws_LHS.y);
//        //int u_y = i + sc.mouse_drag.y;
//        int _x = Repeat(i + sc.mouse_drag.x, ws.cgws_LHS.x);
//        //int u_x = i + sc.mouse_drag.x;
//        //int _y = i;
//        //int _x = i + f;

//        int limit = 20; //limit = limit == 0 ? 10 : limit;

//        if ( fmodf(i , limit) == 0.0f && i < ws.cgws_LHS.y) {
//            DrawList->AddLine(ImVec2(0.0f, _y), ImVec2(ws.cgws_LHS.x, _y), IM_COL32(255, 255, 255, 50), 0.25f); //modifies raws ( X )
//            //ImGui::SetCursorPos(ImVec2(5.0f, _y - 10.0f));
//            //ImGui::Text("%i", u_x);
//        }

//        if (fmodf(i ,limit) == 0.0f && i < ws.cgws_LHS.x) {
//            DrawList->AddLine(ImVec2(_x, 0.0f), ImVec2(_x, ws.cgws_LHS.y), IM_COL32(255, 255, 255, 50), 0.25f); //modifies colums ( Y )
//            //ImGui::SetCursorPos(ImVec2(_x, 0.0f + 30));
//            //ImGui::Text("%i", u_y);
//        }

//    }     