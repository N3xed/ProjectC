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

        public delegate void WindowHandler(MainWindow window);
        
        public Audio.AudioPage AudioTab { get; private set; }

        public WindowHandler ShutdownHandler;
        public WindowHandler MinimizedHandler;
        public WindowHandler MaximizedHandler;

        public MainWindow() {
            InitializeComponent();
            AudioTab = new Audio.AudioPage(this, audioTab);

            this.Closing += MainWindow_Closing;
            StateChanged += MainWindow_StateChanged;
        }

        private void MainWindow_StateChanged(object sender, EventArgs e) {
            switch(this.WindowState) {
                case WindowState.Maximized:
                    MaximizedHandler?.Invoke(this);
                    break;
                case WindowState.Minimized:
                    MinimizedHandler?.Invoke(this);
                    break;
            }
        }

        public void SetTitle(string title) {
            Invoke(() => {
                Title = title;
            });
        }

        private void MainWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e) {
            //TODO: Animation
            ShutdownHandler?.Invoke(this);
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
