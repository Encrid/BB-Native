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
2. Run `python project.py fetch`. It downloads zig, the library of Bridge and the os layer of Bridge-OS.

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
The library is `bridge_native.dll`, `bridge_native.so` or `bridge_native.dylib`. The backend knows no setting.

## Messages

The backend writes one of these reasons when it refuses. Angle brackets mark a part that changes:

- `it knows no setting '<key>'`
- `the native SDK has no entry '<name>'`
- `no exported function '<name>'`
- `this backend opens a module's library by its path, and was handed none`
- `cannot open the library '<path>'`

Bridge puts each reason into a record, after the name of the backend or of the module.

## Contributing

| Document | Content |
| --- | --- |
| `docs/contributing/needs.md` | The needs that each decision meets, and how to weigh a decision. |
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
