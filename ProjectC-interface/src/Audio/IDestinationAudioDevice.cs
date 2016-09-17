using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectC_interface.Audio {
    public interface IDestinationAudioDevice {
        string DisplayName { get; }
        string DeviceName { get; }
        string StatusText { get; }

        bool UISelected { get; set; }
    }
}
