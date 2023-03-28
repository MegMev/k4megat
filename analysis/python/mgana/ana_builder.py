#!/usr/bin/env python3

def replace_all(input: str, repl) -> str:
    output = input
    for a, b in repl.items():
        output = output.replace(a, b)
    return output

def setup_analysis(package: str,
                   name: str='',
                   output_dir: str=''):
    import os
    assert (megat_path := os.getenv('MEGAT_ROOT')), "source thismegat.sh first"

    replacement_dict = {
        '__pkgname__': package,
        '__name__': name,
        '__megatpath__': megat_path
    }

    #
    if not output_dir:
        path = f'{os.getcwd()}/{package}'
    else:
        path = output_dir

    for p in [path, f'{path}/src', f'{path}/include', f'{path}/scripts']:
        try:
            os.makedirs(p)
        except FileExistsError:
            print(f'Warning: MegatAnalyzer package "{package}" already exists.')
            pass
    try:
        tmpl_dir = os.path.join(megat_path, 'python/mgana/templates')
        with open(f'{path}/src/classes.h', 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/classes.h', 'r').read(), replacement_dict))
        with open(f'{path}/src/classes_def.xml', 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/classes_def.xml', 'r').read(), replacement_dict))
        with open(f'{path}/src/{name}.cc', 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/Package.cc', 'r').read(), replacement_dict))
        with open(f'{path}/include/{name}.h', 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/Package.h', 'r').read(), replacement_dict))
        with open(f'{path}/scripts/analysis_cfg.py', 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/analysis_cfg.py', 'r').read(), replacement_dict))
        with open(f'{path}/CMakeLists.txt', 'w') as f:
            f.write(replace_all(open(f'{tmpl_dir}/CMakeLists.txt', 'r').read(), replacement_dict))
    except OSError as error:
        print(f'MegatAnalyzer package "{package}" creation error:')
        print(error)
