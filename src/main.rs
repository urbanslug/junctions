use eds::{EDT, Letter};
use petgraph::Graph;
use petgraph::graph::NodeIndex;
use std::collections::VecDeque;
use std::collections::HashSet;
use std::slice;

fn build_compacted_trie(edt: EDT) -> Graph<(usize, usize, usize), Vec<u8>>{
    let mut automaton = Graph::<(usize, usize, usize), Vec<u8>>::new();

    let diameter =  edt.w();
    let mut solid_iterator: slice::Iter<(usize, usize)> =  edt.get_solid_intervals().iter();
    let mut degenerates_iterator: slice::Iter<(usize, usize)> = edt.get_degenerate_letters().iter();

    // queue to sync the results of the two iterators in ascending order
    let mut queue: VecDeque<(usize, usize, Letter)> = VecDeque::new();

    // temporary buffer to store the nodes from the previous iteration of the loop
    // from_nodes
    let mut stored = HashSet::<(NodeIndex, usize)>::new();

    // Loop over the contents of the queue and build an automaton out of them
    loop {
        let (solid_start, solid_end) = *solid_iterator.next().unwrap_or(&(usize::MAX, usize::MAX));
        let (degenerate_start, degenerate_end) = *degenerates_iterator.next().unwrap_or(&(usize::MAX, usize::MAX));

        let (start, stop, _) = if queue.front().is_some() {
            if degenerate_start != usize::MAX {
                queue.push_back((degenerate_start, degenerate_end, Letter::Degenerate));
            }
            if solid_start != usize::MAX {
                queue.push_back((solid_start, solid_end, Letter::Solid));
            }

            queue.pop_front().unwrap()
        } else if solid_start < degenerate_start {
            if degenerate_start != usize::MAX {
                queue.push_back((degenerate_start, degenerate_end, Letter::Degenerate));
            }
            (solid_start, solid_end, Letter::Solid)
        } else {
            if solid_start != usize::MAX {
                queue.push_back((solid_start, solid_end, Letter::Solid));
            }
            (degenerate_start, degenerate_end, Letter::Degenerate)
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

            stored.retain(|(_, v)|  *v == stop-1);

            nodes_added.iter().for_each(|v| { stored.insert(*v); } );
            automaton.extend_with_edges(edges);
        }

        if queue.is_empty() {
            break;
        }
    }
    automaton
    
}

fn main() {
    let ed_string = "A{T,G}{C,A}{T,A}TC";
    // let ed_string = EDT::from_str("ACTA{ATC,CGA}{ACGT,GCGC}A{CTA,C,}A");
    // let edt = EDT::from_str("ACTA{ATC,CGA}C{ACGT,GCGC}A");

    build_compacted_trie(EDT::from_str(ed_string));
    
}


#[cfg(test)]
mod tests {
    use super::*;
    use petgraph::dot::{Dot, Config};

    #[test]
    fn test_build_compacted_trie() {
        let ed_string = "A{T,G}{C,A}{T,A}TC";
        // let ed_string = EDT::from_str("ACTA{ATC,CGA}{ACGT,GCGC}A{CTA,C,}A");
        // let edt = EDT::from_str("ACTA{ATC,CGA}C{ACGT,GCGC}A");

        let edt = EDT::from_str(ed_string);
        println!("\n{edt}");

        let automaton = build_compacted_trie(edt);
        println!("{:?}", Dot::with_config(&automaton, &[Config::EdgeNoLabel]));

    }
}
