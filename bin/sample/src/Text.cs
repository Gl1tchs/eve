using EveEngine;

namespace Sample
{
	public class Text : Entity
	{
		public string text;
		public Color color;

		protected override void OnCreate()
		{
			var textComponent = GetComponent<TextRendererComponent>();
			textComponent.Text = "Hello, World!";
			textComponent.ForegroundColor = color;
		}
	}
}
