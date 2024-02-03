namespace EveEngine
{
	public enum TextureFormat
	{
		Red,
		RG,
		RGB,
		BGR,
		RGBA,
		BGRA,
	}

	public enum TextureFilteringMode
	{
		Nearest,
		Linear,
	}

	public enum TextureWrappingMode
	{
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
	}
		
	public struct TextureMetadata
	{
		public TextureFormat format = TextureFormat.RGBA;
		public TextureFilteringMode minFilter = TextureFilteringMode.Linear;
		public TextureFilteringMode magFilter = TextureFilteringMode.Linear;
		public TextureWrappingMode wrapS = TextureWrappingMode.ClampToEdge;
		public TextureWrappingMode wrapT = TextureWrappingMode.ClampToEdge;
		public bool generateMipmaps = true;

		public TextureMetadata()
		{
		}
	}

	public class Texture2D : Asset
	{
		public TextureMetadata Metadata { get; set; }

		public Texture2D(string path) : base(path, AssetType.Texture)
		{
		}

		public void SetData(byte[] data, ulong size)
		{

		}
	}
}
