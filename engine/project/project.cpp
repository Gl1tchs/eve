#include "project/project.h"

#include "asset/asset_registry.h"
#include "core/json_utils.h"
#include "project.h"

inline static Ref<Project> s_active_project = nullptr;

void ProjectConfig::serialize(const ProjectConfig& config, const fs::path& path) {
	Json out{
		{ "name", config.name },
		{ "asset_directory", config.asset_directory },
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
	config.starting_scene = json["starting_scene"].get<std::string>();

	return true;
}

Project::Project(const fs::path& path, const ProjectConfig& config) :
		path(path), config(config) {
}

Project::~Project() {
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

std::string Project::get_starting_scene_path() {
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

std::string Project::get_relative_asset_path(const std::string& path) {
	const auto project_dir = get_project_directory();
	const auto asset_dir = get_asset_directory();

	fs::path relative_path;

	// Check if the path is inside the asset directory
	if (path.compare(0, asset_dir.string().length(), asset_dir.string()) == 0) {
		relative_path = fs::relative(path, asset_dir);
		return "res://" + relative_path.string();
	}

	// Check if the path is inside the project directory
	if (path.compare(0, project_dir.string().length(), project_dir.string()) ==
			0) {
		relative_path = fs::relative(path, project_dir);
		return "prj://" + relative_path.string();
	}

	// If the path is not inside the project or asset directory, return the original path
	return path;
}

Ref<Project> Project::create(const fs::path& path) {
	ProjectConfig empty_config{};
	Ref<Project> project = create_ref<Project>(path, empty_config);

	s_active_project = project;

	AssetRegistry::init();

	return project;
}

Ref<Project> Project::load(const fs::path& path) {
	ProjectConfig config{};
	if (!ProjectConfig::deserialize(config, path)) {
		EVE_LOG_ENGINE_ERROR("Unable to load project from: {}", path.string().c_str());
		return nullptr;
	}

	s_active_project = create_ref<Project>(path, config);

	AssetRegistry::init();

	return s_active_project;
}

void Project::save_active(const fs::path& path) {
	EVE_ASSERT_ENGINE(s_active_project);

	const ProjectConfig& config = s_active_project->config;
	ProjectConfig::serialize(config, path);

	s_active_project->path = path;
}

Ref<Project> Project::get_active() {
	return s_active_project;
}
