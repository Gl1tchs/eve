#include "renderer/font.h"

#include "data/fonts/roboto_regular.h"
#include "renderer/texture.h"

#include <FontGeometry.h>
#include <GlyphGeometry.h>

Ref<Font> Font::s_default_font = nullptr;

inline static uint32_t s_memory_counter = 1;

inline static Ref<Texture2D> create_and_cache_atlas(const std::string& font_name, float font_size,
		const std::vector<msdf_atlas::GlyphGeometry>& glyphs, const msdf_atlas::FontGeometry& font_geometry,
		uint32_t width, uint32_t height) {
	msdf_atlas::GeneratorAttributes attributes;
	attributes.config.overlapSupport = true;
	attributes.scanlinePass = true;

	msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<uint8_t, 3>>
			generator(width, height);

	auto thread_count = std::thread::hardware_concurrency();
	if (thread_count % 2 == 0) {
		thread_count /= 2;
	} else if (thread_count % 2 != 0 && thread_count != 1) {
		thread_count = (thread_count + 1) / 2;
	}

	generator.setAttributes(attributes);
	generator.setThreadCount(thread_count);
	generator.generate(glyphs.data(), (int)glyphs.size());

	msdfgen::BitmapConstRef<uint8_t, 3> bitmap = (msdfgen::BitmapConstRef<uint8_t, 3>)generator.atlasStorage();

	// TODO maybe cache with a file
	// msdfgen::savePng(bitmap, "myfont.png");

	TextureMetadata metadata;
	metadata.format = TextureFormat::RGB;
	metadata.generate_mipmaps = false;

	Ref<Texture2D> texture = create_ref<Texture2D>(metadata, (void*)bitmap.pixels, glm::ivec2{ bitmap.width, bitmap.height });
	return texture;
}

inline static Ref<Texture2D> create_texture_atlas(msdfgen::FontHandle* font,
		MSDFData* data, const std::string& name) {
	struct CharsetRange {
		uint32_t begin, end;
	};

	static const CharsetRange charset_ranges[] = {
		{ 0x0020, 0x00FF }
	};

	msdf_atlas::Charset charset;
	for (CharsetRange range : charset_ranges) {
		for (uint32_t c = range.begin; c <= range.end; c++) {
			charset.add(c);
		}
	}

	double font_scale = 1.0;
	data->font_geometry = msdf_atlas::FontGeometry(&data->glyphs);
	int glyphs_loaded = data->font_geometry.loadCharset(font, font_scale, charset);

#if EVE_DEBUG
	EVE_LOG_ENGINE_INFO("Loaded {} glyphs from font (out of {})", glyphs_loaded, charset.size());
#endif

	// FIXME
	//	If camera zooms out this is the reason it seems bad
	double em_size = 48.0;

	msdf_atlas::TightAtlasPacker atlas_packer;
	atlas_packer.setPixelRange(2.0);
	atlas_packer.setMiterLimit(1.0);
	atlas_packer.setPadding(0);
	atlas_packer.setScale(em_size);

	int remaining = atlas_packer.pack(data->glyphs.data(), (int)data->glyphs.size());
	EVE_ASSERT_ENGINE(remaining == 0);

	int width, height;
	atlas_packer.getDimensions(width, height);
	em_size = atlas_packer.getScale();

	constexpr double DEFAULT_ANGLE_THRESHOLD = 3.0;
	constexpr uint64_t LCG_MULTIPLIER = 6364136223846793005ull;

	uint64_t glyph_seed = 0;
	for (msdf_atlas::GlyphGeometry& glyph : data->glyphs) {
		glyph_seed *= LCG_MULTIPLIER;
		glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyph_seed);
	}

	return create_and_cache_atlas(
			name, (float)em_size, data->glyphs, data->font_geometry, width, height);
}

Font::Font(const fs::path& path) {
	msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();

	const std::string path_str = path.string();

	msdfgen::FontHandle* font = msdfgen::loadFont(ft, path_str.c_str());
	if (!font) {
		EVE_LOG_ENGINE_ERROR("Failed to load font: {}", path_str.c_str());
		return;
	}

	atlas_texture = create_texture_atlas(font, &msdf_data, path_str);

	msdfgen::destroyFont(font);
	msdfgen::deinitializeFreetype(ft);
}

Font::Font(const uint8_t* bytes, uint32_t length) {
	msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();

	msdfgen::FontHandle* font = msdfgen::loadFontData(ft, bytes, length);
	if (!font) {
		EVE_LOG_ENGINE_ERROR("Failed to load memory font!");
		return;
	}

	atlas_texture = create_texture_atlas(font, &msdf_data, "memory_font_" + std::to_string(s_memory_counter++));

	msdfgen::destroyFont(font);
	msdfgen::deinitializeFreetype(ft);
}

const MSDFData& Font::get_msdf_data() const {
	return msdf_data;
}

Ref<Texture2D> Font::get_atlas_texture() const {
	return atlas_texture;
}

Ref<Font> Font::get_default() {
	if (!s_default_font) {
		s_default_font = create_ref<Font>(ROBOTO_REGULAR_TTF_DATA, ROBOTO_REGULAR_TTF_LENGTH);
	}
	return s_default_font;
}
