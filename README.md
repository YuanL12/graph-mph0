# graph-mph0
This is a project about computing absolute betti numbers at dimension 0 for a bi-filtered graph. 

## User Guide 
### Install 
Clone the repo and corresponding all submodules
```shell
git clone git@github.com:YuanL12/graph-mph0.git
cd graph-mph0/
git submodule update --init --recursive
```

In you local repository, build this C++ project
```Shell
mkdir build && cd build
cmake ..
make -j4
```
then you will have the executable `main`.

Install the Python package `graph_mph` if you want to run the notebooks under `tutorial` folder.
```Shell
pip install pybind11 # this is necessary 
pip install .
```
Uninstall you don't want to use our package any more
```bash
pip uninstall graph_mph
```

If you prefer to use uv, here is a full installation command lines that has been tested working on a linux machine 
```bash
uv venv --python 3.13
source .venv/bin/activate
uv pip install ipykernel matplotlib
uv pip install .
``` 
That's all you need to run the jupyter notebook `compute_mph_firep.ipynb` under tutorial folder. 

### Input
We support the following two types input
1. a point cloud
2. a bifiltration in the format of `firep`:
```bash
firep
first parameter
second parameter
#_of_edges #_of_vertices 0 
grade_x_of_e_0 grade_y_of_e_0 ; boundary_vertex_0_of_e0 boundary_vertex_1_of_e0
...
grade_x_of_v_0 grade_y_of_v_0 ;
...
```
Example from the figure one of "Computing Betti Tables and Minimal Presentations
of Zero-dimensional Persistent Homology"
```bash
firep
first parameter
second parameter
8 6 0
3 5 ; 0 1 
5 3 ; 0 1 
6 6 ; 2 5
6 2 ; 1 3
2 6 ; 0 2
6 2 ; 3 4 
6 2 ; 4 5
6 2 ; 3 5
1 3 ; 
2 1 ; 
2 6 ;
6 2 ;
6 2 ;
6 2 ;
```

