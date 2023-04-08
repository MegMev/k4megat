import shlex

from .utility import parse_meta, current_mgana_path, get_absolute_directory
from .utility import execute, setup_megat

# api function
def setup_build_parser(parser):
    '''
    Setup build command arguments
    '''
    publicOptions = parser.add_argument_group('User options')
    publicOptions.add_argument('--config',
                               help='config current package', action='store_true')
    publicOptions.add_argument('--build',
                               help='compile and install current package', action='store_true')
    publicOptions.add_argument('--clean',
                               help='clean the build directory of  current package', action='store_true')

def build_analysis(mainparser):
    args, _ = mainparser.parse_known_args()

    #
    flags = [args.config, args.build, args.clean]
    is_all = False if any(flags) else True
    if args.clean:
        is_all = False

    # setup megat env
    setup_megat()

    # get running env config
    pkg_dir = current_mgana_path()
    envs = parse_meta(get_absolute_directory('.mgana/env.json', pkg_dir))

    # create cmd list
    cmds = {
        "pre-build": f"cmake -B {envs['build']} -S {pkg_dir}",
        "build": f"cmake --build {envs['build']} -j 4",
        "install": f"cmake --install {envs['build']}",
        "clean": f"cmake --build {envs['build']} --target clean"
    }

    if args.clean:
        execute(shlex.split(cmds['clean']))
        print(f'----> clean done')
    if args.config or is_all:
        execute(shlex.split(cmds['pre-build']))
        print(f'----> pre-build done')
    if args.build or is_all:
        execute(shlex.split(cmds['build']))
        print(f'----> compiling done')
        execute(shlex.split(cmds['install']))
        print(f'----> intallation done')
