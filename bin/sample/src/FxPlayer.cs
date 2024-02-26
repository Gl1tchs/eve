using EveEngine;

namespace Sample
{
	class FxPlayer : Entity
	{
		public uint FrameCount;

		private SpriteRenderer _sprite;
		private float _animIndex = 0.0f;

		protected override void OnCreate()
		{
			_sprite = GetComponent<SpriteRenderer>();
		}

		protected override void OnUpdate(float dt)
		{
			_animIndex += 0.5f;

			if (_animIndex % 1 == 0)
			{
				_sprite.Index += 1;
			}
			if (_sprite.Index >= FrameCount)
			{
				_sprite.Index = 0;
				_animIndex = 0.0f;
			}
		}

	}
}
