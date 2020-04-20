# library_ssWi

This library aims at implementing a simple communication protocol among nodes, abstracting from the hardware. The name ssWi stands for Shared Slotted Wireless. Wireless is part of the name, even though the library abstracts from the hardware, as the first version was entirely focused on the XBee modules and then the name has not been changed.

The communication channel is represented by ssWiChannel, an abstract class which models the channel that the transceivers access to. The concrete classes must implement the functions: init, read and write. The protocol automatically sends and receives data through the selected channel, exploiting the operting system timers. Addresses are not considered as the communication lays on broadcast transmissions.

The protocol provides the ssWiPort abstraction which is like memory areas shared among all the connected nodes. Reading from one port lets the node retrieve the last written value from the other nodes. Writing on one port means sending such value to other nodes.

Objects instantiated from ssWiSocket are the interface for allowing nodes to access the protocol ports.

![protocol idea](images/ssWi.png)

See https://os.mbed.com/users/mariob/code/ssWi/
