namespace EveEngine
{
	public class PostProcessVolume : Component
	{
		public struct GrayScaleSettings
		{
			public bool enabled = false;

			public GrayScaleSettings()
			{
			}
		}

		public struct ChromaticAberrationSettings
		{
			public bool enabled = false;
			public Vector3 offset = new Vector3(0.009f, 0.006f, -0.006f);

			public ChromaticAberrationSettings()
			{
			}
		}

		public struct BlurSettings
		{
			public bool enabled = false;
			public uint size = 2;
			public float separation = 2.0f;

			public BlurSettings()
			{
			}
		}

		public struct SharpenSettings
		{
			public bool enabled = false;
			public float amount = 0.8f;

			public SharpenSettings()
			{
			}
		}

		public struct VignetteSettings
		{
			public bool enabled = false;
			public float inner = 0.5f;
			public float outer = 1.2f;
			public float strength = 0.8f;
			public float curvature = 0.5f;

			public VignetteSettings()
			{
			}
		}

		public bool IsGlobal
		{
			get => Interop.post_process_volume_component_get_is_global(Entity.Id);
			set => Interop.post_process_volume_component_set_is_global(Entity.Id, value);
		}

		public GrayScaleSettings GrayScale
		{
			get
			{
				Interop.post_process_volume_component_get_gray_scale(Entity.Id, out GrayScaleSettings settings);
				return settings;
			}
			set => Interop.post_process_volume_component_set_gray_scale(Entity.Id, ref value);
		}

		public ChromaticAberrationSettings ChromaticAberration
		{
			get
			{
				Interop.post_process_volume_component_get_chromatic_aberration(Entity.Id, out ChromaticAberrationSettings settings);
				return settings;
			}
			set => Interop.post_process_volume_component_set_chromatic_aberration(Entity.Id, ref value);
		}

		public BlurSettings Blur
		{
			get
			{
				Interop.post_process_volume_component_get_blur(Entity.Id, out BlurSettings settings);
				return settings;
			}
			set => Interop.post_process_volume_component_set_blur(Entity.Id, ref value);
		}

		public SharpenSettings Sharpen
		{
			get
			{
				Interop.post_process_volume_component_get_sharpen(Entity.Id, out SharpenSettings settings);
				return settings;
			}
			set => Interop.post_process_volume_component_set_sharpen(Entity.Id, ref value);
		}

		public VignetteSettings Vignette
		{
			get
			{
				Interop.post_process_volume_component_get_vignette(Entity.Id, out VignetteSettings settings);
				return settings;
			}
			set => Interop.post_process_volume_component_set_vignette(Entity.Id, ref value);
		}
	}
}
