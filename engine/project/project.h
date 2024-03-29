#ifndef PROJECT_H
#define PROJECT_H

#include "asset/asset.h"

struct ProjectConfig {
	std::string name;
	std::string asset_directory;
	std::string script_dll;
	std::string starting_scene;

	static void serialize(const ProjectConfig& config, const fs::path& path);

	static bool deserialize(ProjectConfig& config, const fs::path& path);
};

class Project {
public:
	Project(const fs::path& path, const ProjectConfig& config);
	~Project();

	static const std::string& get_name();

	static const fs::path& get_project_path();

	static fs::path get_project_directory();

	static fs::path get_asset_directory();

	static fs::path get_cache_directory(AssetType type = AssetType::NONE);

	static std::string get_starting_scene_path();

	static fs::path get_script_dll_path();

	static fs::path get_asset_path(const std::string& path);

	static std::string get_relative_asset_path(const fs::path& path);

	static Ref<Project> create(const fs::path& path);

	static Ref<Project> load(const fs::path& path);

	static void save_active(const fs::path& path);

	static Ref<Project> get_active();

private:
	fs::path path;
	ProjectConfig config;

	friend class ProjectSettingsPanel;
};

#endif
