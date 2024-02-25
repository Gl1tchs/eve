using EveEngine;

namespace Sample
{
	public class Text : Entity
	{
		public Entity entity;
		public Color color;

		protected override void OnCreate()
		{
			var textComponent = GetComponent<TextRenderer>();
			textComponent.Text = entity.Name;
			textComponent.ForegroundColor = color;
		}
	}
}
