namespace EveEngine
{
	public delegate void ColliderOnTriggerDelegate(ulong entityId);

	public class Rigidbody2D : Component
	{
		public enum BodyType
		{
			Static = 0,
			Dynamic,
			Kinematic,
		}

		public BodyType Type
		{
			get => Interop.rigidbody2d_component_get_type(Entity.Id);
			set => Interop.rigidbody2d_component_set_type(Entity.Id, value);
		}

		public bool FixedRotation
		{
			get => Interop.rigidbody2d_component_get_fixed_rotation(Entity.Id);
			set => Interop.rigidbody2d_component_set_fixed_rotation(Entity.Id, value);
		}
	}
}
