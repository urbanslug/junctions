use eds::EDT;
use generalized_suffix_tree::GeneralizedSuffixTree;
use petgraph::dot::{Config, Dot};
use petgraph::graph::NodeIndex;
use petgraph::{data::Build, Graph};
use std::cmp::min;
use std::collections::HashSet;

use ndarray::{Array, Array2};
use ndarray_to_img::plot::{self, Plottable};

use crate::types::Tree;

const ADD_Q_0: bool = true;
const ADD_ACCEPTING_STATE: bool = false;

pub fn build_automaton(edt: EDT) -> Graph<usize, String> {
    let diameter = edt.w();
    let solids: &Vec<(usize, usize)> = edt.get_solid_intervals();
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
            let h = edt[stop - 1].len();

            let edge_labels = (0..h)
                .map(|element| {
                    // let node = automaton.add_node((n, element));
                    let mut seed: String = (start..stop)
                        .map(|col| edt.base_at([col, element]) as char)
                        .collect::<String>();
                    seed.retain(|c| c != '*');
                    if seed == String::from("") {
                        seed = String::from("\u{03b5}");
                    }
                    seed
                })
                .collect::<HashSet<String>>();

            if edge_labels.len() == 1 {
                // solid
                let previous_node = maybe_previous_node.unwrap();
                let current_node = automaton.add_node(n);
                let edges = edge_labels
                    .iter()
                    .map(|label| (previous_node, current_node, label.clone()))
                    .collect::<Vec<(NodeIndex, NodeIndex, String)>>();

                automaton.extend_with_edges(edges);
                maybe_previous_node.replace(current_node);
            } else {
                let previous_node = maybe_previous_node.unwrap();

                let labels_vec = edge_labels
                    .iter()
                    .map(|l| l.as_str())
                    .collect::<Vec<&str>>();

                let mut trie = Tree {
                    root: previous_node,
                    hard_node: None,
                    graph: &mut automaton,
                    leaves: HashSet::<NodeIndex>::new(),
                };

                build_radix_tree(&mut trie, &labels_vec, n, true);

                maybe_previous_node.replace(trie.hard_node.unwrap());
                // automaton.extend_with_edges(edges);
            }
        }

        n += 1;
    }

    automaton
}

/// Print the dot output to stdout
pub fn print_dot(automaton: &Graph<usize, String>, input_str: &str) {
    print!("digraph {{");
    print!(
        "
graph [label=\"{}\", labelloc=t, fontsize=12];
rankdir=LR
node [shape=circle]
",
        input_str
    );
    print!(
        "{:?}",
        Dot::with_config(&automaton, &[Config::GraphContentOnly])
    );
    println!("}}");
    println!()
}

fn build_generalized_suffix_tree(text: Vec<&str>) {
    let mut tree = GeneralizedSuffixTree::new();

    text.into_iter().enumerate().for_each(|(idx, s)| {
        tree.add_string(String::from(s), (idx as u8) as char);
    });
    tree.pretty_print();
    println!("{}", tree.is_suffix("BCE"));
}

// TODO: handle duplicate string
fn build_radix_tree(automaton: &mut Tree, text: &Vec<&str>, id: usize, add_hard_node: bool) {
    // let mut text: Vec<&[u8]> = text.iter().map(|s| s.as_bytes()).collect();
    let mut text: Vec<Vec<char>> = text
        .iter()
        .map(|s| s.chars().collect::<Vec<char>>())
        .collect();
    text.sort();

    let mut leaves = HashSet::<NodeIndex>::new();

    let root_node: NodeIndex = automaton.root;
    let trie = &mut automaton.graph;

    let mut i_path = Vec::<(NodeIndex, NodeIndex, String)>::new();

    let mut i: usize = 0;

    while i < text.len() - 1 {
        let f = text.get(i).unwrap();
        let s = text.get(i + 1).unwrap();

        let f_len = f.len();
        let s_len = s.len();

        let mut j: usize = 0;

        let mut string_buffer = String::new();

        // dbg!(f[j..].iter().map(|b| *b as char).collect::<String>(),
        //     s[j..].iter().map(|b| *b as char).collect::<String>(),
        //     &branches,
        //     f_len,
        //     s_len);

        loop {
            if j < min(f_len, s_len) && f[j] == s[j] {
                string_buffer.push(f[j] as char);
            } else {
                let mut h = HashSet::<(NodeIndex, NodeIndex, String)>::new();
                let f_suffix: String = f[j..].iter().map(|b| *b as char).collect();
                let s_suffix: String = s[j..].iter().map(|b| *b as char).collect();

                // replace with i_path check
                //if  trie.edges(root_node).count() == 0 {
                if i_path.is_empty() {
                    //let internal_node = trie.add_node(id);
                    let internal_node = root_node;

                    // h.insert((root_node, internal_node, string_buffer.clone()));

                    let leaf_f = trie.add_node(id);
                    let leaf_s = trie.add_node(id);

                    h.insert((internal_node, leaf_f, f_suffix));
                    h.insert((internal_node, leaf_s, s_suffix.clone()));

                    // i_path.push((root_node, internal_node, string_buffer.clone()));
                    i_path.push((internal_node, leaf_s, s_suffix.clone()));

                    trie.extend_with_edges(h);

                    leaves.insert(leaf_f);
                    leaves.insert(leaf_s);
                } else {
                    // walk down the trie following the path of the ith string
                    let mut k: usize = 0; // TODO: rename furthest similar char
                    let mut furthest_edge: usize = 0;
                    for (_, _, s) in i_path.iter() {
                        k += s.len();

                        if k >= j {
                            break;
                        }

                        furthest_edge += 1;
                    }

                    dbg!(k, j);

                    if furthest_edge >= i_path.len() {
                        let (i, o, s) = i_path[furthest_edge - 1].clone();

                        // let internal_node = trie.add_node(id);
                        let leaf_s = trie.add_node(id);
                        let leaf_f = trie.add_node(id);

                        h.insert((o, leaf_f, String::from("")));
                        h.insert((o, leaf_s, s_suffix.clone()));

                        i_path.push((o, leaf_s, s_suffix.clone()));
                        trie.extend_with_edges(h);

                        // update leaves
                        leaves.remove(&o);
                        leaves.insert(leaf_f);
                        leaves.insert(leaf_s);
                    } else {
                        // break the current furthest edge
                        let (i, o, s) = i_path[furthest_edge].clone();
                        let edge_index = trie.find_edge(i, o).unwrap();
                        trie.remove_edge(edge_index);

                        // add the new leaf and internal node
                        let internal_node = trie.add_node(id);
                        let leaf_s = trie.add_node(id);

                        let s_len = s.len();
                        let s_break = s_len - (k - j);

                        dbg!(&s, s_break);

                        let f_prefix = s[0..s_break].chars().collect::<String>();
                        let f_prefix_2 = s[s_break..].chars().collect::<String>();

                        dbg!(&f_prefix, &f_prefix_2, &s_suffix);

                        h.insert((i, internal_node, f_prefix));
                        h.insert((internal_node, o, f_prefix_2));
                        h.insert((internal_node, leaf_s, s_suffix.clone()));

                        // glue it back together
                        trie.extend_with_edges(h);

                        i_path.truncate(furthest_edge);
                        i_path.push((internal_node, leaf_s, s_suffix.clone()));

                        // update leaves
                        leaves.insert(leaf_s);
                    }
                }

                break;
            }

            j += 1;
        }
        i += 1;
    }

    let hard_node = trie.add_node(id);
    automaton.hard_node = Some(hard_node);

    if add_hard_node {
        let edges = leaves
            .iter()
            .map(|leaf_node| (*leaf_node, hard_node, String::new()));
        trie.extend_with_edges(edges);
    }
}

pub fn visualize_matrix(matrix: &Array2<i32>, image_name: &str) {
    let config = ndarray_to_img::Config {
        verbosity: 0,
        with_color: false,
        annotate_image: true,
        draw_diagonal: false,
        draw_boundaries: true,
        scaling_factor: 50,
    };

    let matrix = plot::Matrix {
        matrix: matrix.clone(),
    };

    let scaled_matrix: plot::Matrix<i32> = matrix.scale_matrix(&config);
    assert_eq!(scaled_matrix.plot(&config, image_name).unwrap(), ());
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_build_automaton() {
        // let ed_string = "A{T,G}{C,A}{T,A}TC";
        // let ed_string = "ACTA{ATC,CGA}{ACGT,GCGC}A{CTA,C,}A";
        // let ed_string = "ACTA{ATC,CGA}C{ACGT,GCGC}A";
        let ed_string = "{ATC,CGA}{ACGT,GCGC}A{CTA,C,}A{C,GTA,}";
        // let ed_string = EDT::from_str("ACTA{ATC,CGA}{ACGT,GCGC}A{CTA,C,}A{C,GTA}");

        let edt = EDT::from_str(ed_string);
        println!("\n{edt}");

        let automaton = build_automaton(edt);
        print_dot(&automaton, ed_string);
    }

    #[test]
    fn test_search_using_st() {
        let text = Vec::from(["ATCAT", "ATCAG"]);
        build_generalized_suffix_tree(text);
    }

    #[test]
    fn test_build_radix_tree() {
        // let text = Vec::from(["ATCAT", "ATCAG", "AAACTA"]);
        //let text = Vec::from(["A", "ATCAGA", "ATCAG", "AAACTA"]);
        let text = Vec::from(["CAT", "A", "ATCAGA", "ATCAG", "AAACTA"]);

        let id = 0;

        let mut graph = Graph::<usize, String>::new();
        let root_node: NodeIndex = graph.add_node(id);

        let mut trie = Tree {
            root: root_node,
            hard_node: None,
            graph: &mut graph,
            leaves: HashSet::<NodeIndex>::new(),
        };

        build_radix_tree(&mut trie, &text, id, false);
        print_dot(&trie.graph, &text.join(" "));

        for leaf in trie.leaves.iter() {
            trie.graph
                .edges_directed(*leaf, petgraph::Incoming)
                .for_each(|e| {
                    dbg!(e);
                });
        }
    }
}
