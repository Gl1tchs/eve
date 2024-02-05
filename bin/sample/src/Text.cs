using EveEngine;

namespace Sample
{
	public class Text : Entity
	{
		public Color color;

		protected override void OnCreate()
		{
			var textComponent = GetComponent<TextRenderer>();
			textComponent.Text = "Hello, World!";
			textComponent.ForegroundColor = color;
		}
	}
}
