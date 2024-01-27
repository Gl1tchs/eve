#include "imgui/imgui_layer.h"

#include "core/window.h"
#include "data/roboto_regular.h"

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <imgui_internal.h>

ImGuiLayer::ImGuiLayer(Ref<Window> window) :
		window(window) {
	// Setup ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	if (settings.enable_docking) {
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	if (settings.enable_viewports) {
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window->get_native_window(), true);
	ImGui_ImplOpenGL3_Init("#version 450");
}

ImGuiLayer::~ImGuiLayer() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiLayer::begin() {
	ImGuiIO& io = ImGui::GetIO();
	if (settings.enable_docking) {
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	} else {
		io.ConfigFlags &= ImGuiConfigFlags_DockingEnable;
	}

	if (settings.enable_viewports) {
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	} else {
		io.ConfigFlags &= ImGuiConfigFlags_ViewportsEnable;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiLayer::end() {
	ImGuiIO& io = ImGui::GetIO();
	auto size = window->get_size();
	io.DisplaySize = ImVec2((float)size.x, (float)size.y);

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}
