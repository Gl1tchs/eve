using EveEngine;

namespace Sample
{
	public class MyEntity : Entity
	{
		public Entity Camera;
		public float Speed = 10.0f;

		private Rigidbody2D _rb2d;

		protected override void OnCreate()
		{
			_rb2d = GetComponent<Rigidbody2D>();

			_rb2d.ApplyForce(Vector2.Right * 50);

			var col2d = GetComponent<BoxCollider2D>();
			col2d.OnTrigger = OnTrigger;

			var volume = GetComponent<PostProcessVolume>();
			volume.Vignette = new PostProcessVolume.VignetteSettings
			{
				enabled = true,
				strength = 0.25f
			};
		}

		protected override void OnUpdate(float dt)
		{
			if (Input.IsKeyPressed(KeyCode.Up))
			{
				Transform.Translate(Vector3.Up * Speed * dt);
			}
			if (Input.IsKeyPressed(KeyCode.Down))
			{
				Transform.Translate(Vector3.Down * Speed * dt);
			}

			if (Input.IsKeyPressed(KeyCode.Left))
			{
				Transform.Translate(Vector3.Left * Speed * dt);
			}
			if (Input.IsKeyPressed(KeyCode.Right))
			{
				Transform.Translate(Vector3.Right * Speed * dt);
			}
		}

		private void OnTrigger(ulong entityId)
		{
			Debug.LogWarning("Hello");
		}
	}
}
