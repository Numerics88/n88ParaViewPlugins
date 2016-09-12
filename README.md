# n88ParaViewPlugins

n88ParaViewPlugins is a collection of ParaView plugins for use with Numerics88 Faim finite
element software. In particular, currently readers for the Scanco AIM and the Numerics88
n88model file formats are provided.


## Using

We recommend that if possible you get pre-build binaries from http://numerics88.com/downloads .

Instructions for loading the plugins into ParaView can be found in the Faim manual
available at http://numerics88.com/documentation .

## Building

If you really want to build the plugins yourself, be warned that this is a difficult and
fragile process. This is partly because ParaView is a very complex project, which depends
on a great many open source projects, each of which has its own peculiarities for building.
Furthermore, in order for the plugins to work with the ParaView binaries available
from http://www.paraview.org/ , certain libraries much match exactly between the binary
ParaView and the build for the plugins.

As a first step, you should check out from the git repository for
n88ParaViewPlugins a tag corresponding to the exact version of ParaView that you want to compile
plugins for. As this documentation is part of the source code, you will then get the
exact instructions for that particular version of ParaView. Instructions are specific
to the operating system:

- [Linux](Build-Linux.md)
- [Windows](Build-Windows.md)
- [macOS](Build-Mac.md)


## Authors and Contributors

n88ParaViewPlugins was developed by Eric Nodwell (eric.nodwell@numerics88.com) at Numerics88
Solutions Ltd.


## Licence

n88ParaViewPlugins is licensed under a MIT-style open source license. See the file LICENSE.
