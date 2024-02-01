#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

class FrameBuffer;
class Shader;
class Texture2D;
class VertexArray;

enum PostProcessingEffect : uint16_t {
	POST_PROCESSING_EFFECT_NONE = 0,
	POST_PROCESSING_EFFECT_GRAY_SCALE = 1 << 0,
	POST_PROCESSING_EFFECT_CHROMATIC_ABERRATION = 1 << 1,
	POST_PROCESSING_EFFECT_BLUR = 1 << 2,
	POST_PROCESSING_EFFECT_SHARPEN = 1 << 3,
	POST_PROCESSING_EFFECT_VIGNETTE = 1 << 4,
};

struct PostProcessorSettings {
	struct ChromaticAberrationSettings {
		float red_offset = 0.009f;
		float green_offset = 0.006f;
		float blue_offset = -0.006f;
	};

	struct BlurSettings {
		uint32_t size = 2;
		// seperation value of shader between 1 and infinity
		float seperation = 2.0f;
	};

	struct SharpenSettings {
		float amount = 0.8;
	};

	struct VignetteSettings {
		float inner = 0.5f;
		float outer = 1.2f;
		float strength = 0.8f;
		float curvature = 0.5f;
	};

	ChromaticAberrationSettings chromatic_aberration{};
	BlurSettings blur{};
	SharpenSettings sharpen{};
	VignetteSettings vignette{};
};

class PostProcessor {
public:
	PostProcessor();

	void process(uint16_t effects, const Ref<FrameBuffer>& screen_buffer);

	uint32_t get_frame_buffer_renderer_id() const;

	PostProcessorSettings& get_settings();
	void set_settings(const PostProcessorSettings& _settings);

private:
	void _process_gray_scale(uint32_t texture_id);

	void _process_chromatic_aberration(uint32_t texture_id);

	void _process_blur(uint32_t texture_id);

	void _process_sharpen(uint32_t texture_id);

	void _process_vignette(uint32_t texture_id);

	void _process_empty(uint32_t screen_texture);

private:
	PostProcessorSettings settings{};

	Ref<FrameBuffer> frame_buffer;

	Ref<VertexArray> vertex_array;

	Ref<Shader> screen_shader;
	Ref<Shader> gray_scale_shader;
	Ref<Shader> chromatic_aberration_shader;
	Ref<Shader> blur_shader;
	Ref<Shader> sharpen_shader;
	Ref<Shader> vignette_shader;

	uint32_t last_texture_id = 0;
	bool effect_provided = false;
};

#endif
