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

### Correctness
We have tested the correctness of our algorithm against rivet. Notice that when loading a point cloud in floating point, rivet use rational numbers for its own drawing purpose. In order to match the result, we also implemented the rational number data types. However, in practice, `float` is often enough. 

```bash
# Our 
cd build 
# ball density-Rips
./main /home/yluo/Documents/graph-mph0/experiment/data/PointCloud/annulus_200.txt ball_density_rivet annulus_200_ball_density_exact_our_out.txt
# degree-Rips
./main /home/yluo/Documents/graph-mph0/experiment/data/PointCloud/annulus_200.txt degree_rivet annulus_200_degree_exact_our_out.txt --swap_xy

# rivet
# ball-density function 
/home/yluo/Documents/rivet/build/rivet_console /home/yluo/Documents/graph-mph0/experiment/data/PointCloud/annulus_200.txt -b --num_threads 1 --datatype points -H 0 --bifil function --function balldensity[] > annulus_200_ball_density_output_rivet.txt
# degree
/home/yluo/Documents/rivet/build/rivet_console /home/yluo/Documents/graph-mph0/experiment/data/PointCloud/annulus_200.txt -b --num_threads 1 --datatype points -H 0 --bifil degree > annulus_200_degree_output_rivet.txt

# we can now compare their difference using python script in test folder 
python /home/yluo/Documents/graph-mph0/test/python/read_and_compare_rivet.py
# The output will looks like the following  
# ours betti_0 len: 35
# ours betti_1 len: 259
# ours betti_2 len: 60
# rivet betti_0 len: 35
# rivet betti_1 len: 259
# rivet betti_2 len: 60
# --------------------------------
# Print the difference
# Compare betti_0:
# Compare betti_1:
# Compare betti_2:
```
### Efficiency
We compare the efficiency over two existing packages, rivet and mpfree, which can compute absolute betti numbers or minimal presentation or both. 



## Notes
### How rivet deal with floating points
1. load data in double
2. approximate a double to a rational number(boost) 
```cpp
// approximate a double to an exact type
exact approx(double x)
{
    int d = 7; //desired number of significant digits
    int log = (int)floor(log10(x)) + 1;

    if (log >= d)
        return exact((int)floor(x));

    long denom = pow(10, d - log);
    return exact((long)floor(x * denom), denom);
}
```
3. convert it back into double when necessary
```cpp
double_value = numerator(e).convert_to<double>() / denominator(e).convert_to<double>();
```