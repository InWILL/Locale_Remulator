using System;
using System.ComponentModel;
using System.Linq;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Input;

namespace LREditor
{
	/// <summary>
	/// Makes a ComboBox editable and filters its items by the typed text (case-insensitive "contains" match).
	/// </summary>
	internal class ComboBoxFilter
	{
		private readonly ComboBox comboBox;
		private readonly Func<object, string> getText;
		private ICollectionView view;
		private object lastValidItem;
		private bool suppress;

		public static void Attach(ComboBox comboBox, Func<object, string> getText)
		{
			new ComboBoxFilter(comboBox, getText);
		}

		private ComboBoxFilter(ComboBox comboBox, Func<object, string> getText)
		{
			this.comboBox = comboBox;
			this.getText = getText;
			comboBox.IsEditable = true;
			comboBox.IsTextSearchEnabled = false;
			comboBox.StaysOpenOnEdit = true;
			comboBox.AddHandler(TextBoxBase.TextChangedEvent, new TextChangedEventHandler(OnTextChanged));
			comboBox.SelectionChanged += (s, e) =>
			{
				if (comboBox.SelectedItem != null) lastValidItem = comboBox.SelectedItem;
			};
			comboBox.DropDownClosed += (s, e) => Commit();
			comboBox.LostKeyboardFocus += (s, e) =>
			{
				if (!comboBox.IsKeyboardFocusWithin && !comboBox.IsDropDownOpen) Commit();
			};
		}

		private ICollectionView View
		{
			get
			{
				if (view == null && comboBox.ItemsSource != null)
					view = CollectionViewSource.GetDefaultView(comboBox.ItemsSource);
				return view;
			}
		}

		private TextBox EditableTextBox
		{
			get { return comboBox.Template?.FindName("PART_EditableTextBox", comboBox) as TextBox; }
		}

		private void OnTextChanged(object sender, TextChangedEventArgs e)
		{
			if (suppress || View == null) return;
			var text = comboBox.Text ?? "";
			var selected = comboBox.SelectedItem;
			// Text was set by selecting an item, not by typing
			if (selected != null && getText(selected) == text) return;

			var textBox = EditableTextBox;
			int caret = textBox != null ? textBox.CaretIndex : text.Length;

			suppress = true;
			try
			{
				View.Filter = string.IsNullOrWhiteSpace(text)
					? (Predicate<object>)null
					: item => (getText(item) ?? "").IndexOf(text.Trim(), StringComparison.OrdinalIgnoreCase) >= 0;
				if (!comboBox.IsDropDownOpen) comboBox.IsDropDownOpen = true;
				// Filtering or opening the drop-down may overwrite / select-all the typed text; restore it
				if (textBox != null)
				{
					if (textBox.Text != text) textBox.Text = text;
					textBox.SelectionLength = 0;
					textBox.CaretIndex = Math.Min(caret, text.Length);
				}
			}
			finally
			{
				suppress = false;
			}
		}

		private void Commit()
		{
			if (View == null) return;
			suppress = true;
			try
			{
				if (comboBox.SelectedItem == null)
				{
					var text = (comboBox.Text ?? "").Trim();
					var match = View.Cast<object>().FirstOrDefault(i => string.Equals(getText(i), text, StringComparison.OrdinalIgnoreCase))
						?? (View.Cast<object>().Count() == 1 ? View.Cast<object>().First() : null)
						?? lastValidItem;
					View.Filter = null;
					comboBox.SelectedItem = match;
				}
				else
				{
					var selected = comboBox.SelectedItem;
					View.Filter = null;
					comboBox.SelectedItem = selected;
				}
				if (comboBox.SelectedItem != null) comboBox.Text = getText(comboBox.SelectedItem);
			}
			finally
			{
				suppress = false;
			}
		}
	}
}
