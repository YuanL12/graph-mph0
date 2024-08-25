# graph-mph0
This is a project about computing Multiparemeter Persistence at dimension 0 for filtered graphs.

## Install 

### Platform: Linux with CPU
`conda create -n dl  python=3.10`

`conda install pytorch==2.3.1 torchvision==0.18.1 torchaudio==2.3.1 cpuonly -c pytorch`

`conda install -c pytorch torchdata`

`conda install -c dglteam/label/th23_cpu dgl`

`pip install jupyterlab`

`conda install -c conda-forge pandas`

`conda install pydantic -c conda-forge`

### Build python lib
Make sure you have activated your desired conda env first, then 
`cmake -DPYTHON_EXECUTABLE=$(which python) -DPYTHON_LIBRARY=$(which python) ..`

Note: data from igl will be stored by default in your root or user path like `/root/.dgl`, remove them if they are not used in the future. 


## Usage
1. Use non-negative integer to label vertices due to node-printing need in Dendrogram


### Reference

[Stanford Course Note](https://web.stanford.edu/class/archive/cs/cs166/cs166.1146/lectures/08/Slides08.pdf) includes Splay Tree. 

[MIT Course Note](https://ocw.mit.edu/courses/6-854j-advanced-algorithms-fall-2008/resources/lec6/) iclude Splay Tree and Dynamic Tree.

