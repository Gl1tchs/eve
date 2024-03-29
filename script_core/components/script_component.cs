namespace EveEngine
{
	/// <summary>
	/// Component representing which script instance to use.
	/// </summary>
	public class ScriptComponent : Component
	{
		/// <summary>
		/// Name of the script class with Namespace.Class notation.
		/// This could be easily taken with "typeof(T).FullName"
		/// </summary>
		public string ClassName
		{
			get => Interop.script_component_get_class_name(Entity.Id);
		}
	}
}
