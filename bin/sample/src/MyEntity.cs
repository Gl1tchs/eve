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

			var sprite = GetComponent<SpriteRenderer>();

			sprite.Index += 1;

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
				_rb2d.ApplyForce(Vector2.Up * Speed);
			}
			if (Input.IsKeyPressed(KeyCode.Down))
			{
				_rb2d.ApplyForce(Vector2.Down * Speed);
			}

			if (Input.IsKeyPressed(KeyCode.Left))
			{
				_rb2d.ApplyForce(Vector2.Left * Speed);
			}
			if (Input.IsKeyPressed(KeyCode.Right))
			{
				_rb2d.ApplyForce(Vector2.Right * Speed);
			}
		}

		private void OnTrigger(ulong entityId)
		{
			Debug.LogWarning("Hello");
		}
	}
}
