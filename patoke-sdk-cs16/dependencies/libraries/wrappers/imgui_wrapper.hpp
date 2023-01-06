#pragma once
// imgui_wrapper.hpp
// contains all the renderer wrappers for the framework
// if it is needed to change to another renderer change these functions

// configs
// todo: add different backend support
#define IMGUI_BACKEND   1
#define SURFACE_BACKEND 0

// render types
// todo: add more of these
#define DX9_IMPL     0
#define OPENGL2_IMPL 0
#define OPENGL3_IMPL 1

// include renderer headers
#include "../imgui/imgui.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
//#include "../imgui/imgui_freetype.h"
#include "../imgui/imgui_internal.h"

// include renderer implementations
#include "../imgui/imgui_impl_win32.h"
#if DX9_IMPL
#include "../imgui/imgui_impl_dx9.h"

#define IMPL_INIT(device)             ImGui_ImplDX9_Init(device)
#define IMPL_NEWFRAME()               ImGui_ImplDX9_NewFrame()
#define IMPL_RENDERDRAWDATA(drawdata) ImGui_ImplDX9_RenderDrawData(drawdata)
#endif

#if OPENGL2_IMPL
#include "../imgui/imgui_impl_opengl2.h"

#define IMPL_INIT(device)             ImGui_ImplOpenGL2_Init()
#define IMPL_NEWFRAME()               ImGui_ImplOpenGL2_NewFrame()
#define IMPL_RENDERDRAWDATA(drawdata) ImGui_ImplOpenGL2_RenderDrawData(drawdata)
#endif

#if OPENGL3_IMPL
#include "../imgui/imgui_impl_opengl3.h"

#define IMPL_INIT(device)             ImGui_ImplOpenGL3_Init()
#define IMPL_NEWFRAME()               ImGui_ImplOpenGL3_NewFrame()
#define IMPL_RENDERDRAWDATA(drawdata) ImGui_ImplOpenGL3_RenderDrawData(drawdata)
#endif

// rendering utilities
typedef ImColor s_color;
typedef ImDrawFlags e_flags; // flags (if they exist)
typedef ImFont* s_font;
typedef ImDrawList* s_drawlist;
typedef ImDrawVert* s_vertex;

enum e_text_flags : int {
    none = (1 << 0),
    drop_shadow = (1 << 1),
    outline = (1 << 2),
    text_flags_max = (1 << 3)
};

enum e_gradient_type : char {
    horizontal = 0,
    vertical = 1,
    all_sides = 2
};

// graph stuff
struct s_graph {
    s_graph(float fade_factor, int max_items, int height, int width) : fade_factor(fade_factor), max_items(max_items), height(height), width(width) {}

    std::vector<int> values{};
    std::vector<s_vec2> vertices{};
    float fade_factor{};
    size_t max_items{};
    int height{};
    int width{};
};

inline std::map<unsigned int, s_graph> graphs;

struct s_render {
    // variables necessary for initialiation in csgo
    bool did_initialize = false;
#if DX9_IMPL
    IDirect3DStateBlock9* pixel_state = NULL;
    IDirect3DVertexDeclaration9* vertex_declaration;
    IDirect3DVertexShader9* vertex_shader;
    DWORD old_write_color_enable;
#endif

#if OPENGL2_IMPL
    HGLRC o_wgl_ctx;
    HGLRC wgl_ctx;
    HDC curr_hdc;
#endif

    // drawlist (if it exists)
    s_drawlist drawlist{};

#if DX9_IMPL
    void setup(void* window, IDirect3DDevice9* device) {
#elif OPENGL2_IMPL
    void setup(void* window, HDC hdc) {
#else
    void setup(void* window) {
#endif
        if (!did_initialize) {
#if OPENGL2_IMPL
            curr_hdc = hdc;

            // create context for gl2
            wgl_ctx = wglCreateContext(curr_hdc);
#endif

            // create internal imgui variables
            auto context = ImGui::CreateContext();
            ImGui::SetCurrentContext(context);

            // initialize imgui
            ImGui::StyleColorsDark();

            ImGui_ImplWin32_Init(window);

            IMPL_INIT(device);

            // got banned off of a fucking server for this
            ImGui::GetIO().IniFilename = NULL;

            did_initialize = true;
        }
    }

#if DX9_IMPL
    void set_state(IDirect3DDevice9 * device) {
        device->GetRenderState(D3DRS_COLORWRITEENABLE, &old_write_color_enable);
        device->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state);
        // fix shadows
        pixel_state->Capture();
        device->GetVertexDeclaration(&vertex_declaration);
        device->GetVertexShader(&vertex_shader);
        device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
        device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
        device->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        device->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        device->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
        device->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);
    }

    void restore_state(IDirect3DDevice9 * device) {
        device->SetRenderState(D3DRS_COLORWRITEENABLE, old_write_color_enable);
        device->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
        pixel_state->Apply();
        pixel_state->Release();
        device->SetVertexDeclaration(vertex_declaration);
        device->SetVertexShader(vertex_shader);
    }

    strong_inline void on_device_lost() {
        ImGui_ImplDX9_InvalidateDeviceObjects();
    }

    strong_inline void on_device_reset() {
        ImGui_ImplDX9_CreateDeviceObjects();
    }
#endif

    strong_inline void new_frame() {
#if OPENGL2_IMPL
        o_wgl_ctx = wglGetCurrentContext();
        wglMakeCurrent(curr_hdc, wgl_ctx);
#endif

        IMPL_NEWFRAME();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        drawlist = ImGui::GetBackgroundDrawList();
    }

    strong_inline void end_frame() {
        ImGui::EndFrame();
        ImGui::Render();
        IMPL_RENDERDRAWDATA(ImGui::GetDrawData());

#if OPENGL2_IMPL
        wglMakeCurrent(curr_hdc, o_wgl_ctx);
#endif
    }

    inline s_color lerp_color(s_color from, s_color to, float t) {
        return s_color(std::lerp(from.Value.x, to.Value.x, t), std::lerp(from.Value.y, to.Value.y, t), std::lerp(from.Value.z, to.Value.z, t),
            std::lerp(from.Value.w, to.Value.w, t));
    }

    void graph_update(unsigned int graph_id, s_vec2 pos, s_graph & graph_data, int value) {
        auto graph_state = graphs.find({ graph_id });
        if (graph_state == graphs.end()) {
            graphs.insert({ graph_id, graph_data });
            graph_state = graphs.find({ graph_id });
        }

        // push new data
        graph_state->second.values.push_back(value);

        // delete data that is outside of our max item range
        if (graph_state->second.values.size() > graph_state->second.max_items) {
            // remove latest vertex added
            // graph_state->second.vertices.pop_back();

            // remove last value added
            graph_state->second.values.erase(graph_state->second.values.begin());
            graph_state->second.values.pop_back();
        }

        // setup rendering shart
        for (size_t idx = 1; idx < graph_state->second.values.size(); idx++) {
            float width = static_cast<float>(graph_state->second.width) / static_cast<float>(graph_state->second.max_items);

            float height = static_cast<float>(graph_state->second.values.at(idx));
            float last_height = static_cast<float>(graph_state->second.values.at(idx - 1));

            s_vec2 curr_pos = pos + s_vec2(width * idx, -height + graph_state->second.height);
            s_vec2 last_pos = pos + s_vec2(width * (idx - 1), -last_height + graph_state->second.height);

            float to_fade = min(graph_state->second.fade_factor, 0.5f);
            float alpha_value_first = static_cast<float>(idx) / (graph_state->second.max_items * to_fade);
            float alpha_value_second = (graph_state->second.max_items - static_cast<float>(idx)) / (graph_state->second.max_items * to_fade);
            int to_color = static_cast<int>(idx < graph_state->second.max_items* to_fade ? alpha_value_first * 255.f : alpha_value_second * 255.f);

            this->line(last_pos, curr_pos, s_color(255, 255, 255, to_color));
            // graph_state->second.vertices.push_back(curr_pos);
        }

        // this->polygon(graph_state->second.vertices.data(), graph_state->second.vertices.size() - 1, s_color(1.f, 1.f, 1.f));
        this->rect(pos, s_vec2(graph_state->second.width, graph_state->second.height), s_color(1.f, 0.f, 0.f));
    }

    void draw_graphs() {
        // for (auto& graph : graphs)
        //	this->polygon(graph.second.vertices.data(), graph.second.vertices.size() - 1, s_color(1.f, 1.f, 1.f));
    }

    void text(const char* text, s_vec2 pos, s_color color, s_font font, s_vec2 align = s_vec2(), e_text_flags flags = e_text_flags::none) {
        s_vec2 aligned_pos = pos;
        float font_size = font->FontSize;
        s_vec2 text_size = font->CalcTextSizeA(font_size, FLT_MAX, 0.f, text);

        aligned_pos -= text_size * align;

        if (flags & e_text_flags::drop_shadow)
            drawlist->AddText(font, font_size, aligned_pos + s_vec2(1, 1), s_color(0.f, 0.f, 0.f, color.Value.w), text);

        if (flags & e_text_flags::outline) {
            // corners
            drawlist->AddText(font, font_size, aligned_pos - s_vec2(1, 1), s_color(0.f, 0.f, 0.f, color.Value.w), text);
            drawlist->AddText(font, font_size, aligned_pos + s_vec2(1, -1), s_color(0.f, 0.f, 0.f, color.Value.w), text);
            drawlist->AddText(font, font_size, aligned_pos + s_vec2(-1, 1), s_color(0.f, 0.f, 0.f, color.Value.w), text);
            drawlist->AddText(font, font_size, aligned_pos + s_vec2(1, 1), s_color(0.f, 0.f, 0.f, color.Value.w), text);

            // sides
            drawlist->AddText(font, font_size, aligned_pos - s_vec2(1, 0), s_color(0.f, 0.f, 0.f, color.Value.w), text);
            drawlist->AddText(font, font_size, aligned_pos - s_vec2(0, 1), s_color(0.f, 0.f, 0.f, color.Value.w), text);
            drawlist->AddText(font, font_size, aligned_pos + s_vec2(1, 0), s_color(0.f, 0.f, 0.f, color.Value.w), text);
            drawlist->AddText(font, font_size, aligned_pos + s_vec2(0, 1), s_color(0.f, 0.f, 0.f, color.Value.w), text);
        }

        drawlist->AddText(font, font_size, aligned_pos, color, text);
    }

    s_vec2 get_text_size(const char* text, s_font font) {
        return font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.f, text);
    }

    void line(s_vec2 point1, s_vec2 point2, s_color color) {
        drawlist->AddLine(point1, point2, color);
    }

    void rect(s_vec2 pos, s_vec2 size, s_color color, float rounding = 0.f, e_flags flags = 0) {
        drawlist->AddRect(pos, pos + size, color, rounding, flags);
    }

    void rect_filled(s_vec2 pos, s_vec2 size, s_color color, float rounding = 0.f, e_flags flags = 0) {
        drawlist->AddRectFilled(pos, pos + size, color, rounding, flags);
    }

    void line(int from_x, int from_y, int to_x, int to_y, s_color color) {
        line({ float(from_x), float(from_y) }, { float(to_x), float(to_y) }, color);
    }

    void rect(int from_x, int from_y, int to_x, int to_y, s_color color, float rounding = 0.f, e_flags flags = 0) {
        rect({ float(from_x), float(from_y) }, { float(to_x), float(to_y) }, color, rounding, flags);
    }

    void rect_filled(int from_x, int from_y, int to_x, int to_y, s_color color, float rounding = 0.f) {
        rect_filled({ float(from_x), float(from_y) }, { float(to_x), float(to_y) }, color, rounding);
    }

    void rect_gradient(s_vec2 pos, s_vec2 size, e_gradient_type type, s_color color1, s_color color2, s_color optional3 = s_color(),
        s_color optional4 = s_color(), float rounding = 0.f, float color_stop = 0.5f, e_flags flags = 0) {

        // todo: rounded gradients for now are imgui only, add for other renderers too

        auto shade_verts_linear_color_gradient = [&](int vert_start_idx, int vert_end_idx, s_vec2 start_pos, s_vec2 to_size, s_color col0,
            s_color col1) {
                constexpr int r_shift = 0;
                constexpr int g_shift = 8;
                constexpr int b_shift = 16;
                constexpr int a_shift = 24;

                s_vec2 gradient_extent = to_size - start_pos;
                float gradient_inv_length2 = 1.0f / ImLengthSqr(gradient_extent);
                s_vertex vert_start = drawlist->VtxBuffer.Data + vert_start_idx;
                s_vertex vert_end = drawlist->VtxBuffer.Data + vert_end_idx;
                const int col0_r = (int)(col0 >> r_shift) & 0xFF;
                const int col0_g = (int)(col0 >> g_shift) & 0xFF;
                const int col0_b = (int)(col0 >> b_shift) & 0xFF;
                const int col0_a = (int)(col0 >> a_shift) & 0xFF;
                const int col_delta_r = ((int)(col1 >> r_shift) & 0xFF) - col0_r;
                const int col_delta_g = ((int)(col1 >> g_shift) & 0xFF) - col0_g;
                const int col_delta_b = ((int)(col1 >> b_shift) & 0xFF) - col0_b;
                const int col_delta_a = ((int)(col1 >> a_shift) & 0xFF) - col0_a;

                for (s_vertex vert = vert_start; vert < vert_end; vert++) {
                    float d = ImDot(vert->pos - start_pos, gradient_extent);
                    float t = ImClamp(d * gradient_inv_length2, 0.0f, 1.0f);
                    int r = (int)(col0_r + col_delta_r * t);
                    int g = (int)(col0_g + col_delta_g * t);
                    int b = (int)(col0_b + col_delta_b * t);
                    int a = (int)(col0_a + col_delta_a * t);
                    vert->col = (r << r_shift) | (g << g_shift) | (b << b_shift) | (a << a_shift);
                }
        };

        // optimize and don't do shading if not needed
        if (rounding == 0.f) {
            switch (type) {
            case e_gradient_type::horizontal:
                drawlist->AddRectFilledMultiColor(pos, pos + size, color1, color2, color2, color1);
                break;
            case e_gradient_type::vertical:
                drawlist->AddRectFilledMultiColor(pos, pos + size, color1, color1, color2, color2);
                break;
            case e_gradient_type::all_sides:
                drawlist->AddRectFilledMultiColor(pos, pos + size, color1, color2, optional3, optional4);
                break;
            }

            return; // stop execution
        }

        const int vtx_idx_0 = drawlist->VtxBuffer.Size;
        drawlist->AddRectFilled(pos, pos + size, s_color(1.f, 1.f, 1.f, 1.f), rounding, flags); // draw a rect to then shade
        const int vtx_idx_1 = drawlist->VtxBuffer.Size;

        // todo: fix color stop pls
        s_vec2 stop = { pos + s_vec2(size * color_stop) };

        switch (type) {
        case e_gradient_type::horizontal:
            shade_verts_linear_color_gradient(vtx_idx_0, vtx_idx_1, { pos.x, pos.y + stop.y }, { pos.x + size.x, pos.y + stop.y }, color1, color2);
            break;
        case e_gradient_type::vertical:
            shade_verts_linear_color_gradient(vtx_idx_0, vtx_idx_1, { pos.x + stop.x, pos.y }, { pos.x + stop.x, pos.y + size.y }, color1, color2);
            break;
            // todo: add all sides rounded gradients
        }
    }

    void polygon(s_vec2 * positions, int points, s_color color) {
        drawlist->AddPolyline((const ImVec2*)positions, points, color, 0, 1.f);
    }

    void polygon_filled(s_vec2 * positions, int points, s_color color) {
        drawlist->AddConvexPolyFilled((const ImVec2*)positions, points, color);
    }

    void push_clip(s_vec2 pos, s_vec2 size) {
        drawlist->PushClipRect(pos, pos + size);
    }

    void pop_clip() {
        drawlist->PopClipRect();
    }
    };

// extern declaration
extern s_render render_i;