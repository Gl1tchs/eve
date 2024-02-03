namespace EveEngine
{
	public static class AssetRegistry
	{
		public static ulong Load(string path, AssetType type)
		{
			return Interop.asset_registry_load(path, type);
		}

		public static void Unload(ulong handle)
		{
			Interop.asset_registry_unload(handle);
		}
	}
}
