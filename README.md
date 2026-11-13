# Star Tracker

C++ core (math, simulation, identification) + Python tools (catalog prep, plots).
They communicate through CSV files. See `docs/` for conventions and notes.

## Build
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build --output-on-failure
```
Use `-DCMAKE_BUILD_TYPE=Release` for timing experiments (debug timings are meaningless).

Eigen and Catch2 are found on the system if installed, otherwise downloaded automatically (needs internet on first configure).

## Python
```
python -m venv .venv && source .venv/bin/activate
pip install -r python/requirements.txt
python python/prepare_catalog.py
```

## Run
```
./build/check_catalog data/catalog.csv
```

