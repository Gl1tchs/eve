#ifndef ASSET_INSPECTOR_H
#define ASSET_INSPECTOR_H

#include "panels/panel.h"

#include "core/json_utils.h"

class AssetInspectorPanel : public Panel {
	EVE_IMPL_PANEL("Asset Inspector")

public:
	AssetInspectorPanel();

	void set_selected(const fs::path& path);

protected:
	void _draw() override;

private:
	fs::path asset_path;

	std::unordered_map<fs::path, Json> asset_cache_map;
};

#endif
