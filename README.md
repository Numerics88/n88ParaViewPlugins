# n88ParaViewPlugins

n88ParaViewPlugins is a collection of ParaView plugins for use with Numerics88 Faim finite
element software. In particular, currently readers for the Scanco AIM and the Numerics88
n88model file formats are provided.


## Using

We recommend that if possible you get pre-build binaries from https://bonelab.github.io/n88/downloads .

Instructions for loading the plugins into ParaView can be found in the Faim manual
available at https://bonelab.github.io/n88/documentation .

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
to the operating system, and can be found in the following files:

- [Build-Linux.md](./Build-Linux.md)
- [Build-Windows.md](./Build-Windows.md)
- [Build-Mac.md](./Build-Mac.md)


## Authors and Contributors

n88ParaViewPlugins was developed by Eric Nodwell at Numerics88
Solutions Ltd. Contact skboyd@ucalgary.ca for any questions.


## Licence

n88ParaViewPlugins is licensed under a MIT-style open source license. See the file LICENSE.
