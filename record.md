## 6.12

```
$ ./topTree_out ../experiment/data/PointCloud/annulus_800.txt 
--------------------------------
Testing degree Rips Filtration
Computing MPH0 by TopTree
nV, nE: 640000, 142633664
size of active grades: 1263949
Computing MPH0... by TopTree
size of adjacency list before collapse: 
        Size of adjacency list (nV): 640000
        Size of edges (nE * 2): 285267328
size of adjacency list after edge collapse (algorithm 1): 
        Size of adjacency list (nV): 636279
        Size of edges (nE * 2): 285259886
size of adjacency list after collapse (algorithm 2): 
        Size of adjacency list (nV): 3409
        Size of edges (nE * 2): 283994146
Time taken to collect active vertices: 0 ms
Time taken to collect active edges: 405 ms
Time taken to sort grades: 22485 ms
Build ggraph:                                        8.690 s   (  14.45 % )
Collapse edge:                                       2.220 s   (   3.69 % )
Update graph inside collapse edge:                   0.280 s   (   0.47 % )
Collapse vertex:                                     8.530 s   (  14.19 % )
Update graph inside collapse vertex:                 1.170 s   (   1.95 % )
Collect grades lexicographically:                   22.890 s   (  38.07 % )
Main Loop(visit grades lexicographically):           9.400 s   (  15.63 % )
Overall timer: 60.13
```
## TODO

Later (not urgent)
1. set a threshold on degree and radius on filtration. 


## Commands 
```bash
# rivet
# ball density with betti numbers computed  
$ time /home/yluo/Documents/rivet/build/rivet_console ../experiment/data/annulus_100.txt -b --num_threads 1 --datatype points -H 0 --bifil function --function balldensity[] > rivet_output_100.txt

# degree with betti numbers computed 
$ time /home/yluo/Documents/rivet/build/rivet_console ../experiment/data/annulus_100.txt -b --num_threads 1 --datatype points -H 0 --bifil degree

# compute the minimal presentation
$ ../rivet/rivet_console experiment/data/rivet/annulus_50_ball_density.firep --minpres --num_threads 1 --datatype firep > rivet_output_100.txt

# add verbosity level 
$ ../rivet/rivet_console experiment/data/rivet/annulus_600_ball_density.firep --minpres --num_threads 1 --datatype firep --verbosity 7

# mpfree 
$ mpfree/build/mpfree_sequential data/mpfree/annulus_400_deg.firep -v
Chunk preprocessing...Num entries at start chunk: 35692214
Local reduction
Num entries after local reduce: 35690560
Sparsification
Build up smaller matrices
After chunk reduction, matrix has 464532 columns and 1567 rows
N' is 466099
Num entries after chunk: 929064
done
Min Gens...done, size is 1567x2084
Ker basis...done, size is 1567x1567
Reparameterize...done
Resulting semi-minimal presentation has 2084 columns and 1567 rows
Minimize...done
Resulting minimal presentation has 2083 columns and 1566 rows
Overall timer: 22.3127
IO timer:              18.5755     ( 83.2508% )
(In IO) Load data into prematrix timer: 5.51593     ( 24.721% )
(In IO) Prematrix to graded matrix timer: 12.879     ( 57.7204% )
Chunk timer:           2.5571     ( 11.4603% )
Mingens timer:         0.590366     ( 2.64587% )
Kerbasis timer:        0.00150547     ( 0.00674716% )
Reparam timer:         0.00166792     ( 0.00747518% )
```


## Rivet




## mpfree
```zsh
(base) yluo@UHome:build$ ./mpfree_sequential -h
Usage: ./mpfree_sequential [OPTIONS] <INPUT_FILE> <OUTPUT_FILE>

Computes the minimial presentation of a sequence of free persistence modules, either in Rivet firep format or in scc2020 format. The output is a sequence of free persistence modules of length one. The output file can be omitted, in which case no output is created

Options:
-c                      - checks whether the input is in the right format and exits
--resolution            - computes the full free resolution. In this case, the output is always in scc2020 format
--rivet-format          - yields the output as a firep file as in the RIVET library (by default, it is a scc2020 file)
--no-chunk              - disables chunk-preprocessing (by default, it is enabled)
--clearing              - enables clearing optimization (by default, it is disabled)
--bit_tree_pivot_column - uses the column type bit_tree_pivot_column from phat. The default is vector_vector from phat
--dim=p                 - Computes the minimal presentation for the p-th, (p+1)-st, and (p+2)-nd module (starting the count with 1). Only useful with scc2020 input format (by default, p=1)
-v                      - prints status messages during the execution
-h --help               - prints this message

```


Note: The end of the input file has to be an empty line!

### Output

#### H_1
This example has a natural geometric interpretation: The boundary of a triangle is born at (0,0), and the triangle is filled in at both (1,0) and (0,1). 
The input gives the portion of the resulting chain complex required to compute the 1st persistent homology module.
```bash
$ ./build/mpfree_sequential tests/small.firep out.txt -v 

firep
first parameter
second parameter
2 3 3
1 0 ; 0 1 2 
0 1 ; 0 1 2 
0 0 ; 1 2 
0 0 ; 0 2 
0 0 ; 0 1 

Output (Resulting minimal presentation matrix has 2 columns and 1 rows): 
scc2020
2
2 1 0
0 1 ; 0 
1 0 ; 0 
0 0 ; 

```

#### H_0
This is the example in Luis and Dmitriy's paper. Two vertices born at (1,3) and (2,1), whereas the two edges are born at (3,5) and (5,3). 
```bash
$ ./build/mpfree_sequential tests/small_dim0.firep out_2.txt -v

firep
first parameter
second parameter
2 2 0
3 5 ; 0 1 
5 3 ; 0 1 
1 3 ; 
2 1 ; 

Resulting minimal presentation has 2 columns and 2 rows
scc2020
2
2 2 0
3 5 ; 0 1 
5 3 ; 0 1 
1 3 ; 
2 1 ; 

## You can see the resolution 
$./mpfree_sequential tests/small_dim0.firep --resolution -v resolution_out.txt
scc2020
2
1 2 2 0
5 5 ; 0 1 
5 3 ; 0 1 
3 5 ; 0 1 
2 1 ; 
1 3 ; 
```


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
