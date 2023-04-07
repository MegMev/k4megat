import os,sys
import subprocess, shlex
import json

from .logger import rootLogger

def parse_meta(infile):
    with open(infile, 'r') as f:
        data = json.load(f)
    return data

def dump_meta(outfile: str,
              data: str):
    with open(outfile, 'w') as f:
        json.dump(data, f, indent=4)

def _find_containing_dir(tgt_file: str='.mgana',
                         cur_dir: str=''):
    '''
    Scan upwards through the containing the directories, starting from cur_dir.
    Return the the first (aka the nearest) one containing the tgt_file.
    '''
    contain_dir = cur_dir
    rootLogger.debug(cur_dir)
    if not os.path.exists(os.path.join(cur_dir, tgt_file)):
        parent_dir = os.path.dirname(cur_dir)
        if parent_dir == cur_dir:
            raise FileNotFoundError(f"Can't find {tgt_file}")
        # assert parent_dir != cur_dir, f"Can't find {tgt_file} ({parent_dir} vs {cur_dir})"
        contain_dir = _find_containing_dir(tgt_file, parent_dir)
    return contain_dir

def current_mgana_path() -> str:
    try:
        path = _find_containing_dir(tgt_file='.mgana', cur_dir=os.getcwd())
        return path
    except FileNotFoundError as e:
        rootLogger.exception(f"Can not .mgana find in any parent directory of {os.getcwd()}\n\n")
        sys.exit(1)

def replace_all(input: str, repl) -> str:
    '''
    Utility to replace each appearance of corresponding variable in
    each item of repl dictionary.
    '''
    output = input
    for a, b in repl.items():
        output = output.replace(a, b)
    return output


def expand_absolute_directory(path: str) -> str:
    '''
    Expand path for absolute one. Append to current directory for relative one.
    '''
    return os.path.abspath(os.path.expanduser(path))

def get_absolute_directory(dir_name: str,
                           top_dir: str) -> str:
    '''
    Return a directory expanded for absolute dir_name; appended to customized top_dir for relative dir_name
    '''
    abs_dir = os.path.expanduser(dir_name)
    if os.path.abspath(dir_name) != dir_name:
        abs_dir = os.path.join(top_dir,dir_name)
    return abs_dir

def is_absolute_path(path: str) -> bool:
    abs_path = expand_absolute_directory(path)
    return True if abs_path == os.path.expanduser(path) else False

def execute(command: list[str],
            working_dir=None,
            env=None,
            is_print: bool=False) -> str:
    if working_dir:
        working_dir = os.path.abspath(os.path.expanduser(working_dir))
    p = subprocess.run(command,
                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                       cwd=working_dir, universal_newlines=True, text=True,
                       check=True, env=env)
    if is_print: print(p.stdout)
    return p.stdout

class Subprocess:
    ''''''
    def __init__(self, command, working_dir=None, capture_stderr=True, env=None):
        """Changes into a specified directory, if provided, and executes a command.

        Restores the old directory afterwards.

        Args:
          command:        The command to run, in the form of sys.argv.
          working_dir:    The directory to change into.
          capture_stderr: Determines whether to capture stderr in the output member
                          or to discard it.
          env:            Dictionary with environment to pass to the subprocess.

        Returns:
          An object that represents outcome of the executed process. It has the
          following attributes:
          terminated_by_signal   True if and only if the child process has been
                                 terminated by a signal.
          exited                 True if and only if the child process exited
                                 normally.
          exit_code              The code with which the child process exited.
          output                 Child process's stdout and stderr output
                                 combined in a string.
        """
        if capture_stderr:
            stderr = subprocess.STDOUT
        else:
            stderr = subprocess.PIPE

        if working_dir:
            working_dir = os.path.abspath(os.path.expanduser(working_dir))

        p = subprocess.Popen(command,
                             stdout=subprocess.PIPE, stderr=stderr,
                             cwd=working_dir,
                             universal_newlines=True, text=True,
                             env=env)

        # communicate returns a tuple with the file object for the child's output.
        self.output = p.communicate()[0]
        self._return_code = p.returncode
        if bool(self._return_code & 0x80000000):
            self.terminated_by_signal = True
            self.exited = False
        else:
            self.terminated_by_signal = False
            self.exited = True
            self.exit_code = self._return_code

def setup_environment(setup_script: str):
    '''
    Update the run environment of current process by invoking setup_script (shell script)
    '''
    import itertools
    stdout = execute(shlex.split(f"mgenv_wrapper {setup_script}"))
    items_iter = iter(stdout.split('\0'))

    rootLogger.debug(f'Setup new environment from {setup_script}:')
    for (k,v) in itertools.zip_longest(items_iter, items_iter):
        if k:
            os.environ[k]=v
            rootLogger.debug(f'{k} = {os.getenv(k)}')

def setup_megat():
    '''
    Update k4megat run environment
    '''
    # get current run environ
    pkg_dir = current_mgana_path()
    envs = parse_meta(get_absolute_directory('.mgana/env.json', pkg_dir))
    old_root = os.getenv('MEGAT_ROOT')

    # update run environ from current pkg metadata
    setup_script = get_absolute_directory('bin/thismegat.sh', envs['megat'])
    setup_environment(setup_script)
    new_root = os.getenv('MEGAT_ROOT')

    # check the updated envrion is effective and no conflict
    assert new_root, f"Can't setup k4megat installation:\n\n {setup_script} not effective"
    if old_root and old_root != new_root:
        rootLogger.warning(f'''
        Unmatched megat installation path:
        current environment: {old_root}
        current package config: {new_root}
        This may cause a potential conflict!
        ''')

def megat_geometry_path() -> str:
    pkg_dir = current_mgana_path()
    envs = parse_meta(get_absolute_directory('.mgana/env.json', pkg_dir))
    return os.path.join(envs['megat'],'geometry/compact')

def mgana_lib_path() -> str:
    pkg_dir = current_mgana_path()
    envs = parse_meta(get_absolute_directory('.mgana/env.json', pkg_dir))
    return os.path.join(envs['install'],'lib')

def mgana_script_path() -> str:
    pkg_dir = current_mgana_path()
    envs = parse_meta(get_absolute_directory('.mgana/env.json', pkg_dir))
    return envs['script']

def mgana_script_path() -> str:
    pkg_dir = current_mgana_path()
    envs = parse_meta(get_absolute_directory('.mgana/env.json', pkg_dir))
    return envs['workspace']
