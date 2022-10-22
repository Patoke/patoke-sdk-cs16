#pragma once
#include <precompiled.hpp>

// imgui framework and rendering wrapper
#include "../imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui_internal.h"

// imgui implementations
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_opengl3.h"

// this header contains most of the imgui wrappers such as the following:
//	1. render wrapper
//	2. function call wrappers
//	3. forward declare functions

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
struct s_imgui_wrapper {
	// helper functions
	void setup_imgui(HDC hdc) {
		// create internal imgui variables
		auto context = ImGui::CreateContext();
		ImGui::SetCurrentContext(context);

		// initialize imgui
		ImGui::StyleColorsDark();

		ImGui_ImplWin32_Init(WindowFromDC(hdc));
		ImGui_ImplOpenGL3_Init();

		// setup main imgui variables
		auto& io = ImGui::GetIO();

		io.IniFilename = nullptr;
		io.LogFilename = nullptr;
	}

	void create_frame() {
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
	}

	void end_frame() {
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	// variables
	bool did_initialize_imgui = false;
} inline imgui_wrapper_i;