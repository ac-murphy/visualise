from pathlib import Path
import numpy as np

def read(path: Path, *, n_cols: int, dtype: np.dtype):
    path = path.with_suffix('.bin')
    data = np.fromfile(path, dtype=dtype).reshape(-1, n_cols)

    # if dataset is 1D it does not make sense to return a 2D array:
    return data.T[0] if n_cols == 1 else data
