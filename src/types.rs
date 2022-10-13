use petgraph::graph::NodeIndex;
// use petgraph::dot::{Dot, Config};
use petgraph::{Graph, data::Build};
use std::collections::HashSet;

pub struct Tree<'a> {
    pub root: NodeIndex,
    pub hard_node: Option<NodeIndex>,
    pub graph: &'a mut Graph<usize, String>,
    pub leaves: HashSet<NodeIndex>
}
