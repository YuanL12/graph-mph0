# graph-mph0
This is a project about computing Multiparemeter Persistence at dimension 0 for filtered graphs.

## TODO
1. Make sure the result of rivet and mpfree matches in paper: Fast minimal Presentation of Bi-
2. Update the table result 
4. Memory Usage

Done: 
3. try the new top-tree library to see the performance change.x
5. Size of grade_collection vector 

## User Guide 
### Input
We suppot the following two types input
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
### Effciency
We compare the efficency over two exisitng packages, rivet and mpfree, which can compute absolute betti numbers or minial presentation or both. 



## Install 
### Basic Steps:
Clone the repo and corresponding all submodules
```shell
git clone git@github.com:YuanL12/graph-mph0.git
cd graph-mph0/
git submodule update --init --recursive
```

Build this C++ project including pythin binding
```Shell
mkdir build && cd build
cmake ..
make -j4
```

Now you can test if the python package is useable
```Shell
cd ../test
python test.py
```

### Pytorch environment
Some ipynb files included use [`pyg`](https://github.com/pyg-team/pytorch_geometric). With conda, you will need to install 
```Shell
# The folloiwng is for GPU verison. you can look for cpu version online from Pytorch web
conda install pytorch==2.3.1 torchvision==0.18.1 torchaudio==2.3.1 pytorch-cuda=12.1 -c pytorch -c nvidia

# Somehow pyg with conda will cause conflicts, but pip is fine.   
pip install torch_geometric 
```

### Common questions
1. Python: abmph package not found

Solution: Make sure you have activated your desired conda environment first, then 
```bash
cmake -DPYTHON_EXECUTABLE=$(which python) -DPYTHON_LIBRARY=$(which python) ..
```

Or you can manually set the Python executable and library paths in `binding/CMakeLists.txt` as the following
```bash
set(PYTHON_EXECUTABLE /root/miniconda3/envs/pyg/bin/python)
set(PYTHON_LIBRARY /root/miniconda3/envs/pyg/bin/python)
```

2. Python: cannot find gcc lib.  
If you use conda and meet the following problem:
```Shell
(base) yluo@yluo-Legion-T7-34IAZ7:~/Documents/graph-mph0/test$ python test.py 
Traceback (most recent call last):
  File "/home/yluo/Documents/graph-mph0/test/test.py", line 3, in <module>
    import abmph
  File "/home/yluo/Documents/graph-mph0/test/../build/abmph/__init__.py", line 1, in <module>
    from ._abmph import *
ImportError: /home/yluo/miniconda3/bin/../lib/libstdc++.so.6: version `GLIBCXX_3.4.32' not found (required by /home/yluo/Documents/graph-mph0/test/../build/abmph/_abmph.cpython-312-x86_64-linux-gnu.so)
```
**Reason**: Pybind11 uses system gcc to build our Python package `_abmph.cpython-312-x86_64-linux-gnu.so` and so links the `libstdc++` library in system path. On the other hand, Python in a conda environment will use the `stdlibc++` provided in the env path. Thus, depending on your g++/gnu version, the `libstdc++` lib in your system may contain a newer version `GLIBCXX_3.4.32` than the `libstdc++.so.6` in your conda environment. You can also check the dependencies of `_abmph.cpython-310-x86_64-linux-gnu.so` by:
```shell
ldd build/abmph/_abmph.cpython-310-x86_64-linux-gnu.so | grep libstdc++
# it will output something like
# libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007940c0000000)      
```
It shows that the `libstdc++.so.6` used by Pybind11 is located at `/usr/lib/x86_64-linux-gnu`.

**Solution**: 
1. If you only want a temporary change in current terminal session, append your system gnu path to library path list by `export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH`.
2. You can also modify your current conda environment (well, some risks). You will need to first remove the one in your conda environment and then copy/link system libstdc++ containing `GLIBCXX_3.4.32` to the conda directory (Ref: https://github.com/pybind/pybind11/discussions/3453). Here are the commands

```bash
# find the path of your system CXX lib used by gcc (Pybind11 will use the system gcc by default)
find /usr -name "libstdc++.so*" 2>/dev/null
# In may case, it gives the following: 
# /usr/lib/gcc/x86_64-linux-gnu/13/libstdc++.so
# /usr/lib/x86_64-linux-gnu/libstdc++.so.6
# /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.33
# /usr/share/gdb/auto-load/usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.33-gdb.py

# Then pick /usr/lib/x86_64-linux-gnu/libstdc++.so.6 as it is used by your Pybind11

# check the GLIBCXX versions you have, you should see GLIBCXX_3.4.32
strings /usr/lib/x86_64-linux-gnu/libstdc++.so.6 | grep GLIBCXX

# remove the one you have in conda env and link the system one 
rm ~/miniconda3/envs/your_package_name/lib/libstdc++.so.6
ln -s /usr/lib/x86_64-linux-gnu/libstdc++.so.6 ~/miniconda3/envs/your_package_name/lib/libstdc++.so.6 
```



## Usage of the Python package
1. Use non-negative integer to label vertices due to node-printing need in Dendrogram
2. Node/edge features in Python has to be np.float64 to be passed into C++ double type. 


## Theory 

Tarjan and Sleator showed that:

Any sequence of m operations on an n-node splay tree takes at most O(mlogn) time total.

➡️ So, average cost = O(log n) per operation — amortized complexity.




## How rivet deal with floating points
1. load data in double
2. approximate a double to a rational numebr(boost) 
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



## Reference
[Stanford Course Note](https://web.stanford.edu/class/archive/cs/cs166/cs166.1146/lectures/08/Slides08.pdf) includes Splay Tree. 

[MIT Course Note](https://ocw.mit.edu/courses/6-854j-advanced-algorithms-fall-2008/resources/lec6/) iclude Splay Tree and Dynamic Tree.

