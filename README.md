# Changelog

### General CFL-reachability solver

After setting up, use the following command to run CFL-reachability analysis on the input grammar and input graph via the genaral CFL-reachability solver:

```
cfl -std GRAMMAR_FILE GRAPH_FILE
```

The GRAMMAR_FILE should be normalized with the following format:

```
A   B   C
```

where A denotes the symbol on the left-hand side of a production rule and B, C denotes the symbols on the right-hand side of the production rule.
**Note**: B and C are optional, and the symbols should be separated by a tab character, i.e., `\t`.

The GRAPH_FILE should be with the following format:

```
EDGE_SOURCE    EDGE_DESTINATION    EDGE_LABEL    LABEL_INDEX
```

**Note**: the symbols should also be separated by a tab character, i.e., `\t` and any EDGE_LABEL with and index (i.e., subscript) should end with "_i", e.g., `1    2    f_i    3`.



### Plan:

- Include our work to the open-source tool [SVF](https://github.com/SVF-tools/SVF).

- Develop a grammar reader which can automatically identify the transitive relations in the input grammar so that the POCR solver can be applied.
