#ifndef PROJECT_H
#define PROJECT_H

#include "asset/asset.h"

struct ProjectConfig {
	std::string name;
	fs::path asset_directory;
	std::string asset_registry;
	AssetHandle starting_scene;

	static void serialize(const ProjectConfig& config, const fs::path& path);

	static bool deserialize(ProjectConfig& config, const fs::path& path);
};

class Project {
public:
	Project(const fs::path& path, const ProjectConfig& config);

	static const std::string& get_name();

	static const fs::path& get_project_path();

	static fs::path get_project_directory();

	static fs::path get_asset_directory();

	static std::string get_asset_registry_path();

	static AssetHandle get_starting_scene_handle();

	static fs::path get_asset_path(std::string asset);

	static Ref<Project> create(const fs::path& path);

	static Ref<Project> load(const fs::path& path);

	static void save_active(const fs::path& path);

private:
	static Ref<Project> s_active_project;

	fs::path path;
	ProjectConfig config;
};

#endif
