#include "panels/stats_panel.h"

#include "renderer/renderer.h"

#include <imgui.h>

StatsPanel::StatsPanel(const Ref<Renderer>& _renderer) :
		renderer(_renderer) {
}

void StatsPanel::_draw() {
	ImGui::Columns(2, nullptr, false);

	if (ImGui::TreeNodeEx("Render Stats", ImGuiTreeNodeFlags_DefaultOpen)) {
		RendererStats stats = renderer->get_stats();

		ImGui::NextColumn();
		ImGui::NextColumn();

		ImGui::TextUnformatted("Quad Count");
		ImGui::NextColumn();
		ImGui::InputScalar("##QuadCount", ImGuiDataType_U32, &stats.quad_count, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);

		ImGui::NextColumn();

		ImGui::TextUnformatted("Vertex Count");
		ImGui::NextColumn();
		ImGui::InputScalar("##VertexCount", ImGuiDataType_U32, &stats.vertex_count, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);

		ImGui::NextColumn();

		ImGui::TextUnformatted("Index Count");
		ImGui::NextColumn();
		ImGui::InputScalar("##IndexCount", ImGuiDataType_U32, &stats.index_count, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);

		ImGui::NextColumn();

		ImGui::TextUnformatted("Draw Calls");
		ImGui::NextColumn();
		ImGui::InputScalar("##DrawCalls", ImGuiDataType_U32, &stats.draw_calls, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);

		ImGui::NextColumn();

		ImGui::TreePop();
	}

	ImGui::Columns();
}
