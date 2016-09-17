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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace ProjectC_interface {
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {

        public Audio.AudioPage AudioTab { get; private set; }

        public MainWindow() {
            InitializeComponent();
            AudioTab = new Audio.AudioPage(this, audioTab);
        }

        public void Invoke(Action action) {
            Dispatcher.Invoke(action);
        }

        private void sourceCanvasCheckBox_checked(object sender, RoutedEventArgs args) {
            AudioTab.SourceCanvasCheckBoxCheckedListener?.Invoke((CheckBox)sender, args);
        }

        private void sourceCanvasCheckBox_unchecked(object sender, RoutedEventArgs args) {
            AudioTab.SourceCanvasCheckBoxUncheckedListener?.Invoke((CheckBox)sender, args);
        }

        private void sourceListBox_Selected(object sender, RoutedEventArgs e) {
            AudioTab.SourceListBoxSelectedListener?.Invoke((ListBox)sender, e);
        }

        private void destinationListBox_Selected(object sender, RoutedEventArgs e) {
            AudioTab.DestinationListBoxSelectedListener?.Invoke((ListBox)sender, e);
        }
    }
}
