namespace EveEngine
{
	public class TextRenderer : Component
	{
		public string Text
		{
			get => Interop.text_renderer_component_get_text(Entity.Id);
			set => Interop.text_renderer_component_set_text(Entity.Id, value);
		}

		public ulong Font
		{
			get => Interop.text_renderer_component_get_font(Entity.Id);
			set => Interop.text_renderer_component_set_font(Entity.Id, value);
		}

		public Color ForegroundColor
		{
			get
			{
				Interop.text_renderer_component_get_fg_color(Entity.Id, out Color color);
				return color;
			}
			set => Interop.text_renderer_component_set_fg_color(Entity.Id, ref value);
		}

		public Color BackgroundColor
		{
			get
			{
				Interop.text_renderer_component_get_bg_color(Entity.Id, out Color color);
				return color;
			}
			set => Interop.text_renderer_component_set_bg_color(Entity.Id, ref value);
		}

		public float Kerning
		{
			get => Interop.text_renderer_component_get_kerning(Entity.Id);
			set => Interop.text_renderer_component_set_kerning(Entity.Id, value);
		}

		public float LineSpacing
		{
			get => Interop.text_renderer_component_get_line_spacing(Entity.Id);
			set => Interop.text_renderer_component_set_line_spacing(Entity.Id, value);
		}

		public bool IsScreenSpace
		{
			get => Interop.text_renderer_component_get_is_screen_space(Entity.Id);
			set => Interop.text_renderer_component_set_is_screen_space(Entity.Id, value);
		}
	};
}
