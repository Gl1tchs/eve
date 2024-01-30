#ifndef CONTENT_BROWSER_H
#define CONTENT_BROWSER_H

#include "panels/panel.h"

#include "asset/asset.h"

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

	void _refresh_asset_tree();

	void _draw_rename_file_dialog(const fs::path& path);

	void _draw_popup_context(const fs::path& path);

	bool _is_asset(const fs::path& path) const;

private:
	std::unordered_map<fs::path, AssetData> asset_paths;

	int idx = 0;
	int selected_idx = -1;
	int renaming_idx = -1;
};

#endif
