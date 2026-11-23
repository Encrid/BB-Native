# BB-Native

BB-Native is the native backend of Bridge: it runs each module as a shared library, in the process of the host.

## Background

Bridge loads modules onto a world, and a backend runs them. BB-Native is the backend `native`.
It opens the library of a module from the path that the host gave, and finds each export by its name, as `bridge_<name>`.
A native module runs with the access of the host, for the speed of the hardware: the backend trusts its modules.
The backend keeps none of the limits of a world.

A module compiles against the SDK of this repository: `docs/extern/bridge_native.h.md` tells how to write one.
The SDK includes `bridge_abi.h` and `bridge_schema.h`, thus a module also compiles with the headers of a release of Bridge.

## Install

1. Install Python 3.11 or a later version.
2. Run `python project.py fetch`. It downloads zig, the library of Bridge and the os layer, mosl.

## Usage

| Command | Result |
| --- | --- |
| `python project.py fetch` | Downloads what the build needs. |
| `python project.py build` | Builds the backend. |
| `python project.py pack` | Packs the backend and its SDK for a release. |
| `python project.py test` | Builds the backend, and the programs and modules of the tests. Then runs the tests. |
| `python project.py test --break` | Runs the tests with their claims made false. Each test must fail. |
| `python project.py test --branches` | Runs the tests on a build with probes. Each branch of the code must run. |
| `python project.py test --leaks` | Runs the tests on a build with counts. Each test must give back what the code takes. |
| `python project.py check` | Checks the project. |
| `python project.py clean` | Removes what the build made. Keeps what `fetch` downloaded. |
| `python project.py distclean` | Removes the folder `build`, also what `fetch` downloaded. |

A host names the backend in its deployment, with the name `native` and the path of its library as `library`.
The library is `bridge_native.dll`, `bridge_native.so` or `bridge_native.dylib`.

## The copies

The backend needs 1 setting, `copies`: the path of a folder, as a text. The backend makes the folder if it is not there.
For each load and each reload, the backend makes the folder `<generation>` in it. It writes a copy of the library there under its own name, and opens it.
Beside the copy, it makes a hard link of each other file of the folder of the library. So the loader finds a runtime such as SDL3.

When the system refuses a link, it copies only a library of the system, and skips the other files.
When the module unloads, the backend closes the copy and removes its folder.

Thus the backend never holds the file of the host open:

- A reload gives the new build. An open of the same path would give the library that is open already: on Windows, Linux and macOS.
- On Windows, a program can write, move or remove the file of a loaded module. No program can write onto a library that is open.
- Replace a library beside a loaded module by a move onto its file. A hard link shares its bytes, so a write in place changes the library of the copy.

The folder belongs to 1 backend at a time. Give each host a folder of its own, and never a path of its content.
At its start, the backend removes what an earlier run left: each folder of digits only, and each file `<generation>-<file name>` of v0.6.0. It removes no other entry.

A copy that does not go when its module unloads, for example a library that the system still holds, stays with no message. The next start of a backend on that folder removes it.
The backend is built against `BACKEND_ABI` 1, the number of the backend seam of Bridge. A Bridge of another number refuses it.

## Messages

The backend writes one of these reasons when it refuses. Angle brackets mark a part that changes:

- `it knows no setting '<key>'`
- `it needs the setting copies: a folder for the copies of the modules`
- `it states the setting copies 2 times`
- `the setting copies is a folder, as a text`
- `cannot make the folder of the copies '<path>'`
- `cannot make the folder of a copy '<path>'`
- `the native SDK has no entry '<name>'`
- `no exported function '<name>'`
- `this backend opens a module's library by its path, and was handed none`
- `cannot open the library '<path>'`, also when the library does not read or its copy does not write
- `out of memory: there was none for <what>`

Bridge puts each reason into a record of its own, at a load and at a reload: https://github.com/Encrid/Bridge/blob/v0.9.0/docs/extern/bridge.h.md#messages.

An entry that gets no memory gives null, with the last reason of the list. This table gives `<what>` of each entry:

| Entry | `<what>` |
| --- | --- |
| `backend_create` | `the backend`, `the folder of the copies` |
| `module_compile` | `the path of a copy`, `the image of a module` |
| `instance_create` | `an instance of an image` |
| `instance_get_func` | `a function of an instance` |

## Contributing

| Document | Content |
| --- | --- |
| `docs/contributing/needs.md` | The needs that each decision meets. |
| `docs/contributing/decisions.md` | The steps of a decision. |
| `docs/contributing/coding_style.md` | How to write the code. |
| `docs/contributing/documentation_style.md` | How to write the documents. |
| `docs/contributing/tests.md` | How to write the tests. |
| `docs/contributing/commits.md` | How to write a commit. |
| `docs/contributing/pull_request.md` | How to write a pull request. |
| `docs/contributing/agreement.md` | The agreement `cla.md`, which each contributor signs. |

## License

The GNU Affero General Public License, version 3 only. The file `license` holds the text.

The backend is built with the headers of Bridge, and Bridge loads it. Thus it is under the same licence, and its source goes with it.
For a work that stays closed, other terms are for sale: ask the account `Encrid` on the service that hosts this repository.
