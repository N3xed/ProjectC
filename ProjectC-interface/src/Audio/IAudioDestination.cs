using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectC_interface.Audio {
    public interface IAudioDestination {
        string Name { get; }
        string IPAddressText { get; }

        ICollection<IDestinationAudioDevice> Devices { get; }
    }
}
