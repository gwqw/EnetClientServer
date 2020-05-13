# EnetClientServer
C++ client-server wrapper-classes on `enet` (http://enet.bespin.org/) library.

## EnetServer

`enet_server` - server, starts listen port, has 2 channel: 0 -- reliable channel, 1 -- unreliable channel

Command line:

`enet_server <port_number>`

For exit you can enter `quit` in command line.

By default server outputs received data to `standart output`.

## EnetClient

`enet_client` -- client, connects to address and port number. You can transmit data in reliable channel (0), and unreliable channel (1).

Command line:

`enet_client <address> [<port_number>]`

By default client transmits text data by reliable channel and binary data by unreliable channel.

```sh
s:<string_data>     # transmits string
a[0],a[1],a[2],a[3] # transmits array of integer
```

For exit you can push `ctrl+D` (in `unix` systems).