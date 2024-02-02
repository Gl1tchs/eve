#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

class FrameBuffer;
class Shader;
class Texture2D;
class VertexArray;

struct PostProcessVolume {
	struct GrayScaleSettings {
		bool enabled = false;
	};

	struct ChromaticAberrationSettings {
		bool enabled = false;
		glm::vec3 offset = { 0.009f, 0.006f, -0.006f };
	};

	struct BlurSettings {
		bool enabled = false;
		uint32_t size = 2;
		// seperation value of shader between 1 and infinity
		float seperation = 2.0f;
	};

	struct SharpenSettings {
		bool enabled = false;
		float amount = 0.8;
	};

	struct VignetteSettings {
		bool enabled = false;
		float inner = 0.5f;
		float outer = 1.2f;
		float strength = 0.8f;
		float curvature = 0.5f;
	};

	//! TODO implement this beheaviour
	bool is_global = true;
	GrayScaleSettings gray_scale{};
	ChromaticAberrationSettings chromatic_aberration{};
	BlurSettings blur{};
	SharpenSettings sharpen{};
	VignetteSettings vignette{};

	inline bool is_any_effect_provided() const {
		return gray_scale.enabled || chromatic_aberration.enabled || blur.enabled || sharpen.enabled || vignette.enabled;
	}
};

class PostProcessor {
public:
	PostProcessor();

	bool process(const Ref<FrameBuffer>& screen_buffer, const PostProcessVolume& _volume);

	Ref<FrameBuffer> get_frame_buffer();

	uint32_t get_frame_buffer_renderer_id() const;

private:
	void _process_gray_scale();

	void _process_chromatic_aberration();

	void _process_blur();

	void _process_sharpen();

	void _process_vignette();

	void _draw_screen_quad();

private:
	PostProcessVolume volume{};

	Ref<FrameBuffer> frame_buffer;

	Ref<VertexArray> vertex_array;

	Ref<Shader> gray_scale_shader;
	Ref<Shader> chromatic_aberration_shader;
	Ref<Shader> blur_shader;
	Ref<Shader> sharpen_shader;
	Ref<Shader> vignette_shader;

	uint32_t last_texture_id = 0;
	bool effect_provided = false;
};

#endif
