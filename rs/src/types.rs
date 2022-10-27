use petgraph::graph::NodeIndex;
// use petgraph::dot::{Dot, Config};
use eds::Letter;
use petgraph::{data::Build, Graph};
use std::collections::HashSet;

pub struct Tree<'a> {
    pub root: NodeIndex,
    pub hard_node: Option<NodeIndex>,
    pub graph: &'a mut Graph<usize, String>,
    pub leaves: HashSet<NodeIndex>,
}

pub type Height = usize;

// start index
// stop (exclusive) index
// degenerate or solid letter
// number of variants
pub type EdtSets = Vec<(usize, usize, Letter, Height)>;

// A letter index is either the start or end of a letter within a set

pub struct EdtProperties {
    pub edt_sets: EdtSets,
    pub starts: Vec<Vec<usize>>,
    pub ends: Vec<Vec<usize>>,
}

// A string of many strings seperated by an underscore
pub struct MultiString {
    pub underscore_separated_string: String,
    pub rank_bit_vector: Vec<usize>,
}
