#ifndef FONT_H
#define FONT_H

#include "asset/asset.h"
#include "renderer/texture.h"

#undef INFINITE
#include <msdf-atlas-gen.h>

struct MSDFData {
	std::vector<msdf_atlas::GlyphGeometry> glyphs;
	msdf_atlas::FontGeometry font_geometry;
};

class Font final : public Asset {
public:
	EVE_IMPL_ASSET(AssetType::FONT)

	Font(const std::string& path);
	Font(const uint8_t* bytes, uint32_t length);

	virtual ~Font() = default;

	const MSDFData& get_msdf_data() const;

	Ref<Texture2D> get_atlas_texture() const;

	static Ref<Font> get_default();

private:
	MSDFData msdf_data{};
	Ref<Texture2D> atlas_texture;

	static Ref<Font> s_default_font;

	friend class Application;
};

#endif
