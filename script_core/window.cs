namespace EveEngine
{
	/// <summary>
	/// Enum representing the cusor mode.
	/// </summary>
	public enum WindowCursorMode
	{
#pragma warning disable CS1591
		Normal,
		Hidden,
		Disabled,
		Captured,
#pragma warning restore CS1591
	}

	/// <summary>
	/// Static class representing the application window.
	/// </summary>
	public static class Window
	{
		/// <summary>
		/// Mode of the cursor on the window.
		/// </summary>
		public static WindowCursorMode CursorMode
		{
			get => Interop.window_get_cursor_mode();
			set => Interop.window_set_cursor_mode(value);
		}
	}
}
