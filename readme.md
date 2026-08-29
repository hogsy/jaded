# Jaded

<div align="center">

![Jaded Icon](resources/jade-editor-icon_128.png)

[Instructions](#instructions) | [Contributing](#contributing)

----

</div>

Jaded is a community patched fork of Ubisoft Montpellier's Jade Engine, based on the copy originally available [here](https://archive.org/details/rayman-4-prototype-source-code), which is from around late May or early June 2006.
The goal of this project is to fix bugs, improve stability and modernize the engine so it can be used by the community.

This project is still very much a work-in-progress and there's a long way to go.

## Instructions

This project is not designed to be used with the original prototype Rayman 4 BF data.
While there's nothing to _prevent_ you from using it, just mind that fur will not be displayed correctly until you modify the materials again.

Essentially the flag toggling the fur was set incorrectly internally, which was rectified by Droolie.
Instead, please use the BF created by Droolie, [here](https://www.raymanpc.com/forum/viewtopic.php?t=75066).

In hindsight, we probably could've fixed this through some sort of migration, but it's a bit late for it now.

Alternatively, the editor does technically allow you to create a completely from-scratch project if you want to, but you're very much on your own there.

Further information on using Jade is available per the [raydev.wiki](https://raydev.wiki/jade:overview) website.

### Installing

It's not necessary to compile Jaded yourself if you're just interested in using the runtime or editor.

You can either download the latest "stable" release via the [Releases](https://github.com/hogsy/jaded/releases) section on GitHub.

Or alternatively you can grab the very latest build from a successful job [here](https://github.com/hogsy/jaded/actions/workflows/debug.yml).
Once you've accessed the page linked, click on the latest run and at the bottom is a download from the produced artifact.

### Compiling

Currently Jaded will only compile on Windows and for the x86 target architecture, and has only been tested on Windows 10 onwards.
Debug builds are recommended instead of release, purely as these can be more stable and will provide you with more information if something goes wrong.

The project has been updated to use utilise [CMake](https://cmake.org/), so compilation will naturally require that to be installed before you can proceed. Once done, you can do the following.

1. Use the `create_project_*` batch. As of Jan. 2026, you'll want the one targeting x86.
2. Navigate into the [solutions](solutions) directory, and open `Jaded.sln`.
3. You should be able to build the project successfully (if not, open a ticket).
4. Output should be found under [runtime](runtime), under the root directory.

If instead you would prefer to compile the project via the command-line, you can then execute the following from under the [solutions](solutions) directory.

```
cmake --build . --config Debug --target Jaded --parallel 8
```

## Contributing

Interested in helping out?
Feel free to fork and open a pull-request, or get in touch!

See the [contributing](CONTRIBUTING.md) guide for more information.
