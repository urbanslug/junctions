# Junctions

Do two elastic degenerate strings have an intersection?


## Test

Example 
```
cargo test naive::tests::test_contains_intersect -- --nocapture
```

### Visualize dot
```
wl-paste | sed 's/label = \"\\"\\"\"//g' | dot -Tpng > output.png
```
