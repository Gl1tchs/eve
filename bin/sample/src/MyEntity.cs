using EveEngine;

namespace Sample
{
	public class MyEntity : Entity
	{
		public float Speed = 10.0f;

		protected override void OnCreate()
		{
			Debug.LogInfo("Hello, World!");
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
