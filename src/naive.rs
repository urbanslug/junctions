use eds::EDT;
use generalized_suffix_tree::GeneralizedSuffixTree;
use ndarray::{Array, Array2};
use ndarray_to_img;
use ndarray_to_img::plot::{self, Plottable};
use petgraph::dot::{Config, Dot};
use petgraph::graph::NodeIndex;
use petgraph::visit::EdgeRef;
use petgraph::Graph;
use std::cmp::min;
use std::collections::HashSet;

use super::utils::print_dot;
use crate::utils;

const LOG_LEVEL: u8 = 5;

/// Build an automaton on all the chars of the ED-string
pub fn build_automaton(edt: EDT) -> Graph<usize, String> {
    let diameter = edt.w();
    let size = edt.size();

    let solids: &Vec<(usize, usize)> = edt.get_solid_intervals();
    let degenerates: &Vec<(usize, usize)> = edt.get_degenerate_letters();

    let mut i: usize = 0; // solids_pointer
    let mut j: usize = 0; // degenerates_pointer

    let mut automaton = Graph::<usize, String>::new();
    let mut previous_nodes = Vec::<NodeIndex>::new();

    let mut n: usize = 0;
    // let mut idx: usize = 0;
    // let mut prev_hard_node = n;

    // Always add start node
    // start node
    let start_node: NodeIndex = automaton.add_node(n);
    previous_nodes.push(start_node);
    // stored.insert(((start_node), 0));
    n += 1;

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
            for idx in start..stop {
                let h = edt[idx].len();

                let nodes_buf = if idx + 1 == diameter {
                    // an accepting state node
                    Vec::<NodeIndex>::from([automaton.add_node(n)])
                } else {
                    (0..h)
                        .map(|node_height: usize| {
                            let current_node = automaton.add_node(n);
                            n += 1;
                            current_node
                        })
                        .collect::<Vec<NodeIndex>>()
                };

                if idx == start {
                    previous_nodes
                        .iter()
                        .for_each(|previous_node_id: &NodeIndex| {
                            nodes_buf.iter().enumerate().for_each(|(h, node_index)| {
                                automaton.add_edge(
                                    *previous_node_id,
                                    *node_index,
                                    String::from(edt.base_at([idx, h]) as char),
                                );
                            })
                        })
                } else if idx + 1 == diameter {
                    previous_nodes
                        .iter()
                        .enumerate()
                        .for_each(|(h, previous_node_id)| {
                            nodes_buf.iter().for_each(|node_index| {
                                automaton.add_edge(
                                    *previous_node_id,
                                    *node_index,
                                    String::from(edt.base_at([idx, h]) as char),
                                );
                            })
                        })
                } else {
                    nodes_buf.iter().enumerate().for_each(|(h, node_index)| {
                        automaton.add_edge(
                            previous_nodes[h],
                            *node_index,
                            String::from(edt.base_at([idx, h]) as char),
                        );
                    })
                }

                previous_nodes = nodes_buf;
            }
        }
    }

    automaton
}

/*
             q
    0   ...    j   ...   n
    |--------------------|
    |                    |
  . |                    |
  . |                    |
w . |                    |
  i |                    |
  . |                    |
  . |                    |
  . |                    |
    |--------------------|
    m
 */

pub fn intersect(w: &Graph<usize, String>, q: &Graph<usize, String>) -> bool {
    let n = q.node_count(); // cols
    let m = w.node_count(); // rows

    // array takes (cols, row) tuple
    let mut matrix: Array2<i32> = Array::zeros((m, n)); // cols, rows
                                                        // let mut matrix: Array2<i32> = Array::zeros((n, m)); // cols, rows

    matrix[[0, 0]] = 1;

    let w_vec = w.node_indices().collect::<Vec<NodeIndex>>();
    let q_vec = q.node_indices().collect::<Vec<NodeIndex>>();

    if LOG_LEVEL > 4 {
        dbg!(m, n, w_vec.len(), q_vec.len());
    }

    // rows
    for i in 0..m {
        let current_w = w
            .edges_directed(w_vec[i], petgraph::Incoming)
            .map(|in_w| {
                (
                    in_w.weight().clone(),
                    in_w.source().index(),
                    in_w.target().index(),
                )
            })
            .collect::<HashSet<(String, usize, usize)>>();

        if LOG_LEVEL > 4 {
            eprintln!("{} => {:?}", i, current_w);
        }

        // cols
        for j in 0..n {
            // An incoming label can either be * and/or A, T, C or G.
            let current_q = q
                .edges_directed(q_vec[j], petgraph::Incoming)
                .map(|in_q| {
                    (
                        in_q.weight().clone(),
                        in_q.source().index(),
                        in_q.target().index(),
                    )
                })
                .collect::<HashSet<(String, usize, usize)>>();

            for (i_str, i_src, i_tgt) in current_w.iter() {
                for (j_str, j_src, j_tgt) in current_q.iter() {
                    if (matrix[(*i_src, *j_src)] == 1
                        || (matrix[(*i_src, *j_tgt)] == 1 && *i_str == String::from("*")))
                        && (*i_str == String::from("*")
                            || *j_str == String::from("*")
                            || *j_str == *i_str)
                    {
                        matrix[(*i_tgt, *j_tgt)] = 1;
                        break;
                    }
                }
            }
        }
    }

    if LOG_LEVEL > 4 {
        utils::visualize_matrix(&matrix, "matrix.png");
    }

    matrix[(m - 1, n - 1)] == 1
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
    fn test_contains_intersect() {
        let ed_string_w = "{AT,TC}{ATC,T}";
        let ed_string_q = "{TT,T,}{CT,T}";

        let edt_w = EDT::from_str(ed_string_w);
        let n = edt_w.size();
        println!("\n{edt_w}");

        let automaton_w = build_automaton(edt_w);
        print_dot(&automaton_w, ed_string_w);

        let edt_q = EDT::from_str(ed_string_q);
        let m = edt_q.size();
        println!("\n{edt_q}");

        let automaton_q = build_automaton(edt_q);
        print_dot(&automaton_q, ed_string_q);

        assert_eq!(intersect(&automaton_w, &automaton_q), true);
    }

    #[test]
    fn test_no_intersect() {
        let ed_string_w = "{AT,TC}{ATC,T}";
        let ed_string_q = "{TT,A}{CT,T}";

        let edt_w = EDT::from_str(ed_string_w);
        let n = edt_w.size();
        println!("\n{edt_w}");

        let automaton_w = build_automaton(edt_w);
        print_dot(&automaton_w, ed_string_w);

        let edt_q = EDT::from_str(ed_string_q);
        let m = edt_q.size();
        println!("\n{edt_q}");

        let automaton_q = build_automaton(edt_q);
        print_dot(&automaton_q, ed_string_q);

        assert_eq!(intersect(&automaton_w, &automaton_q), false);
    }
}
