"""BB-Native's build: one compiler, one script.

    python build.py --bridge DIR                 build, for this machine
    python build.py --bridge DIR test [-k NAME]  build, then run the cases (all, or those matching)
    python build.py --bridge DIR test --copies 6 each case six times at once: under load
    python build.py --bridge DIR --target T      one of TARGETS, below
    python build.py --bridge DIR --debug         unoptimised, with debug information
    python build.py --bridge DIR package         build, run every case, and make what is handed

Needs zig on the path. DIR is a folder that holds the two packages of Bridge, `bridge` and
`seams`, each with its statement: they are taken as they are stated, and a file that is not
what its hash says is refused. Every path is taken from where this file is and from --out.
"""
import argparse
import concurrent.futures
import hashlib
import os
import platform
import re
import shutil
import subprocess
import sys
import tarfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent
# What differs by target, and nothing else does: where it runs, how a program and a library are
# named, which folder of utils/os is its operating system's, which of the system's declarations are asked for by name (the language is ISO C, and a
# compiler's gnu mode would have shown them unasked), what a link names, and how a library finds what
# sits beside it.
TARGETS = {
    'x86_64-windows-gnu': {
        'runs on': ('Windows', 'AMD64'), 'program': '.exe', 'library': '.dll', 'os': 'windows',
        'compile': [], 'link': [], 'beside': [], 'named': []},
    'x86_64-linux-gnu': {
        'runs on': ('Linux', 'x86_64'), 'program': '', 'library': '.so', 'os': 'posix',
        'compile': ['-fPIC', '-D_DEFAULT_SOURCE'], 'link': ['-lm', '-lpthread', '-ldl'],
        'beside': ['-Wl,-rpath,$ORIGIN'], 'named': ['-Wl,-soname,bridge.so']},
    'aarch64-macos': {
        'runs on': ('Darwin', 'arm64'), 'program': '', 'library': '.dylib', 'os': 'posix',
        'compile': ['-fPIC', '-D_DARWIN_C_SOURCE'], 'link': [],
        'beside': ['-Wl,-rpath,@loader_path'], 'named': ['-Wl,-install_name,@rpath/bridge.dylib']},
    'aarch64-linux-gnu': {
        'runs on': ('Linux', 'aarch64'), 'program': '', 'library': '.so', 'os': 'posix',
        'compile': ['-fPIC', '-D_DEFAULT_SOURCE'], 'link': ['-lm', '-lpthread', '-ldl'],
        'beside': ['-Wl,-rpath,$ORIGIN'], 'named': ['-Wl,-soname,bridge.so']},
    'x86_64-macos': {
        'runs on': ('Darwin', 'x86_64'), 'program': '', 'library': '.dylib', 'os': 'posix',
        'compile': ['-fPIC', '-D_DARWIN_C_SOURCE'], 'link': [],
        'beside': ['-Wl,-rpath,@loader_path'], 'named': ['-Wl,-install_name,@rpath/bridge.dylib']},
    'aarch64-windows-gnu': {
        'runs on': ('Windows', 'ARM64'), 'program': '.exe', 'library': '.dll', 'os': 'windows',
        'compile': [], 'link': [], 'beside': [], 'named': []},
}
TIMEOUT = 120

OS = ['memory', 'library', 'thread', 'mutex', 'event']

NATIVE_BACKEND = ['native_load', 'native_phase', 'native_backend', 'native_abi', 'native_module',
                  'native_instance', 'native_func']
# a module of the tests is a shared library: the name, its source, what it is compiled with
MODULES = [
    ('nmover', 'nmover', []), ('nflood', 'nflood', []), ('nforeign', 'nforeign', []),
    ('nsetter', 'nsetter', []), ('nwatcher', 'nwatcher', []),
    ('nstamp5', 'nstamp', ['VALUE=5', 'PHASE="update"']),
    ('nstamp9', 'nstamp', ['VALUE=9', 'PHASE="update"']),
    ('nsingle', 'nsingle', []), ('ntoggle', 'ntoggle', []), ('ntag', 'ntag', []),
    ('ndirect', 'ndirect', []), ('nsparse', 'nsparse', []),
    ('nlayout0', 'nlayout', ['V=0', 'KEPT=1']), ('nalloc', 'nalloc', []),
    ('nnoinit', 'nnoinit', []),
]

def die(msg):
    sys.stderr.write('build: %s\n' % msg)
    sys.exit(1)


def stale(outs, deps):
    if not all(o.exists() for o in outs):
        return True
    oldest = min(o.stat().st_mtime for o in outs)
    return any(d.stat().st_mtime > oldest for d in deps)


def step(what, outs, deps, commands):
    """Run `commands` if an output is missing or older than a dependency."""
    if not stale(outs, deps):
        return
    for o in outs:
        o.parent.mkdir(parents=True, exist_ok=True)
    for command in commands:
        if callable(command):
            command()
            continue
        done = subprocess.run([str(c) for c in command], stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT)
        if done.returncode:
            for o in outs:
                if o.exists():
                    o.unlink()
            # how it ended is said with what it said: a process the system ended says nothing
            raise RuntimeError('%s\n  %s\n  ended with %d (0x%08X)\n%s'
                               % (what, ' '.join(str(c) for c in command), done.returncode,
                                  done.returncode & 0xFFFFFFFF,
                                  done.stdout.decode('utf-8', 'replace')))
        if done.stdout.strip():
            sys.stdout.write(done.stdout.decode('utf-8', 'replace'))


def together(steps):
    """Steps that do not depend on each other, at once."""
    with concurrent.futures.ThreadPoolExecutor(os.cpu_count()) as pool:
        failed = [f.exception() for f in [pool.submit(step, *s) for s in steps] if f.exception()]
    if failed:
        die('\n'.join(str(f) for f in failed))


def tool(name, hints=()):
    exe = name + ('.exe' if os.name == 'nt' else '')
    for hint in hints:
        if (Path(hint) / exe).exists():
            return Path(hint) / exe
    found = shutil.which(name)
    return Path(found) if found else None


def take(folder):
    """The two packages of Bridge, from a folder that holds them: each file held against the
    hash its package states, and the commit of Bridge both were made from, which is one."""
    if not folder:
        die('name the folder of Bridge\'s packages: --bridge DIR')
    commits = set()
    for name in ('bridge', 'seams'):
        stated = folder / name / 'package.txt'
        if not stated.is_file():
            die('%s is not there: %s does not hold the package %s' % (stated, folder, name))
        for line in stated.read_text(encoding='utf-8').splitlines():
            head, _, rest = line.partition(' ')
            if head == 'commit':
                commits.add(rest)
            if len(head) == 64 and rest.strip():
                held = folder / name / rest.strip()
                if not held.is_file() or hashlib.sha256(held.read_bytes()).hexdigest() != head:
                    die('%s is not what the package %s states' % (held, name))
    if len(commits) != 1:
        die('the two packages of Bridge are of different commits: %s' % ', '.join(sorted(commits)))
    return commits.pop()


# What is handed. A package is a folder, and this is what each holds: a file of the tree, of
# the build or of Bridge's seams, and its name in the package where that differs.
def packages(out, lib, bridge):
    licence = (ROOT / 'license', 'license')
    return {
        'backend_native': [(out / ('bridge_native' + lib), 'bridge_native' + lib), licence],
        'sdk_native': [(bridge / 'seams' / h, h) for h in ('bridge_abi.h', 'bridge_schema.h')] +
                      [(f, f.name) for f in sorted((ROOT / 'sdk').iterdir())] + [licence],
    }


def commit():
    """The commit the tree is at, and whether the tree is that commit and nothing more."""
    def git(*words):
        return subprocess.run(['git'] + list(words), cwd=ROOT, capture_output=True, text=True).stdout.strip()
    changed = git('status', '--porcelain', '--untracked-files=no')
    return git('rev-parse', 'HEAD') + (' with changes that are not committed' if changed else '')


def write_packages(target, out, lib, bridge, ran):
    """Each package of a target, as a folder under packages/, with its statement: its name, the
    commit, the commit of Bridge it was built against, the target, whether the cases were run
    and passed on it, and the SHA-256 of each file."""
    at, against = commit(), take(bridge)
    for name, files in packages(out, lib, bridge).items():
        folder = out / 'packages' / name
        folder.mkdir(parents=True, exist_ok=True)
        said = ['package %s' % name, 'commit %s' % at, 'bridge %s' % against, 'target %s' % target,
                'run %s' % ('yes' if ran else 'no')]
        for source, called in files:
            data = source.read_bytes()
            if not (folder / called).exists() or (folder / called).read_bytes() != data:
                (folder / called).write_bytes(data)
            said.append('%s  %s' % (hashlib.sha256(data).hexdigest(), called))
        for stray in set(p.name for p in folder.iterdir()) - {c for _, c in files} - {'package.txt'}:
            (folder / stray).unlink()
        text = '\n'.join(said) + '\n'
        if not (folder / 'package.txt').exists() or (folder / 'package.txt').read_text() != text:
            (folder / 'package.txt').write_text(text, newline='\n')


def archive_packages(target, out):
    """Each package as one archive, named for the package and the target: what is published."""
    made = []
    for folder in sorted(p for p in (out / 'packages').iterdir() if p.is_dir()):
        path = out / 'packages' / ('%s-%s.tar.gz' % (folder.name, target))
        with tarfile.open(path, 'w:gz') as tar:
            for f in sorted(folder.iterdir()):
                tar.add(f, arcname='%s/%s' % (folder.name, f.name))
        made.append(path)
    return made


def build(target, debug, out, bridge):
    row = TARGETS[target]
    exe, lib = row['program'], row['library']
    zig = tool('zig') or die('zig is not on the path')
    take(bridge)
    obj = out / 'obj'
    cc = [zig, 'cc', '-target', target] + (['-O0', '-g'] if debug else ['-O2', '-DNDEBUG'])
    cc += row['compile']
    ours = cc + ['-std=c23', '-pedantic-errors', '-Wall', '-Wextra', '-Werror',
                 '-Wno-missing-field-initializers']
    system, here = row['link'], row['beside']
    # what is Bridge's is its two packages, and no path of its tree
    host_seam, seams = '-I%s' % (bridge / 'bridge'), '-I%s' % (bridge / 'seams')
    os_dir = ROOT / 'utils/os' / row['os']
    utils, sdk = '-I%s' % (ROOT / 'utils/os'), '-I%s' % (ROOT / 'sdk')
    stated = [bridge / name / 'package.txt' for name in ('bridge', 'seams')]
    headers = [Path(__file__)] + stated + [h for d in ('src', 'sdk', 'tests/*', 'tests/modules/*',
                                                       'utils/os', 'utils/os/' + row['os'])
                                           for h in ROOT.glob(d + '/*.h')]
    defines = ['-D%s="%s"' % (key, path.as_posix()) for key, path in
               [('NATIVE_PATH', out / ('bridge_native' + lib))] +
               [('%s_PATH' % name.upper(), out / 'modules' / (name + lib)) for name, _, _ in MODULES]]
    compiles = []

    def compile_(name, source, flags):
        compiles.append(('compiling %s' % name, [obj / (name + '.o')], [source] + headers,
                         [flags + ['-c', source, '-o', obj / (name + '.o')]]))
        return obj / (name + '.o')

    # the backend: the backend seam, the operating system's layer, and what is its own
    backend_o = [compile_(name, ROOT / 'src' / (name + '.c'), ours + [seams, utils, sdk])
                 for name in NATIVE_BACKEND] + \
                [compile_('os-' + name, os_dir / (name + '.c'), ours + [utils]) for name in OS]
    # a host of the tests is compiled against bridge.h and linked with the library, as a host is
    support = '-I%s' % (ROOT / 'tests/support')
    host_o = [compile_('host-' + c.stem, c, ours + [host_seam, seams, support] + defines)
              for c in sorted((ROOT / 'tests/host').glob('*.c')) +
              sorted((ROOT / 'tests/support').glob('host*.c'))]
    shared = '-I%s' % (ROOT / 'tests/modules')
    module_o = {name: [compile_('module-%s-%s' % (name, c.stem), c,
                                ours + [seams, sdk, shared, '-fvisibility=hidden'] +
                                ['-D' + d for d in flags])
                       for c in sources(ROOT / 'tests/modules', source, 'nidle', '"native_idle.h"')]
                for name, source, flags in MODULES}
    # the program that calls the backend as the library does: the backend seam, the layer over
    # the operating system, and a loader of its own
    direct_o = [compile_('direct-' + c.stem, c, ours + [seams, utils] + defines)
                for c in sorted((ROOT / 'tests/direct').glob('*.c'))] + \
               [obj / ('os-%s.o' % name) for name in OS]
    together(compiles)

    links = []

    def link(name, objects, flags=(), shared=False):
        target_file = out / (name + (lib if shared else exe))
        links.append(('linking %s' % name, [target_file], list(objects),
                      [cc + (['-shared'] if shared else []) + list(objects) + list(flags) +
                       ['-o', target_file]]))

    link('bridge_native', backend_o, system, shared=True)
    for name, _, _ in MODULES:
        link('modules/' + name, module_o[name], shared=True)
    # the library is put beside the host, which finds it there
    beside = out / ('bridge' + lib)
    step('putting the library beside the host', [beside], [bridge / 'bridge' / beside.name],
         [lambda: shutil.copyfile(bridge / 'bridge' / beside.name, beside)])
    link('host', host_o + [bridge / 'bridge' / beside.name], system + here)
    link('direct', direct_o, system)
    together(links)
    write_packages(target, out, lib, bridge, False)
    return {'python': sys.executable, 'exe': exe, 'bridge': bridge}


def sources(where, name, idle, said='_idle.h"'):
    """A module's sources: the one file of its name, or every C file of the directory of its
    name - docs/style.md, O3, allows five functions a file, and a module may export more. A
    module that includes the idle header of its backend states that its shutdown and its reload
    do nothing, and is built with the source that has them."""
    one = where / (name + '.c')
    found = [one] if one.is_file() else sorted((where / name).glob('*.c'))
    stated = any(said in c.read_text(encoding='utf-8') for c in found)
    return found + ([where / (idle + '.c')] if stated else [])


def command_line(out, built, command, kept):
    """What a case runs: a program of the build, or a tool of ours over the tree."""
    if command[0] == 'package':
        return [str(built['python']), str(ROOT / 'tools/package.py'), str(out), str(built['bridge'])]
    if command[0] == 'cla':
        return [str(built['python']), str(ROOT / 'tools/cla.py'), '--decide'] + command[1:]
    if command[0] in ('c23', 'style', 'cover', 'xref', 'licence'):
        return [str(built['python']), str(ROOT / ('tools/%s.py' % command[0]))]
    if command[0] == 'said':
        return [str(built['python']), str(ROOT / 'tools/said.py'), str(kept)]
    if command[0] == 'deps':
        return [str(built['python']), str(ROOT / 'tools/deps.py'), str(out), '--check']
    return [str(out / (command[0] + built['exe']))] + command[1:]


def run(argv, out):
    """One copy of a case: what it said, and how it ended."""
    try:
        done = subprocess.run(argv, cwd=out, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                              timeout=TIMEOUT)
        return done.stdout.decode('utf-8', 'replace'), done.returncode
    except subprocess.TimeoutExpired as late:
        return (late.stdout or b'').decode('utf-8', 'replace'), 'timeout'


def judge(said, code, match, never):
    """Why a copy of a case failed, or None. With a pattern, the pattern decides and the exit
    code does not: several cases are a refusal that ends the program. Without one, the exit
    code does."""
    if code == 'timeout':
        return 'did not end in %d seconds' % TIMEOUT
    if never and re.search(never, said, re.S):
        return 'said what it must not: %s' % never
    if match and match.startswith('@') and \
            said.replace('\r', '') != (ROOT / match[1:]).read_text(encoding='utf-8'):
        return 'did not say what %s holds, word for word' % match[1:]
    if match and not match.startswith('@') and not re.search(match, said, re.S):
        return 'ended with %s and did not say: %s' % (code, match)
    if not match and code:
        return 'ended with %s' % code
    return None


def test(out, built, only, copies):
    """Each case by what it said. With copies, each case is started that many times at once,
    and every copy is judged as a case is: a case is a process, and the copies compete for the
    processors, for memory, and for what a process shares with its neighbours."""
    cases = [c for c in CASES if not only or only in c[0]]
    failed = []
    # what every case said is kept, a file a case: tools/said.py holds each message of the
    # source against it. A run of every case begins with none, so that no case that is gone
    # holds a message
    kept = out / 'said'
    if not only:
        shutil.rmtree(kept, ignore_errors=True)
    kept.mkdir(exist_ok=True)
    for name, command, match, never in cases:
        argv = command_line(out, built, command, kept)
        count = 1 if name in ALONE else copies
        with concurrent.futures.ThreadPoolExecutor(count) as pool:
            ran = list(pool.map(lambda _: run(argv, out), range(count)))
        (kept / (name + '.txt')).write_text(ran[0][0], encoding='utf-8')
        wrong = [(said, why) for said, why in ((s, judge(s, c, match, never)) for s, c in ran) if why]
        if not wrong:
            print('ok    %s' % name)
            continue
        print('FAIL  %s: %s, in %d of %d\n%s' % (name, wrong[0][1], len(wrong), count, wrong[0][0]))
        failed.append(name)
    print('%d of %d passed' % (len(cases) - len(failed), len(cases)))
    if failed:
        die('failed: %s' % ' '.join(failed))
    if not cases:
        die('no test is named like %r' % only)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('command', nargs='?', default='build', choices=['build', 'test', 'package'])
    ap.add_argument('--bridge', type=Path, metavar='DIR', help='the folder of Bridge\'s two packages')
    ap.add_argument('--target', choices=sorted(TARGETS))
    ap.add_argument('--debug', action='store_true')
    ap.add_argument('--out', type=Path, help='where to build: build/<target> beside this file')
    ap.add_argument('-k', metavar='NAME', help='only the cases whose name has this in it')
    ap.add_argument('--copies', type=int, default=1, metavar='N',
                    help='start each case N times at once, and judge every copy')
    ap.add_argument('--rounds', type=int, default=1, metavar='N', help='run the cases N times')
    args = ap.parse_args()
    machine = (platform.system(), platform.machine())
    mine = next((t for t, row in TARGETS.items() if row['runs on'] == machine), None)
    target = args.target or mine or die('no target runs on %s %s: name one' % machine)
    if args.command == 'test' and target != mine:
        die('%s cannot be run here: build it, and test it where it runs' % target)
    bridge = args.bridge.resolve() if args.bridge else None
    out = (args.out or ROOT / 'build' / (target + ('-debug' if args.debug else ''))).resolve()
    built = build(target, args.debug, out, bridge)
    if args.command == 'package':
        if target == mine:
            test(out, built, None, 1)
        write_packages(target, out, TARGETS[target]['library'], bridge, target == mine)
        for path in archive_packages(target, out):
            print(path)
    if args.command == 'test':
        for _ in range(args.rounds):
            test(out, built, args.k, args.copies)


# Every case: its name, what is run, what its output must match, what it must not. A case ends
# in "alive": checked by what was said, and by the process still being there to say it.
TALLY = r"game:mover@0.1.0/tally: native tally 2 1000.*native tally 2 1000.*alive"
SEEN = r"b2 game:setter@0.1.0/mark: marked.*b2 view:watcher@0.1.0/seen: seen 1 77.*alive"
CASES = [(name, ['host', name], match, never) for name, match, never in [
    ('BRIDGE_NATIVE-system-reads-and-writes', TALLY, None),
    ('BRIDGE_NATIVE_COMPONENT-declared', TALLY, None),
    ('BRIDGE_NATIVE_ADD_SYSTEM-registered-and-run', TALLY, None),
    ('bridge_native_get_query-the-query-of-a-system', TALLY, None),
    ('bridge_native_get_span-two-tables-walked', TALLY, None),
    ('bridge_native_log-a-line-with-two-numbers', TALLY, None),
    ('BRIDGE_NATIVE_ATTACH-seen-by-an-observer', SEEN, None),
    ('bridge_native_lend-a-value-lent-to-attach', r"seen 1 77.*alive", None),
    ('BRIDGE_NATIVE_PUT_CHANGES-changes-stated-and-made', r"seen 1 9.*alive", r"seen 1 5"),
    ('bridge_api_t-entry-from-a-foreign-thread-refused',
     r"test:foreign@0.1.0/reach: an id from another thread, and from this one 0 1.*alive", None),
    ('call-an-entry-refused-fails-the-invocation',
     r"test:flood@0.1.0/flood: system 'flood' failed: spawn: its spawns of 1024 between two "
     r"barriers are used up.*alive", None),
    ('backend_get_capabilities-untrusted-refused',
     r"module 'game:mover@0.1.0' is refused: it is not trusted, and backend 'native' runs only "
     r"modules that are.*alive", r"cannot open"),
    ('backend_get_capabilities-work-limit-refused',
     r"is refused: it states work_limit, which backend 'native' cannot keep.*alive", r"cannot open"),
    ('backend_get_capabilities-memory-limit-refused',
     r"is refused: it states memory_limit, which backend 'native' cannot keep.*alive", r"cannot open"),
    ('backend_get_capabilities-timeout-refused',
     r"is refused: it states shutdown_timeout, which backend 'native' cannot keep.*alive",
     r"cannot open"),
    ('module_compile-no-path-refused',
     r"did not load: this backend opens a module's library by its path, and was handed none.*alive",
     None),
    ('module_compile-library-cannot-open',
     r"did not load: cannot open the library 'no-such-library'.*alive", None),
    ('backend_create-a-setting-refused',
     r"backend 'native' would not start: it knows no setting 'cache'.*bridge_create refused.*alive",
     None),
    ('bridge_native_get_single-the-one-row',
     r"it has no value 0 0.*it has a value: gravity, wind 10 1.*wind 20 2.*wind 20 77.*alive", None),
    ('bridge_native_get_bits-read-and-turned',
     r"by words: rows, and of them on 6 6.*flicker.*rows, and of them on 6 3.*"
     r"look_b: by rows: on, and their sum 3 39.*find: by id: alive, and of them on 6 3.*alive", None),
    ('BRIDGE_NATIVE_DECLARE_TAG-carried-with-no-bytes',
     r"look: M, in tables: rows that carry it, and spans whose base says carried 3 1.*"
     r"P, by its index: entities whose cell says carried 2 0.*R, direct-indexed: entities given it 3 0"
     r".*alive", None),
    ('bridge_native_get_cells-reached-by-an-id',
     r"look_b: the table walked: rows, and of them given D 6 3.*"
     r"near: by id: alive, and of them given D 6 3.*near: by id: the sum of D 306 0.*alive", None),
    ('bridge_native_get_sparse_index-reached-by-an-id',
     r"look: the set walked: spans, rows 1 3.*the sum of its values 306 0.*"
     r"look_b: the table walked: rows, and of them in the set 6 3.*alive", None),
    ('BRIDGE_NATIVE_DECLARE-an-enum-a-struct-names',
     r"st:store@0.1.0: loaded.*look: values, and the sum of a 3 3003.*alive", r"not loaded"),
    ('bridge_native_get_list-triggers-of-an-observer', SEEN, None),
    ('BRIDGE_NATIVE_OBSERVER-registered-and-told', SEEN, None),
    ('bridge_native_get_header-read-through-a-pointer', SEEN, None),
    ('BRIDGE_NATIVE_STATE-a-statement-put-in-scratch', TALLY, None),
    ('bridge_native_put-bytes-copied-into-scratch', TALLY, None),
    ('bridge_line_t-a-line-with-its-two-numbers', TALLY, None),
    ('declare-two-components-declared', TALLY, None),
    ('add_system-registered-and-run', TALLY, None),
    ('spawn-a-thousand-made-at-init', TALLY, None),
    ('observe-told-of-an-attach', SEEN, None),
    ('attach-a-value-given-again',
     r"value_b.*heard_d: heard of D: entity, events 3 2.*near: by id: the sum of D 1104 0.*alive", None),
    ('detach-a-component-taken',
     r"detach_b_half.*heard_d: heard of D: entity, events 1 4.*rows, and of them given D 6 2.*alive", None),
    ('destroy-two-entities-gone',
     r"destroy_half.*rows, and of them given D 4 1.*near: by id: alive, and of them given D 4 1.*alive",
     None),
    ('alloc-bytes-kept-from-init', r"say: kept 1 42.*alive", None),
    ('instance_get_func-no-such-export-refused',
     r"module 'test:noinit@0.1.0' did not load: no exported function 'bridge_init'.*alive", None),
]] + [
    ('define_func-an-entry-the-abi-has-not-refused', ['direct'],
     r"an entry the SDK has not is refused: the native SDK has no entry 'teleport'", None),
]
# the tree, held: each tool of ours is a case, and is run once however many copies are asked for
CASES += [
    ('c23', ['c23'], None, None),
    ('style', ['style'], None, None),
    ('deps', ['deps'], None, None),
    ('said', ['said'], None, None),
    ('cover', ['cover'], None, None),
    ('xref', ['xref'], None, None),
    ('licence', ['licence'], None, None),
    ('cla-the-holder-is-not-asked', ['cla', 'holder', 'holder', ''], '^nothing, passes: the holder', None),
    ('cla-a-name-on-record-passes', ['cla', 'ann', 'holder', 'bob,ann'], '^nothing, passes: ann has signed', None),
    ('cla-anybody-else-is-asked-and-fails', ['cla', 'ann', 'holder', 'bob'], '^ask, fails: ann has not signed', None),
    ('cla-the-sentence-by-its-author-is-recorded', ['cla', 'ann', 'holder', '', 'ann', 'I have read cla.md and I sign it.'], '^record, passes: ann signs', None),
    ('cla-the-sentence-by-another-is-not-taken', ['cla', 'ann', 'holder', '', 'bob', 'I have read cla.md and I sign it.'], '^ask, fails', None),
    ('cla-other-words-are-not-taken', ['cla', 'ann', 'holder', '', 'ann', 'I sign, more or less.'], '^ask, fails', None),
    ('package', ['package'], None, None),
]
ALONE = ('c23', 'style', 'deps', 'said', 'cover', 'xref', 'licence', 'cla-the-holder-is-not-asked', 'cla-a-name-on-record-passes', 'cla-anybody-else-is-asked-and-fails', 'cla-the-sentence-by-its-author-is-recorded', 'cla-the-sentence-by-another-is-not-taken', 'cla-other-words-are-not-taken', 'package')


if __name__ == '__main__':
    main()
