# OpenRGB Plugins

There isn't much of a reason for this document to exist but some plugins may require special installation

## Auto Updates

You may notice that the windows version ships with the plugin itself (``ORGBUpdatePlugin.dll``) and 3 other dll files (``Openssl.dll``/``libcrypto.dll`` and ``Qt5Network.dll``)\

The other 3 go in the folder with the executable for OpenRGB

Otherwise the plugin *will not load*

With only Qt5Network it may load but it will spit a failed to parse initial commit page error if you don't also have the 2 OpenSSL DLLs with it
