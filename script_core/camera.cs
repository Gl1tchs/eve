namespace EveEngine
{
	/// <summary>
	/// Abstract class representing a camera on scene.
	/// </summary>
	public abstract class Camera
	{
		/// <summary>
		/// Aspect ratio of the screen.
		/// </summary>
		public float AspectRatio
		{
			get
			{
				Interop.camera_component_camera_get_aspect_ratio(Entity.Id, out float aspectRatio);
				return aspectRatio;
			}
			set
			{
				Interop.camera_component_camera_set_aspect_ratio(Entity.Id, ref value);
			}
		}

		/// <summary>
		/// Entity object which is attached to the camera.
		/// </summary>
		protected internal Entity Entity;

		internal void SetEntity(Entity entity)
		{
			Entity = entity;
		}
	}

	/// <summary>
	/// Class representing a orthographic camera on scene.
	/// </summary>
	public class OrthographicCamera : Camera
	{
		/// <summary>
		/// Camera frustum box size.
		/// </summary>
		public float ZoomLevel
		{
			get
			{
				Interop.camera_component_camera_get_zoom_level(Entity.Id, out float zoomLevel);
				return zoomLevel;
			}
			set
			{
				Interop.camera_component_camera_set_zoom_level(Entity.Id, ref value);
			}
		}

		/// <summary>
		/// Camera near clip. Objects will appear after this value.
		/// </summary>
		public float NearClip
		{
			get
			{
				Interop.camera_component_camera_get_near_clip(Entity.Id, out float nearClip);
				return nearClip;
			}
			set
			{
				Interop.camera_component_camera_set_near_clip(Entity.Id, ref value);
			}
		}

		/// <summary>
		/// Camera far clip. Objects will stop appering after this value.
		/// </summary>
		public float FarClip
		{
			get
			{
				Interop.camera_component_camera_get_far_clip(Entity.Id, out float farClip);
				return farClip;
			}
			set
			{
				Interop.camera_component_camera_set_far_clip(Entity.Id, ref value);
			}
		}
	}
}
