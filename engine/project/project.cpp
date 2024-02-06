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

fs::path Project::get_asset_path(const std::string& path) {
	EVE_ASSERT_ENGINE(s_active_project);

	const std::string_view proj_substr = "prj://";
	const std::string_view res_substr = "res://";

	fs::path result_path;

	if (const auto pos = path.find(proj_substr); pos != std::string::npos) {
		const auto project_dir = get_project_directory();
		result_path = project_dir / std::string_view(path).substr(pos + proj_substr.length());
	} else if (const auto pos = path.find(res_substr); pos != std::string::npos) {
		const auto path_copy_dir = get_asset_directory();
		result_path = path_copy_dir / std::string_view(path).substr(pos + res_substr.length());
	} else {
		result_path = path;
	}

	return result_path;
}

std::string Project::get_relative_asset_path(const fs::path& path) {
	const auto project_dir = get_project_directory();
	const auto asset_dir = get_asset_directory();

	std::string path_string = path.string();

	fs::path relative_path;

	// Check if the path is inside the asset directory
	if (path_string.compare(0, asset_dir.string().length(), asset_dir.string()) == 0) {
		relative_path = fs::relative(path, asset_dir);
		return "res://" + relative_path.string();
	}

	// Check if the path is inside the project directory
	if (path_string.compare(0, project_dir.string().length(), project_dir.string()) ==
			0) {
		relative_path = fs::relative(path, project_dir);
		return "prj://" + relative_path.string();
	}

	// If the path is not inside the project or asset directory, return the original path
	return path_string;
}

Ref<Project> Project::create(const fs::path& path) {
	ProjectConfig empty_config{};
	Ref<Project> project = create_ref<Project>(path, empty_config);

	s_active_project = project;

	asset_registry::init();

	return project;
}

Ref<Project> Project::load(const fs::path& path) {
	ProjectConfig config{};
	if (!ProjectConfig::deserialize(config, path)) {
		EVE_LOG_ENGINE_ERROR("Unable to load project from: {}", path.string().c_str());
		return nullptr;
	}

	s_active_project = create_ref<Project>(path, config);

	asset_registry::init();

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
