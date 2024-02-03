namespace EveEngine
{
	/// <summary>
	/// A static class providing methods to query input state, such as key and mouse button presses.
	/// </summary>
	public static class Input
	{
		/// <summary>
		/// Checks if the specified keyboard key is currently pressed.
		/// </summary>
		/// <param name="keyCode">The KeyCode of the key to check.</param>
		/// <returns>True if the key is currently pressed, otherwise false.</returns>
		public static bool IsKeyPressed(KeyCode keyCode)
		{
			return Interop.input_is_key_pressed(keyCode);
		}

		/// <summary>
		/// Checks if the specified keyboard key has been released since the last frame.
		/// </summary>
		/// <param name="keyCode">The KeyCode of the key to check.</param>
		/// <returns>True if the key has been released, otherwise false.</returns>
		public static bool IsKeyReleased(KeyCode keyCode)
		{
			return Interop.input_is_key_released(keyCode);
		}

		/// <summary>
		/// Checks if the specified mouse button is currently pressed.
		/// </summary>
		/// <param name="mouseCode">The MouseCode of the mouse button to check.</param>
		/// <returns>True if the mouse button is currently pressed, otherwise false.</returns>
		public static bool IsMousePressed(MouseCode mouseCode)
		{
			return Interop.input_is_mouse_pressed(mouseCode);
		}

		/// <summary>
		/// Checks if the specified mouse button has been released since the last frame.
		/// </summary>
		/// <param name="mouseCode">The MouseCode of the mouse button to check.</param>
		/// <returns>True if the mouse button has been released, otherwise false.</returns>
		public static bool IsMouseReleased(MouseCode mouseCode)
		{
			return Interop.input_is_mouse_released(mouseCode);
		}

		/// <summary>
		/// Retrieves the current mouse position.
		/// </summary>
		/// <returns>A Vector2 representing the current mouse position.</returns>
		public static Vector2 GetMousePosition()
		{
			Interop.input_get_mouse_position(out Vector2 position);
			return position;
		}

		/// <summary>
		/// Retrieves the current mouse scroll offset.
		/// </summary>
		/// <returns>A Vector2 representing the current offset of the mouse scroll.</returns>
		public static Vector2 GetScrollOffset()
		{
			Interop.input_get_scroll_offset(out Vector2 offset);
			return offset;
		}
	}
}
