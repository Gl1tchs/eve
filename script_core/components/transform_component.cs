namespace EveEngine
{
	/// <summary>
	/// Represents a transform component attached to an entity.
	/// </summary>
	public class TransformComponent : Component
	{
		/// <summary>
		/// Parent transform of current's
		/// </summary>
		public TransformComponent Parent
		{
			get
			{
				if (Entity.Parent is null)
				{
					return null;
				}

				return Entity.Parent.GetComponent<TransformComponent>();
			}
		}

		/// <summary>
		/// Gets or sets the position of the transform.
		/// </summary>
		public Vector3 Position
		{
			get
			{
				Interop.transform_component_get_local_position(Entity.Id, out Vector3 position);
				return position;
			}
			set => Interop.transform_component_set_local_position(Entity.Id, ref value);
		}

		/// <summary>
		/// Gets or sets the rotation of the transform.
		/// </summary>
		public Vector3 Rotation
		{
			get
			{
				Interop.transform_component_get_local_rotation(Entity.Id, out Vector3 rotation);
				return rotation;
			}
			set => Interop.transform_component_set_local_rotation(Entity.Id, ref value);
		}

		/// <summary>
		/// Gets or sets the scale of the transform.
		/// </summary>
		public Vector3 Scale
		{
			get
			{
				Interop.transform_component_get_local_scale(Entity.Id, out Vector3 scale);
				return scale;
			}
			set => Interop.transform_component_set_local_scale(Entity.Id, ref value);
		}

		/// <summary>
		/// Gets the world position of the transform.
		/// </summary>
		public Vector3 WorldPosition
		{
			get
			{
				Interop.transform_component_get_position(Entity.Id, out Vector3 position);
				return position;
			}
		}

		/// <summary>
		///  Gets the world rotation of the transform.
		/// </summary>
		public Vector3 WorldRotation
		{
			get
			{
				Interop.transform_component_get_rotation(Entity.Id, out Vector3 rotation);
				return rotation;
			}
		}

		/// <summary>
		///  Gets the world scale of the transform.
		/// </summary>
		public Vector3 WorldScale
		{
			get
			{
				Interop.transform_component_get_scale(Entity.Id, out Vector3 scale);
				return scale;
			}
		}


		/// <summary>
		/// Gets the forward direction of the transform.
		/// </summary>
		/// <returns>The forward vector.</returns>
		public Vector3 Forward
		{
			get
			{
				Interop.transform_component_get_forward(Entity.Id, out Vector3 forward);
				return forward;
			}
		}

		/// <summary>
		/// Gets the right direction of the transform.
		/// </summary>
		/// <returns>The right vector.</returns>
		public Vector3 Right
		{
			get
			{
				Interop.transform_component_get_right(Entity.Id, out Vector3 right);
				return right;
			}
		}

		/// <summary>
		/// Gets the up direction of the transform.
		/// </summary>
		/// <returns>The up vector.</returns>
		public Vector3 Up
		{
			get
			{
				Interop.transform_component_get_up(Entity.Id, out Vector3 up);
				return up;
			}
		}

		/// <summary>
		/// Translates the transform by translation.
		/// </summary>
		/// <param name="translation">Value to translate for.</param>
		public void Translate(Vector3 translation)
		{
			Interop.transform_component_translate(Entity.Id, ref translation);
		}

		/// <summary>
		/// Rotates the transform by angle at <c>axis</c>.
		/// </summary>
		/// <param name="angle">Angle to rotate for.</param>
		/// <param name="axis">Axis to rotate on.</param>
		public void Rotate(float angle, Vector3 axis)
		{
			Interop.transform_component_rotate(Entity.Id, angle, ref axis);
		}
	}
}
