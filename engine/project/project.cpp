#include "project/project.h"

#include "asset/asset_registry.h"
#include "core/json_utils.h"

inline static Ref<Project> s_active_project = nullptr;

void ProjectConfig::serialize(const ProjectConfig& config, const fs::path& path) {
	Json out{
		{ "name", config.name },
		{ "asset_directory", config.asset_directory },
		{ "asset_registry", config.asset_registry },
		{ "starting_scene", config.starting_scene },
	};

	json_utils::write_file(path, out);
}

bool ProjectConfig::deserialize(ProjectConfig& config, const fs::path& path) {
	Json json;
	if (!json_utils::read_file(path, json)) {
		EVE_LOG_ENGINE_ERROR("Failed to load project file from: {}", path.string().c_str());
		return false;
	}

	config.name = json["name"].get<std::string>();
	config.asset_directory = json["asset_directory"].get<std::string>();
	config.asset_registry = json["asset_registry"].get<std::string>();
	config.starting_scene = json["starting_scene"].get<AssetHandle>();

	return true;
}

Project::Project(const fs::path& path, const ProjectConfig& config) :
		path(path), config(config) {
}

const std::string& Project::get_name() {
	EVE_ASSERT_ENGINE(s_active_project);

	return s_active_project->config.name;
}

const fs::path& Project::get_project_path() {
	EVE_ASSERT_ENGINE(s_active_project);

	return s_active_project->path;
}

fs::path Project::get_project_directory() {
	EVE_ASSERT_ENGINE(s_active_project);

	return s_active_project->path.parent_path();
}

fs::path Project::get_asset_directory() {
	EVE_ASSERT_ENGINE(s_active_project);

	return get_project_directory() / s_active_project->config.asset_directory;
}

fs::path Project::get_asset_registry_path() {
	EVE_ASSERT_ENGINE(s_active_project);

	return get_asset_path(s_active_project->config.asset_registry);
}

AssetHandle Project::get_starting_scene_handle() {
	EVE_ASSERT_ENGINE(s_active_project);

	return s_active_project->config.starting_scene;
}

fs::path Project::get_asset_path(std::string asset) {
	EVE_ASSERT_ENGINE(s_active_project);

	const std::string proj_substr = "prj://";
	const std::string res_substr = "res://";

	if (const auto pos = asset.find(proj_substr); pos != std::string::npos) {
		const auto project_dir = get_project_directory();

		asset.erase(pos, proj_substr.length());

		return project_dir / asset;
	} else if (const auto pos = asset.find(res_substr);
			   pos != std::string::npos) {
		const auto asset_dir = get_asset_directory();

		asset.erase(pos, res_substr.length());

		return asset_dir / asset;
	}

	return asset;
}

Ref<Project> Project::create(const fs::path& path) {
	ProjectConfig empty_config{};
	Ref<Project> project = create_ref<Project>(path, empty_config);

	s_active_project = project;

	return project;
}

Ref<Project> Project::load(const fs::path& path) {
	ProjectConfig config{};
	if (!ProjectConfig::deserialize(config, path)) {
		EVE_LOG_ENGINE_ERROR("Unable to load project from: {}", path.string().c_str());
		return nullptr;
	}

	s_active_project = create_ref<Project>(path, config);

	// deserialize asset registry
	AssetRegistry::deserialize(get_asset_registry_path());

	return s_active_project;
}

void Project::save_active(const fs::path& path) {
	EVE_ASSERT_ENGINE(s_active_project);

	const ProjectConfig& config = s_active_project->config;
	ProjectConfig::serialize(config, path);

	s_active_project->path = path;

	// serialize asset registry
	AssetRegistry::serialize(get_asset_registry_path());
}
