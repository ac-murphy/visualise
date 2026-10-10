from pathlib import Path
import sys

from numpy.f2py.auxfuncs import throw_error

# boilerplate to ensure local files are visible:
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

import argparse
import sys
import numpy as np
import visualise_3d

__functions__ = {}
__points__ = np.array([])


if __name__ == "__main__":
    # 1. read command line arguments:
    # here we account for the '--' argument separator for using blender.
    argv = sys.argv
    if '--' in argv: argv = argv[argv.index('--') + 1:]

    # initialise the parser:
    parser = argparse.ArgumentParser()
    parser.add_argument('--mode', type=str)
    parser.add_argument('--input', type=Path)
    args = parser.parse_args(argv)

    # 2. decide whether to use matplotlib or blender
    match args.mode:
        case '3D':
            visualise_3d.run_with(args)
        case _:
            raise ValueError(f'__main__ \"unsupported mode \'{args.mode}\'\"')