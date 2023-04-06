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

def execute(command: list[str],
            working_dir=None,
            env=None,
            is_print: bool=False):
    if working_dir:
        working_dir = os.path.abspath(os.path.expanduser(working_dir))
    p = subprocess.run(command,
                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                       cwd=working_dir, universal_newlines=True, text=True,
                       check=True, env=env)
    if is_print:
        print(p.stdout)

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

# todo: can't source in Popen and the environment not passed
def setup_megat():
    pkg_dir = current_mgana_path()
    envs = parse_meta(get_absolute_directory('.mgana/env.json', pkg_dir))
    old_root = os.getenv('MEGAT_ROOT')
    setup_script = get_absolute_directory('bin/thismegat.sh', envs['megat'])
    execute(shlex.split(f'source {setup_script}'))
    new_root = os.getenv('MEGAT_ROOT')
    assert new_root, f"Can't setup k4megat installation:\n\n {setup_script} not effective"

    if old_root and old_root != new_root:
        rootLogger.warning(f'''
        Unmatched megat installation path:
        current environment: {old_root}
        current package config: {new_root}
        This may cause a potential conflict!
        ''')
