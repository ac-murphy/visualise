from pathlib import Path
import sys
import argparse

# boilerplate to ensure local files are visible:
SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))


if __name__ == "__main__":
    # 1. read command line arguments:
    # a) here we account for the '--' argument separator for using blender.
    argv = sys.argv[1:]
    if '--' in argv: argv = argv[argv.index('--') + 1:]
    # b) initialise the parser:
    parser = argparse.ArgumentParser()
    parser.add_argument('--mode', type=str)
    parser.add_argument('--input', type=Path)
    args = parser.parse_args(argv)

    # 2. decide whether to use matplotlib or blender
    match args.mode:
        case '3D':
            import visualise_3d
            visualise_3d.main(args)
        case '2D':
            import visualise_2d
            visualise_2d.main(args)
        case _:
            raise ValueError(f'__main__ \"unsupported mode \'{args.mode}\'\"')