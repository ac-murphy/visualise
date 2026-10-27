from pathlib import Path
import sys
import argparse

# boilerplate to ensure local files are visible:
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

import system.json as json

if __name__ == "__main__":
    # 1. read command line arguments:
    # a) here we account for the '--' argument separator for using blender:
    argv = sys.argv[1:]
    if '--' in argv: argv = argv[argv.index('--') + 1:]
    # b) initialise the parser:
    parser = argparse.ArgumentParser()
    parser.add_argument('--mode', type=str)
    parser.add_argument('--input', type=Path)
    parser.add_argument('--settings', type=Path)
    args = parser.parse_args(argv)

    # 2. run specified visualise mode (if possible):
    match args.mode:
        case 'geometry':
            import geometry_main
            geometry_main.main(args.input, args.settings)
        case 'plotting':
            import plotting_main
            plotting_main.main(args.input, args.settings)
        case _:
            raise ValueError(f'__main__ \"unsupported mode \'{args.mode}\'\"')