#include "../hooks.hpp"

bool __stdcall n_hooked::hk_swap_buffers(HDC hdc) {
    static auto o_hook = n_detour::get<swap_buffers_fn>(hk_swap_buffers);

    auto game_window_dc = WindowFromDC(hdc);

    if (!render_i.did_initialize)
        ofn_wndproc = reinterpret_cast<WNDPROC>(SetWindowLong(game_window_dc, GWL_WNDPROC, reinterpret_cast<long>(hk_wndproc)));

    // initialize imgui
    render_i.setup(game_window_dc);

    render_i.new_frame();
    {
        esp_i.tracers();

        // do imgui stuff here
        ui_i.draw();
    }
    render_i.end_frame();

    return o_hook(hdc);
}