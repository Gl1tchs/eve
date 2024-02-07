using System.Runtime.InteropServices;

namespace EveEngine
{
	public class CircleCollider2D : Component
	{
		public Vector2 Offset
		{
			get
			{
				Interop.circle_collider2d_component_get_offset(Entity.Id, out Vector2 offset);
				return offset;
			}
			set => Interop.circle_collider2d_component_set_offset(Entity.Id, ref value);
		}

		public float Radius
		{
			get => Interop.circle_collider2d_component_get_radius(Entity.Id);
			set => Interop.circle_collider2d_component_set_radius(Entity.Id, value);
		}

		public bool IsTrigger
		{
			get => Interop.circle_collider2d_component_get_is_trigger(Entity.Id);
			set => Interop.circle_collider2d_component_set_is_trigger(Entity.Id, value);
		}

		public ColliderOnTriggerDelegate OnTrigger
		{
			set
			{
				Interop.circle_collider2d_component_set_on_trigger(Entity.Id, Marshal.GetFunctionPointerForDelegate(value));
			}
		}

		public float Density
		{
			get => Interop.circle_collider2d_component_get_density(Entity.Id);
			set => Interop.circle_collider2d_component_set_density(Entity.Id, value);
		}

		public float Friction
		{
			get => Interop.circle_collider2d_component_get_friction(Entity.Id);
			set => Interop.circle_collider2d_component_set_friction(Entity.Id, value);
		}

		public float Restitution
		{
			get => Interop.circle_collider2d_component_get_restitution(Entity.Id);
			set => Interop.circle_collider2d_component_set_restitution(Entity.Id, value);
		}

		public float RestitutionThreshold
		{
			get => Interop.circle_collider2d_component_get_restitution_threshold(Entity.Id);
			set => Interop.circle_collider2d_component_set_restitution_threshold(Entity.Id, value);
		}
	}
}
