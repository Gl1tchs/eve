namespace EveEngine
{
	public enum AssetType
	{
		None,
		Texture,
		Font,
		Scene
	}

	public abstract class Asset
	{
		public readonly ulong handle;
		public readonly string path;
		public readonly AssetType type;

		public Asset(ulong handle, string path, AssetType type)
		{
			this.handle = handle;
			this.path = path;
			this.type = type;
		}

		public Asset(string path, AssetType type)
		{
			this.path = path;
			this.type = type;

			this.handle = AssetRegistry.Load(path, type);
		}
	}
}
