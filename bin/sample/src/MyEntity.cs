using EveEngine;

namespace Sample
{
	public class MyEntity : Entity
	{
		public Entity Camera;
		public float Speed = 10.0f;

		protected override void OnCreate()
		{
			var rb2d = GetComponent<Rigidbody2D>();
			rb2d.Type = Rigidbody2D.BodyType.Dynamic;

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
	}
}
