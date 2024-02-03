namespace EveEngine
{
	/// <summary>
	/// Represents a camera component attached to an entity.
	/// </summary>
	public class CameraComponent : Component
	{
		/// <summary>
		/// Gets the orthographic camera associated with this camera component.
		/// </summary>
		public OrthographicCamera Camera
		{
			get
			{
				Camera.SetEntity(Entity);
				return Camera;
			}
		}

		/// <summary>
		/// Gets or sets a value indicating whether the camera is the primary camera in the scene.
		/// </summary>
		public bool IsPrimary
		{
			get
			{
				Interop.camera_component_get_is_primary(Entity.Id, out bool isPrimary);
				return isPrimary;
			}
			set => Interop.camera_component_set_is_primary(Entity.Id, ref value);
		}

		/// <summary>
		/// Gets or sets a value indicating whether the camera has a fixed aspect ratio.
		/// </summary>
		public bool IsFixedAspectRatio
		{
			get
			{
				Interop.camera_component_get_is_fixed_aspect_ratio(Entity.Id, out bool isFixedAspectRatio);
				return isFixedAspectRatio;
			}
			set => Interop.camera_component_set_is_fixed_aspect_ratio(Entity.Id, ref value);
		}
	}
}
