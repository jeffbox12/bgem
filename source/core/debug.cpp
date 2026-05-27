/*
 *  MIT License
 *  Copyright (c) 2026 Cătălin Gabriel Drăghiță
 */

#include "core/debug.h"
#include "core/debug_defaults.h"
#include "imgui.h"
#include "imgui_backends/imgui_impl_sdl3.h"
#include "imgui_backends/imgui_impl_opengl3.h"

static int s_active = 0;

void bgem_debug_init(SDL_Window *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = NULL;
    ImGui_ImplSDL3_InitForOpenGL(window, NULL);
    ImGui_ImplOpenGL3_Init("#version 300 es");
}

void bgem_debug_shutdown(void) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void bgem_debug_toggle(void)    { s_active = !s_active; }
int  bgem_debug_isActive(void)  { return s_active; }

void bgem_debug_processEvent(SDL_Event *event) {
    ImGui_ImplSDL3_ProcessEvent(event);
}

void bgem_debug_newFrame(void) {
    if (!s_active) return;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Debug");
    ImGui::Text("Input");
    ImGui::Checkbox("Enable input log on the console", &inputLog);
    ImGui::End();
}

void bgem_debug_render(void) {
    if (!s_active) return;
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
