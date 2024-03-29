namespace EveEngine
{
	/// <summary>
	/// Static class to visualize and log things fastly and natively.
	/// </summary>
	public static class Debug
	{
		/// <summary>
		/// Trace a log natively from engine.
		/// </summary>
		/// <param name="format">Format string</param>
		/// <param name="args">Arguments to format</param>
		public static void Log(string format, params object[] args)
		{
			string message = string.Format(format, args);
			Interop.debug_log(message);
		}

		/// <summary>
		/// Trace an info log natively from engine.
		/// </summary>
		/// <param name="format">Format string</param>
		/// <param name="args">Arguments to format</param>
		public static void LogInfo(string format, params object[] args)
		{
			string message = string.Format(format, args);
			Interop.debug_log_info(message);
		}

		/// <summary>
		/// Trace a warning log natively from engine.
		/// </summary>
		/// <param name="format">Format string</param>
		/// <param name="args">Arguments to format</param>
		public static void LogWarning(string format, params object[] args)
		{
			string message = string.Format(format, args);
			Interop.debug_log_warning(message);
		}

		/// <summary>
		/// Trace an error log natively from engine.
		/// </summary>
		/// <param name="format">Format string</param>
		/// <param name="args">Arguments to format</param>
		public static void LogError(string format, params object[] args)
		{
			string message = string.Format(format, args);
			Interop.debug_log_error(message);
		}

		/// <summary>
		/// Trace a fatal log natively from engine.
		/// </summary>
		/// <param name="format">Format string</param>
		/// <param name="args">Arguments to format</param>
		public static void LogFatal(string format, params object[] args)
		{
			string message = string.Format(format, args);
			Interop.debug_log_fatal(message);
		}
	}
}
