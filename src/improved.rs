use crate::types;
use eds::EDT;
use generalized_suffix_tree::GeneralizedSuffixTree;
use ndarray::{Array, Array2};

use suffix::SuffixTable;

use crate::utils;

fn build_generalized_suffix_tree(text: &Vec<String>) {
    let mut tree = GeneralizedSuffixTree::new();

    text.iter().enumerate().for_each(|(idx, s)| {
        tree.add_string(s.clone(), (idx as u8) as char);
    });
    tree.pretty_print();
}

pub fn intersect(w: &EDT, q: &EDT) -> bool {
    // TODO: memoize

    // compute the edt sets
    let properties_w: types::EdtSets = utils::iterate_sets(w);
    let properties_q: types::EdtSets = utils::iterate_sets(q);

    let n = properties_w.len();
    let size_w = w.size(); // N
    let size_q = q.size(); // M
    let m = properties_q.len();

    /*
                                  N
    |-----------------------------|
    |                             |
    |                             |
    |          W_matrix           |
    |                             |
    |-----------------------------|
    m
                                  M
    |-----------------------------|
    |                             |
    |                             |
    |          Q_matrix           |
    |                             |
    |-----------------------------|
    n
     */
    let mut q_matrix: Array2<i32> = Array::zeros((n, size_q)); // rows, cols
    let mut w_matrix: Array2<i32> = Array::zeros((m, size_w)); // rows, cols

    let mut stop: usize = size_w - 1;
    let mut prev_len: usize = 0;
    let accepting_w_cols: Vec<usize> = utils::set_members(w, &properties_w, n - 1)
        .iter()
        .rev()
        .map(|s: &String| {
            stop = stop - prev_len;
            prev_len = s.len();
            stop
        })
        .collect();

    let mut stop: usize = size_q - 1;
    let mut prev_len: usize = 0;
    let accepting_q_cols: Vec<usize> = utils::set_members(q, &properties_q, m - 1)
        .iter()
        .rev()
        .map(|s: &String| {
            stop = stop - prev_len;
            prev_len = s.len();
            stop
        })
        .collect();

    eprintln!(
        "accepting w cols {:?} \naccepting q cols {:?}",
        accepting_w_cols, accepting_q_cols
    );

    // TODO: move to utils::iterate_sets

    let mut i_start = 0; // within N
    let mut j_start = 0; // within M

    for i in 0..n {
        let w_i_strings: Vec<String> = utils::set_members(w, &properties_w, i);

        // insert underscores into a string
        let w_i_gen_string = w_i_strings
            .clone()
            .iter_mut()
            .map(|s: &mut String| {
                s.push('_');
                s.chars()
            })
            .flatten()
            .collect::<String>();

        // a "bitvector" that counts the number of undscrores in a string so far
        let mut x: usize = 0;
        let underscores_count_vec: Vec<usize> = w_i_gen_string
            .chars()
            .map(|ch| {
                if ch == '_' {
                    x = x + 1;
                }
                x
            })
            .collect();

        let i_ends = w_i_strings
            .iter()
            .map(|s| {
                i_start += if i_start == 0 { s.len() - 1 } else { s.len() };
                i_start
            })
            .collect::<Vec<usize>>();

        eprintln!("i_ends: {:?}", i_ends);

        let st = SuffixTable::new(w_i_gen_string.clone());

        for j in 0..m {
            // TODO: rename q_j_strings
            let w_j_strings: Vec<String> = utils::set_members(q, &properties_q, j);

            eprintln!("(i, j) ({i}, {j})");
            eprintln!("\tunderscores bit vector");
            eprintln!("\t\t{:?}", underscores_count_vec);

            eprintln!("\tw (blue)");
            for s in w_i_strings.iter() {
                let p = st.positions(s);
                eprintln!("\tself spell:");
                eprintln!("\t\tstring:{s} positions {:?}", p);
            }

            eprintln!("\tq (red)");
            for s in w_j_strings.iter() {
                //let p: &[u32] = st.positions(s);
                // let mut s = s.clone();
                // s.push('_');
                let positions: &[u32] = st.positions(s);
                eprintln!(
                    "\tquery: {} text: {:?} positions {:?}",
                    s, &w_i_gen_string, positions
                );

                // all occurences of s
                for position in positions.iter() {
                    let start_position = *position as usize;
                    // let end_position = start_position + s.len();

                    // TODO: what if we are cont in the next degenerate letter?
                    // either it is at start of letter or
                    if i == 0
                        && j == 0
                        && start_position > 0
                        && w_i_gen_string.as_bytes()[start_position - 1] as char != '_'
                    {
                        continue;
                    }

                    let actual_start = if start_position == 0 {
                        start_position + i_start
                    } else {
                        start_position + i_start - underscores_count_vec[start_position] - 1
                    };

                    let matrix_col = actual_start + s.len();

                    eprintln!(
                        "\t\tactual start {} end_position {}",
                        actual_start, matrix_col
                    );

                    // TODO: add condition, check prev col

                    let foo = || {
                        i_ends
                            .iter()
                            .fold(false, |acc, idx| acc || w_matrix[(j - 1, *idx)] == 1)
                    };

                    if j == 0 && i == 0 {
                        eprintln!("\t\t({j}, {matrix_col}) <- 1 ");
                        w_matrix[(j, matrix_col)] = 1;
                    } else if j > 0 && foo() {
                        eprintln!("\t\t({j}, {matrix_col}) <- 1 ");
                        w_matrix[(j, matrix_col)] = 1;
                    } else {
                        ()
                    };
                }
            }

            // let st = build_generalized_suffix_tree(&w_i_strings);
        }

        // furthest_w_char += w_i_strings.iter().fold(0, |acc, s| s.len() + acc);
    }

    if true {
        utils::visualize_matrix(&w_matrix, "imrpoved_w_matrix.png");
        utils::visualize_matrix(&q_matrix, "imrpoved_q_matrix.png");
    }

    // do we have at least one end state on the q matrix
    let end_q_matrix = accepting_q_cols
        .into_iter()
        .fold(false, |acc, col| acc || q_matrix[(n - 1, col)] == 1);

    let end_w_matrix = accepting_w_cols
        .into_iter()
        .fold(false, |acc, col| acc || w_matrix[(m - 1, col)] == 1);

    end_q_matrix && end_w_matrix
}

#[cfg(test)]
mod tests {
    use super::*;

    mod has_intersect {
        use super::super::*;

        // TODO: maybe merge the naive and improved tests?
        #[test]
        fn test_entire_strings() {
            let ed_string_w = "{AT,TC}{ATC,T}";
            let ed_string_q = "{TC,G}{CT,T}";

            let edt_w = EDT::from_str(ed_string_w);
            println!("\n{edt_w}");

            let edt_q = EDT::from_str(ed_string_q);
            println!("\n{edt_q}");

            assert_eq!(intersect(&edt_w, &edt_q), true);

            let ed_string_w = "{AT,TC}{ATC,T}";
            let ed_string_q = "{TC,T}{CT,T}";

            let edt_w = EDT::from_str(ed_string_w);

            let edt_q = EDT::from_str(ed_string_q);

            assert_eq!(intersect(&edt_w, &edt_q), true);
        }

        #[test]
        fn test_substrings() {
            let ed_string_w = "{AT,TC}{ATC,T}";
            let ed_string_q = "{TT,T}{CT,T}";

            let edt_w = EDT::from_str(ed_string_w);
            println!("\n{edt_w}");

            let edt_q = EDT::from_str(ed_string_q);
            println!("\n{edt_q}");

            assert_eq!(intersect(&edt_w, &edt_q), true);
        }

        #[test]
        fn test_epsilons() {
            let ed_string_w = "{AT,TC}{ATC,T}";
            let ed_string_q = "{TT,T,}{CT,T}";

            let edt_w = EDT::from_str(ed_string_w);
            println!("\n{edt_w}");

            let edt_q = EDT::from_str(ed_string_q);
            println!("\n{edt_q}");

            assert_eq!(intersect(&edt_w, &edt_q), true);
        }
    }

    mod lacks_intersect {
        use super::super::*;

        #[test]
        fn test_no_intersect() {
            let ed_string_w = "{AT,TC}{ATC,T}";
            let ed_string_q = "{TT,A}{CT,T}";

            let edt_w = EDT::from_str(ed_string_w);
            let n = edt_w.size();
            println!("\n{edt_w}");

            // let automaton_w = build_automaton(edt_w);
            // print_dot(&automaton_w, ed_string_w);

            let edt_q = EDT::from_str(ed_string_q);
            let m = edt_q.size();
            println!("\n{edt_q}");

            // let automaton_q = build_automaton(edt_q);
            // print_dot(&automaton_q, ed_string_q);

            assert_eq!(intersect(&edt_w, &edt_q), true);
        }
    }

    #[test]
    #[ignore]
    fn test_search_using_st() {
        // let text = Vec::from(["ATCAT", "ATCAG"]);
        // build_generalized_suffix_tree(text);
    }
}
