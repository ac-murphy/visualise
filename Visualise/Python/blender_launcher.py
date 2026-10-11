from pathlib import Path
import argparse
import subprocess
import sys

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--python', type=Path)
    args, _ = parser.parse_known_args(sys.argv)
    exe_dir = args.python.parent/'dist/Visualise'

    print(exe_dir)

    forward_args = sys.argv[sys.argv.index('--') + 1:]
    subprocess.run(
        [exe_dir/'Visualise.exe', *forward_args],
        check=True
    )