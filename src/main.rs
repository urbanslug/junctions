use eds::EDT;
use petgraph::Graph;
use petgraph::graph::NodeIndex;
use std::collections::HashSet;
use petgraph::dot::{Dot, Config};


fn build_automaton(edt: EDT) -> Graph<(usize, usize, usize), Vec<u8>>{
    let diameter =  edt.w();
    let solids: &Vec<(usize, usize)> =  edt.get_solid_intervals();
    let degenerates: &Vec<(usize, usize)> = edt.get_degenerate_letters();

    let mut i: usize = 0; // solids_pointer
    let mut j: usize = 0; // degenerates_pointer

    let mut automaton = Graph::<(usize, usize, usize), Vec<u8>>::new();
    // temporary buffer to store the nodes from the previous iteration of the loop
    // from_nodes
    let mut stored = HashSet::<(NodeIndex, usize)>::new();

    // start node
    let start_node = automaton.add_node((0, 0, 0));
    stored.insert(((start_node), 0));

    while i < solids.len() || j < degenerates.len() {
        let (start, stop) = if i < solids.len() && j < degenerates.len() {
            let (s_s, s_e): (usize, usize) = solids[i];
            let (d_s, d_e): (usize, usize) = degenerates[j];

            if s_s < d_s {
                i += 1;
                (s_s, s_e)
            } else {
                j += 1;
                (d_s, d_e)
            }


        } else if i >= solids.len() {
            let (d_s, d_e): (usize, usize) = degenerates[j];
            j += 1;
            (d_s, d_e)
        } else {
            let (s_s, s_e): (usize, usize) = solids[i];
            i += 1;
            (s_s, s_e)
        };


        if stop <= diameter {
            let h = edt[stop-1].len();
            let nodes_added = (0..h)
                .map(|element| {
                    let node = automaton.add_node((start, stop, element));
                    (node, start)
                })
                .collect::<HashSet<(NodeIndex, usize)>>();

            let edges = stored
                .iter()
                .flat_map(|(from_node, _)| {
                    nodes_added.iter().map(|(to_node, _)| (*from_node, *to_node))
                })
                .collect::<Vec<(NodeIndex, NodeIndex)>>();

            stored.clear();

            nodes_added.iter().for_each(|v| { stored.insert(*v); } );
            automaton.extend_with_edges(edges);
        }

    }


    let accepting_state = automaton.add_node((diameter, diameter, 0));
    

    let edges = stored
        .iter()
        .map(|(from_node, _)| (*from_node, accepting_state))
        .collect::<Vec<(NodeIndex, NodeIndex)>>();

    automaton.extend_with_edges(edges);

    automaton
}


fn print_dot(automaton: Graph<(usize, usize, usize), Vec<u8>>) {
    print!("digraph {{");
    print!("
rankdir=LR
node [shape=box]
");
    print!("{:?}", Dot::with_config(&automaton, &[Config::GraphContentOnly, Config::EdgeNoLabel]));
    println!("}}");
    println!()

}


fn main() {
    // let ed_string = "A{T,G}{C,A}{T,A}TC";
    // let ed_string = EDT::from_str("ACTA{ATC,CGA}{ACGT,GCGC}A{CTA,C,}A");
    // let edt = EDT::from_str("ACTA{ATC,CGA}C{ACGT,GCGC}A");
    
    // build_compacted_trie(EDT::from_str(ed_string));
}


#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn test_build_compacted_trie() {
        let ed_string = "A{T,G}{C,A}{T,A}TC";
        // let ed_string = "ACTA{ATC,CGA}{ACGT,GCGC}A{CTA,C,}A";
        // let ed_string = "ACTA{ATC,CGA}C{ACGT,GCGC}A";
        let ed_string = "{ATC,CGA}{ACGT,GCGC}A{CTA,C,}A{C,GTA,}";
        // let ed_string = EDT::from_str("ACTA{ATC,CGA}{ACGT,GCGC}A{CTA,C,}A{C,GTA}");


        let edt = EDT::from_str(ed_string);
        println!("\n{edt}");

        let automaton = build_automaton(edt);
        print_dot(automaton);
    }
}
