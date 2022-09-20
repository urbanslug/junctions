use eds::EDT;
use petgraph::Graph;
use petgraph::graph::NodeIndex;
use std::collections::HashSet;
use petgraph::dot::{Dot, Config};
use generalized_suffix_tree::GeneralizedSuffixTree;



const ADD_Q_0: bool = true;
const ADD_ACCEPTING_STATE: bool = false;

fn build_automaton(edt: EDT) -> Graph<usize, String>{
    let diameter =  edt.w();
    let solids: &Vec<(usize, usize)> =  edt.get_solid_intervals();
    let degenerates: &Vec<(usize, usize)> = edt.get_degenerate_letters();

    let mut i: usize = 0; // solids_pointer
    let mut j: usize = 0; // degenerates_pointer

    let mut automaton = Graph::<usize, String>::new();
    let mut maybe_previous_node: Option<NodeIndex> = None;

    let mut n = 0;

    if ADD_Q_0 {
        // start node
        let start_node: NodeIndex = automaton.add_node(n);
        maybe_previous_node = Some(start_node);
        // stored.insert(((start_node), 0));
        n += 1;
    }

    while i < solids.len() || j < degenerates.len() {

        // figure out what we are iterating through
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


        // add nodes or edges for that degenerate letter or solid string
        if stop <= diameter {
            let h = edt[stop-1].len();
            let current_node = automaton.add_node(n);

            let edge_labels = (0..h)
                .map(|element| {
                    // let node = automaton.add_node((n, element));
                    let mut seed: String = (start..stop)
                        .map(|col|  edt.base_at([col, element]) as char )
                        .collect::<String>();
                    seed.retain(|c| c != '*');
                    if seed == String::from("") {
                        seed = String::from("\u{03b5}");
                    }
                    seed
                })
                .collect::<HashSet<String>>();

            let previous_node =  maybe_previous_node.unwrap();
            let edges = edge_labels.iter().map(|seed| (previous_node, current_node, seed.clone()));

            maybe_previous_node.replace(current_node);
            automaton.extend_with_edges(edges);
        }

        n += 1;
    }

    automaton
}


fn print_dot(automaton: Graph<usize, String>, input_str: &str) {
    print!("digraph {{");
    print!("
graph [label=\"{}\", labelloc=t, fontsize=12];
rankdir=LR
node [shape=circle]
", input_str);
    print!("{:?}", Dot::with_config(&automaton, &[Config::GraphContentOnly]));
    println!("}}");
    println!()
}


fn build_generalized_suffix_tree(text: Vec<&str>) {
    let mut tree = GeneralizedSuffixTree::new();

    

    text.into_iter().enumerate().for_each(|(idx, s)| {
        tree.add_string(String::from(s), (idx as u8) as char);
    }
    );
    tree.pretty_print();
    println!("{}", tree.is_suffix("BCE"));
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
        // let ed_string = "A{T,G}{C,A}{T,A}TC";
        // let ed_string = "ACTA{ATC,CGA}{ACGT,GCGC}A{CTA,C,}A";
        // let ed_string = "ACTA{ATC,CGA}C{ACGT,GCGC}A";
        let ed_string = "{ATC,CGA}{ACGT,GCGC}A{CTA,C,}A{C,GTA,}";
        // let ed_string = EDT::from_str("ACTA{ATC,CGA}{ACGT,GCGC}A{CTA,C,}A{C,GTA}");


        let edt = EDT::from_str(ed_string);
        println!("\n{edt}");

        let automaton = build_automaton(edt);
        print_dot(automaton, ed_string);
    }

    #[test]
    fn test_search_using_st() {
        let text = Vec::from(["ATCAT", "ATCAG"]);
        build_generalized_suffix_tree(text);
    }
}
