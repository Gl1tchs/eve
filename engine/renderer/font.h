#ifndef FONT_H
#define FONT_H

#include "renderer/texture.h"

#undef INFINITE
#include <msdf-atlas-gen.h>

struct MSDFData {
	std::vector<msdf_atlas::GlyphGeometry> glyphs;
	msdf_atlas::FontGeometry font_geometry;
};

class Font {
public:
	Font(const std::string& path);
	Font(const uint8_t* bytes, uint32_t length);

	~Font();

	const MSDFData& get_msdf_data() const;

	Ref<Texture2D> get_atlas_texture() const;

	static Ref<Font> get_default();

private:
	MSDFData msdf_data{};
	Ref<Texture2D> atlas_texture;
};

#endif
