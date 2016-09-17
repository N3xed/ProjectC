using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace ProjectC_interface.Audio {
    public class AudioPage : PageBase {
        public delegate void OnCheckBoxEvent(CheckBox sender, EventArgs args);
        public delegate void OnListBoxEvent(ListBox sender, EventArgs args);

        public OnCheckBoxEvent SourceCanvasCheckBoxCheckedListener;
        public OnCheckBoxEvent SourceCanvasCheckBoxUncheckedListener;
        public OnListBoxEvent SourceListBoxSelectedListener;
        public OnListBoxEvent DestinationListBoxSelectedListener;

        public AudioPage(MainWindow window, TabItem tab) : base(window, tab) {
        }

        public void PopulateSourceList(ICollection<IAudioSource> sources) {
            Form.Invoke(() => {
                Form.sourceListBox.ItemsSource = sources;
            });
        }

        public void PopulateDestinationList(ICollection<IAudioDestination> destinations) {
            Form.Invoke(() => {
                Form.destinationItemsControl.ItemsSource = destinations;
            });
        }
    }
}
