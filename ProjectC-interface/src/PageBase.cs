using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace ProjectC_interface {
    public abstract class PageBase {
        public delegate void OnPageEvent(PageBase sender, TabItem tab);

        public OnPageEvent SelectedListener;
        public OnPageEvent UnselectedListener;

        public TabItem Tab { get; private set; }
        public MainWindow Form { get; private set; }

        public PageBase(MainWindow window, TabItem tab) {
            Form = window;
            Tab = tab;
            tab.Tag = this;

            (tab.Parent as TabControl).SelectionChanged += PageBase_SelectionChanged;
        }

        private void PageBase_SelectionChanged(object sender, SelectionChangedEventArgs e) {
            if(e.AddedItems.Contains(Tab)) {
                SelectedListener?.Invoke(this, Tab);
            } else if(e.RemovedItems.Contains(Tab)) {
                UnselectedListener?.Invoke(this, Tab);
            }
        }
    }
}
