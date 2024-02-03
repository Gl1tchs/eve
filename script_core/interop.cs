using System;
using System.Runtime.CompilerServices;

namespace EveEngine
{
	/// <summary>
	/// Stattic middleware class for C# and C++ communication.
	/// </summary>
	static partial class Interop
	{
		#region Application

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void application_quit();

		#endregion
		#region Window

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static WindowCursorMode window_get_cursor_mode();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void window_set_cursor_mode(WindowCursorMode mode);

		#endregion
		#region Debug

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void debug_log(string message);


		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void debug_log_info(string message);


		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void debug_log_warning(string message);


		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void debug_log_error(string message);


		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void debug_log_fatal(string message);

		#endregion
		#region Entity

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static object get_script_instance(ulong entityId);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static string entity_destroy(ulong entityId);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static ulong entity_get_parent(ulong entityId);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static string entity_get_name(ulong entityId);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool entity_has_component(ulong entityId, Type component_type);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void entity_add_component(ulong entityId, Type component_type);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static ulong entity_find_by_name(string name);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static ulong entity_instantiate(string name, ulong parentId, ref Vector3 position, ref Vector3 rotation, ref Vector3 scale);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static object entity_assign_script(ulong entityId, string class_name);

		#endregion
		#region TransformComponent

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_get_local_position(ulong entityId, out Vector3 position);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_set_local_position(ulong entityId, ref Vector3 position);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_get_local_rotation(ulong entityId, out Vector3 rotation);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_set_local_rotation(ulong entityId, ref Vector3 rotation);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_get_local_scale(ulong entityId, out Vector3 scale);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_set_local_scale(ulong entityId, ref Vector3 scale);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_get_position(ulong entityId, out Vector3 position);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_get_rotation(ulong entityId, out Vector3 rotation);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_get_scale(ulong entityId, out Vector3 scale);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_get_forward(ulong entityId, out Vector3 forward);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_get_right(ulong entityId, out Vector3 right);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_get_up(ulong entityId, out Vector3 up);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_translate(ulong entityId, ref Vector3 translation);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void transform_component_rotate(ulong entityId, float angle, ref Vector3 axis);

		#endregion
		#region CameraComponent

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void camera_component_camera_get_aspect_ratio(ulong entityId, out float aspectRatio);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void camera_component_camera_set_aspect_ratio(ulong entityId, ref float aspectRatio);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void camera_component_camera_get_zoom_level(ulong entityId, out float zoomLevel);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void camera_component_camera_set_zoom_level(ulong entityId, ref float zoomLevel);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void camera_component_camera_get_near_clip(ulong entityId, out float nearClip);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void camera_component_camera_set_near_clip(ulong entityId, ref float nearClip);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void camera_component_camera_get_far_clip(ulong entityId, out float farClip);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void camera_component_camera_set_far_clip(ulong entityId, ref float farClip);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void camera_component_get_is_primary(ulong entityId, out bool isPrimary);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void camera_component_set_is_primary(ulong entityId, ref bool isPrimary);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void camera_component_get_is_fixed_aspect_ratio(ulong entityId, out bool isFixedAspectRatio);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void camera_component_set_is_fixed_aspect_ratio(ulong entityId, ref bool isFixedAspectRatio);

		#endregion
		#region ScriptComponent

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static string script_component_get_class_name(ulong entityId);

		#endregion
		#region SceneManager

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void scene_manager_load_scene(string path);

		#endregion
		#region Input

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool input_is_key_pressed(KeyCode keyCode);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool input_is_key_released(KeyCode keyCode);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool input_is_mouse_pressed(MouseCode mouseCode);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static bool input_is_mouse_released(MouseCode mouseCode);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void input_get_mouse_position(out Vector2 position);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		internal extern static void input_get_scroll_offset(out Vector2 offset);

		#endregion
	}
}
