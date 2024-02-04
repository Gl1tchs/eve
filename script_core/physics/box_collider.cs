namespace EveEngine
{
	public class BoxCollider2D : Component
	{
		public Vector2 Offset
		{
			get
			{
				Interop.box_collider2d_component_get_offset(Entity.Id, out Vector2 offset);
				return offset;
			}
			set => Interop.box_collider2d_component_set_offset(Entity.Id, ref value);
		}

		public Vector2 Size
		{
			get
			{
				Interop.box_collider2d_component_get_size(Entity.Id, out Vector2 size);
				return size;
			}
			set => Interop.box_collider2d_component_set_size(Entity.Id, ref value);
		}

		public float Density
		{
			get => Interop.box_collider2d_component_get_density(Entity.Id);
			set => Interop.box_collider2d_component_set_density(Entity.Id, value);
		}

		public float Friction
		{
			get => Interop.box_collider2d_component_get_friction(Entity.Id);
			set => Interop.box_collider2d_component_set_friction(Entity.Id, value);
		}

		public float Restitution
		{
			get => Interop.box_collider2d_component_get_restitution(Entity.Id);
			set => Interop.box_collider2d_component_set_restitution(Entity.Id, value);
		}

		public float RestitutionThreshold
		{
			get => Interop.box_collider2d_component_get_restitution_threshold(Entity.Id);
			set => Interop.box_collider2d_component_set_restitution_threshold(Entity.Id, value);
		}
	}

}
