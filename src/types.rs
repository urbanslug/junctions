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
pub type EdtSets = Vec<(usize, usize, Letter, Height)>;
