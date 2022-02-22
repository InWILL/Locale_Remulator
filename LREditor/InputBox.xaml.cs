using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace LREditor
{
    /// <summary>
    /// Interaction logic for InputBox.xaml
    /// </summary>
    public partial class InputBox : Window
    {
        public string InputResult;
        public InputBox()
        {
            InitializeComponent();
            TextBox_NewName.Focus();
        }

        private void Button_Save_Click(object sender, RoutedEventArgs e)
        {
            InputResult = TextBox_NewName.Text;
            DialogResult = true;
            this.Close();
        }
    }
}
