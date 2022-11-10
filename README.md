# BeamCUE

BeamCUE is a commandline tool which uses the data from OutGauge to provide RGB effects using the Corsair CUE SDK. You can also monitor the OutGauge data coming out of your game. This has only been tested using BeamNG.drive, however this should work with any other games that support OutGauge. It listens on port 4444 by default.

Currently this takes exclusive access of all the Corsair peripherals, however only supports the keyboard as of now. I don't currently plan to support other RGB SDKs due to a lack of hardware, however this could change (and probably could be implemented easily regardless) in the future.

To build this, simply download and open the solution in Visual Studio. From there you can build it. Ensure the libraries are located in the folder, before you do so.
