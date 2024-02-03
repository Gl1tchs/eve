namespace EveEngine
{
	/// <summary>
	/// A static class providing functionality to manage scenes in the game.
	/// </summary>
	public static class SceneManager
	{
		/// <summary>
		/// Loads scene from given path.
		/// </summary>
		/// <param name="path">Path of the path in eve asset path format</param>
		/// <example>
		///  SceneManager.LoadScene("res://scenes/scene1.escn");
		/// </example>
		public static void LoadScene(string path)
		{
			Interop.scene_manager_load_scene(path);
		}
	}
}
