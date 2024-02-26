#ifndef CONTENT_BROWSER_H
#define CONTENT_BROWSER_H

#include "asset/asset.h"
#include "core/json_utils.h"
#include "scene/scene.h"

#include "panels/panel.h"

class ContentBrowserPanel : public Panel {
	EVE_IMPL_PANEL("Content Browser");

public:
	ContentBrowserPanel();
	virtual ~ContentBrowserPanel();

protected:
	void _draw() override;

private:
	struct AssetData {
		AssetHandle handle;
		AssetType type;
	};

	void _draw_file(const fs::path& path);

	void _draw_asset_modal(
			Ref<Scene> scene, AssetHandle handle, const fs::path& path);

	void _draw_rename_file_dialog(const fs::path& path);

	void _draw_popup_context(const fs::path& path);

	bool _is_asset_file(const fs::path& path);

private:
	int idx = 0;
	int selected_idx = -1;
	int renaming_idx = -1;

	std::unordered_map<fs::path, Json> asset_cache_map;
};

#endif
