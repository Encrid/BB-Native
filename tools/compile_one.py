# Copyright (C) 2026 Enkrid
#
# Under the GNU Affero General Public License, version 3 only: see license.
import importlib.util
import subprocess
import sys
import tempfile
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TOOLS = ROOT / '.toolchain' / 'tools'


def toolchain_fetch():
    sys.path.insert(0, str(TOOLS))
    spec = importlib.util.spec_from_file_location('fetch', TOOLS / 'fetch.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main(args):
    if not args:
        sys.exit('usage: python tools/compile_one.py <source> [<include folder>...]')
    zig_config = tomllib.loads((ROOT / '.toolchain' / 'toolchain.toml').read_text(encoding='utf-8'))['zig']
    zig = toolchain_fetch().zig_found(zig_config)
    if not zig:
        sys.exit('compile_one: zig %s is neither fetched nor on the path' % zig_config['version'])
    with tempfile.TemporaryDirectory() as folder_out:
        command = [zig, 'cc', '-std=c23', '-c', args[0], '-o', str(Path(folder_out) / 'one.o')]
        for folder in args[1:]:
            command += ['-I', folder]
        done = subprocess.run(command, capture_output=True, text=True, errors='replace')
    sys.stdout.write(done.stdout + done.stderr)
    sys.exit(done.returncode)


main(sys.argv[1:])
