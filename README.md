# POCR

# Intro

**POCR** is a light-weight CFL-reachability tool developed upon the open-source tool [SVF](https://github.com/SVF-tools/SVF.git) and run on Ubuntu (20.04 or higher) or MacOS. It contains a basic CFL-reachability solver and several several state-the-art techniques published in recent papers.

## News

Under refractoring...

## Award

2022 ACM SIGPLAN Distinguished Artifact Award

## Publications

- Yuxiang Lei*, Camille Bossut*, Yulei Sui and Qirun Zhang. <a href="https://dl.acm.org/doi/10.1145/3656451">Context-Free Language Reachability via Skewed Tabulation</a>. <i>ACM SIGPLAN Conference on Programming Language Design and Implementation (PLDI), 2024</i>.
- Pei Xu, Yuxiang Lei*, Yulei Sui and Jingling Xue. <a href="https://dl.acm.org/doi/10.1145/3649862">Iterative-Epoch Online Cycle Elimination for Context-Free Language Reachability</a>. <i>Proceedings of the ACM on Programming Languages 8.OOPSLA1 (2024): 1437-1462</i>.
- Yuxiang Lei, Yulei Sui, Shin Hwei Tan and Qirun Zhang. <a href="https://dl.acm.org/doi/10.1145/3591233">Recursive State Machine Guided Graph Folding for Context-Free Language Reachability</a>. <i>Proceedings of the ACM on Programming Languages 7. PLDI (2023): 318–342</i>.
- Yuxiang Lei, Yulei Sui, Shuo Ding and Qirun Zhang. <a href="https://dl.acm.org/doi/10.1145/3563343">Taming Transitive Redundancy for Context-Free Language Reachability</a>. <i>Proceedings of the ACM on Programming Languages 6. OOPSLA2 (2022): 1556-1582</i>.

## Setup POCR

### 1. Install dependencies

```
sudo apt install cmake gcc g++ libtinfo-dev libz-dev zip wget npm
```

### 2. Install SVF

```
git clone -b svf-llvm14 https://github.com/kisslune/SVF.git
cd SVF
source ./build.sh
```

### 3. Set SVF Directory as an Environment Parameter 

You can do this by appending the following line

```export SVF_DIR=<your SVF directory>```

to the file `/etc/profile` or `~/.bashrc`.

### 4. Build POCR

```
git clone https://github.com/kisslune/POCR.git
cd POCR
source ./build.sh
```

### 5. Setup environment


```
. ./setup.sh
```



## Usage


### General CFL-reachability solver

POCR provides a general standard CFL-reachability solver as the baseline, which accepts an input <GRAMMAR_FILE> and an input <GRAPH_FILE>. You can invoke the general solver using the following command:


```
cfl -std <GRAMMAR_FILE> <GRAPH_FILE>
```

or 

```
cfl -pocr <GRAMMAR_FILE> <GRAPH_FILE>
```

The <GRAMMAR_FILE> should be normalized with the following format:

```
A   B   C
```

where A denotes the symbol on the left-hand side of a production rule and B, C denotes the symbols on the right-hand side of the production rule.
**Note**: B and C are optional, and the symbols should be separated by a tab character, i.e., `"\t"`.

<GRAPH_FILE> should be with the following format:

```
EDGE_SOURCE    EDGE_DESTINATION    EDGE_LABEL    LABEL_INDEX
```

**Note**: the symbols should also be separated by a tab character, i.e., `"\t"` and any EDGE_LABEL having a subscript should end with "\_i". For example,  an edge $1 \xrightarrow{X_3} 2$ is denoted by 
```1    2    X_i    3``` 
in <GRAPH_FILE>.


### Field-Sensitive Alias Analyzer

You can invoke the alias analyzer by either of the following two commands:

```
aa -std <GRAPH_FILE>
```

```
aa -pocr <GRAPH_FILE>
```
where the format of the input <GRAPH_FILE> is the same as what for the [general solver](https://github.com/kisslune/POCR/blob/master/README.md#general-cfl-reachability-solver).

The difference between the above two commands is that the first one solves CFL-reachability using the classical algorithm whereas the second one uses our *POCR* algorithm.

**Note**: the EDGE_LABEL of the input grammar file may contain the following six terminals of the context-free grammar:


- a: assigment
- d: dereference
- f_i: address of field with an index i

and their reverses aber, dbar and fbar_i.

[Grammar](https://github.com/kisslune/POCR/blob/master/images/aa.png) (normalized and already embeded in the solver).


### Context-Sensitive Value-Flow Analyzer

Like alias analyzer, you can use

```
vf -std <GRAPH_FILE>
```
or
```
vf -pocr <GRAPH_FILE>
```

to run value-flow analysis on the input <GRAPH_FILE> with the classical CFL-reachability algorithm or our *POCR* algorithm. The format of the input <GRAPH_FILE> is also the same as what for the [general solver](https://github.com/kisslune/POCR/blob/master/README.md#general-cfl-reachability-solver).

**Note**: the EDGE_LABEL of the input grammar file may contain the following six terminals of the context-free grammar:

- a: assignment
- call_i: call with a callsite index i
- ret_i: return with a callsite index i


[Grammar](https://github.com/kisslune/POCR/blob/master/images/vf.png) (normalized and already embeded in the solver).


### Test Cases

We provide some program expression graphs (PEGs) and value-flow graphs (VFGs) in [tests/](https://github.com/kisslune/POCR/tree/master/tests) as the test cases of POCR. You can enter the directory and try POCR with the following commands:


```
aa -pocr art.peg
vf -pocr art.vfg
cfl -std aa.cfg art.peg
cfl -std vf.cfg art.vfg
```

