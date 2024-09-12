# graph-mph0
This is a project about computing Multiparemeter Persistence at dimension 0 for filtered graphs.

## Usage
1. Use non-negative integer to label vertices due to node-printing need in Dendrogram
2. Node/edge features in Python has to be np.float64 to be passed into C++ double type. 

## Install 
Something to say.

### Common questions
1. Python: package not found

Solution: Make sure you have activated your desired conda environment first, then 
```bash
cmake -DPYTHON_EXECUTABLE=$(which python) -DPYTHON_LIBRARY=$(which python) ..
```

Or you can manually set the Python executable and library paths in binding/CMakeLists.txt as the following
```bash
set(PYTHON_EXECUTABLE /root/miniconda3/envs/pyg/bin/python)
set(PYTHON_LIBRARY /root/miniconda3/envs/pyg/bin/python)
```

2. Python: cannot find gcc lib.  

`ImportError: libstdc++.so.6: version 'GLIBCXX_3.4.32' not found (required by graph-mph0/build/abmph/_abmph.cpython-311-x86_64-linux-gnu.so)`

Solution: (https://github.com/pybind/pybind11/discussions/3453) you will need to first remove the one in your conda environment and then copy/link system GLIBCXX_3.4.32 to the conda directory. Here are the commands
```bash
# find the path of your system CXX lib
find /usr -name "libstdc++.so*" 2>/dev/null
# then pick the one with libstdc++.so.6

# check the GLIBCXX versions you have 
strings /usr/lib/x86_64-linux-gnu/libstdc++.so.6 | grep GLIBCXX
# If GLIBCXX_3.4.32 is listed, then this version is supported by that libstdc++.so file.

# remove the one you have in conda env and link the system one 
rm ~/miniconda3/envs/your_package_name/lib/libstdc++.so.6
ln -s /usr/lib/x86_64-linux-gnu/libstdc++.so.6 ~/miniconda3/envs/your_package_name/lib/libstdc++.so.6 
```



### Reference

[Stanford Course Note](https://web.stanford.edu/class/archive/cs/cs166/cs166.1146/lectures/08/Slides08.pdf) includes Splay Tree. 

[MIT Course Note](https://ocw.mit.edu/courses/6-854j-advanced-algorithms-fall-2008/resources/lec6/) iclude Splay Tree and Dynamic Tree.

