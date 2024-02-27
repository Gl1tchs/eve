#ifndef CONTENT_BROWSER_H
#define CONTENT_BROWSER_H

#include "asset/asset.h"

#include "panels/panel.h"

class Scene;

class ContentBrowserPanel : public Panel {
	EVE_IMPL_PANEL("Content Browser");

public:
	ContentBrowserPanel();
	virtual ~ContentBrowserPanel();

	const fs::path& get_selected() const;

protected:
	void _draw() override;

private:
	struct AssetData {
		AssetHandle handle;
		AssetType type;
	};

	void _draw_file(const fs::path& path);

	void _draw_asset(Ref<Scene> scene, const fs::path& path);

	void _draw_folder(const fs::path& path);

	void _draw_rename_file_dialog(const fs::path& path);

	void _draw_popup_context(const fs::path& path);

	bool _is_asset_file(const fs::path& path);

private:
	fs::path selected_path;

	int idx = 0;
	int selected_idx = -1;
	int renaming_idx = -1;
};

#endif
