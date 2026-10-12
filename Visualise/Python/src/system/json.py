from pathlib import Path
import json

def read(path: Path):
    # return None if path does not exist:
    path = path.with_suffix('.json')
    if not path.exists():
        return None

    # read json:
    with open(path) as data_in:
        return json.load(data_in)

def write(data: dict, path: Path):
    # automatically create filepath if not already existing:
    path = path.with_suffix('.json')
    path.parent.mkdir(parents=True, exist_ok=True)

    # write json:
    with open(path) as data_out:
        json.dump(data, data_out, indent=4)