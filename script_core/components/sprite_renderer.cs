namespace EveEngine
{
	public class SpriteRenderer : Component
	{
		public ulong Texture
		{
			get => Interop.sprite_renderer_component_get_texture(Entity.Id);
			set => Interop.sprite_renderer_component_set_texture(Entity.Id, value);
		}

		public Color Color
		{
			get
			{
				Interop.sprite_renderer_component_get_color(Entity.Id, out Color color);
				return color;
			}
			set => Interop.sprite_renderer_component_set_color(Entity.Id, ref value);
		}

		public Vector2 TexTiling
		{
			get
			{
				Interop.sprite_renderer_component_get_tex_tiling(Entity.Id, out Vector2 texTiling);
				return texTiling;
			}
			set => Interop.sprite_renderer_component_set_tex_tiling(Entity.Id, ref value);
		}

		public bool IsAtlas
		{
			get => Interop.sprite_renderer_component_get_is_atlas(Entity.Id);
			set => Interop.sprite_renderer_component_set_is_atlas(Entity.Id, value);
		}

		public Vector2 BlockSize
		{
			get
			{
				Interop.sprite_renderer_component_get_block_size(Entity.Id, out Vector2 blockSize);
				return blockSize;
			}
			set => Interop.sprite_renderer_component_set_block_size(Entity.Id, ref value);
		}

		public uint Index
		{
			get => Interop.sprite_renderer_component_get_index(Entity.Id);
			set => Interop.sprite_renderer_component_set_index(Entity.Id, value);
		}
	}
}
