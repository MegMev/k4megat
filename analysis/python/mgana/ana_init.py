import os
import shlex

from .utility import expand_absolute_directory, get_absolute_directory, replace_all
from .utility import execute
from .logger import rootLogger

def setup_analysis(package: str,
                   name: str='',
                   output_dir: str='',
                   work_dir: str='',
                   script_dir: str='',
                   build_dir: str='',
                   install_dir: str='',
                   megat_root: str=''):
    '''
    Setup the analysis directories and create file templates.
    '''
    # create the top-level directory
    if not output_dir:
        path = os.path.abspath('./')
    else:
        output_dir = expand_absolute_directory(output_dir)
        path = get_absolute_directory(package, output_dir)

    if not os.path.exists(path):
        os.mkdir(path)

    # create dict of metadata
    if megat_root:
        megat_root = expand_absolute_directory(megat_root)
        setup_script = get_absolute_directory('bin/thismegat.sh', megat_root)
        execute(shlex.split(f'source {setup_script}'))

    assert os.getenv('MEGAT_ROOT'), "Can't find k4megat installation:\nsource thismegat.sh or pass in k4megat root directory using --megat-root command option"
    megat_root = os.getenv('MEGAT_ROOT')

    replacement_dict = {
        '__pkgname__': package,
        '__name__': name,
        '__megat_path__': megat_root,
        '__script_path__': get_absolute_directory(script_dir, path),
        '__work_path__': get_absolute_directory(work_dir, path),
        '__build_path__': get_absolute_directory(build_dir, path),
        '__install_path__': get_absolute_directory(install_dir, path),
    }

    # create basic sub-directories and template files
    sub_dirs = ['include', 'src', '.mgana']
    for p in sub_dirs:
        try:
            os.makedirs(get_absolute_directory(p, path))
        except FileExistsError:
            rootLogger.warning(f'{package}: {get_absolute_directory(p, path)} already exists.')
            pass

    # create optional sub-directories
    sub_dirs = [work_dir, script_dir, build_dir, install_dir]
    for p in sub_dirs:
        try:
            os.makedirs(get_absolute_directory(p, path))
        except FileExistsError:
            rootLogger.warning(f'{package}: {get_absolute_directory(p, path)} already exists.')
            pass

    # create default files from templates
    try:
        tmpl_dir = os.path.join(megat_root, 'python/mgana/templates')
        with open(f'{path}/src/dict.h', 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/dict.h', 'r').read(), replacement_dict))
        with open(f'{path}/src/dict_def.xml', 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/dict_def.xml', 'r').read(), replacement_dict))
        with open(f'{path}/src/{name}.cc', 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/Package.cc', 'r').read(), replacement_dict))
        with open(f'{path}/include/{name}.h', 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/Package.h', 'r').read(), replacement_dict))
        with open(get_absolute_directory(f'{script_dir}/analysis_stage1.py', path), 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/analysis_stage1.py', 'r').read(), replacement_dict))
        with open(get_absolute_directory(f'{script_dir}/analysis_final.py', path), 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/analysis_final.py', 'r').read(), replacement_dict))
        with open(get_absolute_directory(f'{script_dir}/analysis_plot.py', path), 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/analysis_plot.py', 'r').read(), replacement_dict))
        with open(f'{path}/CMakeLists.txt', 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/CMakeLists.txt', 'r').read(), replacement_dict))
        with open(f'{path}/.mgana/env.json', 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/env.json', 'r').read(), replacement_dict))
    except OSError as error:
        print(f'MegatAnalyzer package "{package}" creation error:')
        print(error)

# api function
def setup_init_parser(parser):
    '''
    Setup init command arguments
    '''
    publicOptions = parser.add_argument_group('User options')
    publicOptions.add_argument('package', help='name of the analysis package to be built')
    publicOptions.add_argument('--name', help='name of the main analysis utility', default='DummyAnalysis')
    publicOptions.add_argument('--out-dir', help='output directory where the analysis package will be written')
    publicOptions.add_argument('--work-dir', help='an optional directory for analysis output', default='workspace')
    publicOptions.add_argument('--script-dir', help='the default directory for analysis job scripts', default='script')
    publicOptions.add_argument('--build-dir', help='build directory',default='.build')
    publicOptions.add_argument('--install-dir', help='build directory',default='.install')
    publicOptions.add_argument('--megat-root', help='root directory of k4megat')

def init_analysis(mainparser):
    args, _ = mainparser.parse_known_args()
    setup_analysis(package=args.package,
                   name=args.name,
                   output_dir=args.out_dir,
                   work_dir= args.work_dir,
                   script_dir= args.script_dir,
                   build_dir= args.build_dir,
                   install_dir= args.install_dir,
                   megat_root = args.megat_root
                   )
