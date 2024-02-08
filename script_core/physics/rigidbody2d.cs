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

		public enum ForceMode
		{
			Force = 0,
			Impulse
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

		public Vector2 Velocity
		{
			get
			{
				Interop.rigidbody2d_component_get_velocity(Entity.Id, out Vector2 velocity);
				return velocity;
			}
		}

		public float AngularVelocity
		{
			get => Interop.rigidbody2d_component_get_angular_velocity(Entity.Id);
		}

		public void ApplyForce(Vector2 force, ForceMode forceMode = ForceMode.Force)
		{
			ApplyForce(force, Vector2.Zero, forceMode);
		}

		public void ApplyForce(Vector2 force, Vector2 offset, ForceMode forceMode = ForceMode.Force)
		{
			Interop.rigidbody2d_component_add_force(Entity.Id, forceMode, ref force, ref offset);
		}

		public void ApplyAngularImpulse(float angularImpulse)
		{
			Interop.rigidbody2d_component_add_angular_impulse(Entity.Id, angularImpulse);
		}

		public void ApplyTorque(float torque)
		{
			Interop.rigidbody2d_component_add_torque(Entity.Id, torque);
		}
	}
}
