use crate::{types, utils::compute_properties};
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
pub fn intersect(w: &EDT, q: &EDT) -> bool {
    // TODO: memoize

    // compute the edt sets
    let properties_w: types::EdtSets = utils::iterate_sets(w);
    let properties_q: types::EdtSets = utils::iterate_sets(q);

    let size_w = w.size(); // N
    let size_q = q.size(); // M
    let m = properties_q.len();
    let n = properties_w.len();

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

    let properties_w_computed = compute_properties(w, &properties_w);
    let i_starts = properties_w_computed.starts;
    let i_ends = properties_w_computed.ends;
    eprintln!("i_starts: {:?} i_ends: {:?}", i_starts, i_ends);

    // TODO: Continue i+1 from earliest reached  prefix
    for i in 0..n {
        // extract the actual strings in the ith set
        let w_i_strings: Vec<String> = utils::set_members(w, &properties_w, i);

        // insert underscores into a string
        let multi_string = utils::seperate_strings_with_underscores(&w_i_strings);
        let underscore_separated_string: String = multi_string.underscore_separated_string;
        // a rank "bitvector" that counts the number of undscrores in a string so far
        let underscores_count_vec: Vec<usize> = multi_string.rank_bit_vector;

        let st = SuffixTable::new(underscore_separated_string.clone());

        // eprintln!("letter start in N: {}", i_letter_start);

        eprintln!("\tw (blue)");
        for s in w_i_strings.iter() {
            let p = st.positions(s);
            eprintln!("\tself spell:");
            eprintln!("\t\tstring:{s} positions {:?}", p);
        }

        for j in 0..m {
            // TODO: rename q_j_strings
            let q_j_strings: Vec<String> = utils::set_members(q, &properties_q, j);

            eprintln!("(i, j) ({i}, {j})");
            eprintln!("\tunderscores bit vector");
            eprintln!("\t\t{:?}", underscores_count_vec);

            eprintln!("\tq (red)");
            for s in q_j_strings.iter() {
                //let p: &[u32] = st.positions(s);
                // let mut s = s.clone();
                // s.push('_');
                let positions: &[u32] = st.positions(s);
                eprintln!(
                    "\tquery: {} text: {:?} positions {:?}",
                    s, &underscore_separated_string, positions
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
                        && underscore_separated_string.as_bytes()[start_position - 1] as char != '_'
                    {
                        continue;
                    }

                    let i_letter_start = i_starts[i][0];

                    let actual_start = if start_position == 0 {
                        start_position + i_letter_start
                    } else {
                        start_position + i_letter_start - underscores_count_vec[start_position]
                    };

                    let matrix_col =
                        if i_letter_start == 0 && underscores_count_vec[start_position] == 0 {
                            actual_start + s.len()
                        } else {
                            actual_start + s.len() - 1
                        };

                    eprintln!(
                        "\t\tactual start {} end_position {}",
                        actual_start, matrix_col
                    );

                    // TODO: add condition, check prev col

                    // Checks previous row to see if any active prefix can be extended
                    let any_active_prefix = || -> bool {
                        if i == 0 {
                            return false;
                        }
                        i_ends[i - 1]
                            .iter()
                            .fold(false, |acc, idx| acc || w_matrix[(j - 1, *idx)] == 1)
                    };

                    if j == 0 && i == 0 {
                        eprintln!("\t\t({j}, {matrix_col}) <- 1 ");
                        w_matrix[(j, matrix_col)] = 1;
                    } else if j > 0 && any_active_prefix() {
                        eprintln!("\t\t({j}, {matrix_col}) <- 1 ");
                        w_matrix[(j, matrix_col)] = 1;
                    } else {
                        ()
                    };
                }
            }

            // let st = build_generalized_suffix_tree(&w_i_strings);
        }
    }

    let mut q_matrix: Array2<i32> = Array::zeros((n, size_q)); // rows, cols

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

    let properties_q_computed = compute_properties(q, &properties_q);
    let j_starts = properties_q_computed.starts;
    let j_ends = properties_q_computed.ends;

    for j in 0..m {
        // extract the actual strings in the j_th set
        let q_j_strings: Vec<String> = utils::set_members(q, &properties_q, j);

        // insert underscores into a string
        let multi_string = utils::seperate_strings_with_underscores(&q_j_strings);
        let underscore_separated_string: String = multi_string.underscore_separated_string;
        // a rank "bitvector" that counts the number of undscrores in a string so far
        let underscores_count_vec: Vec<usize> = multi_string.rank_bit_vector;

        let st = SuffixTable::new(underscore_separated_string.clone());

        eprintln!("\tw (blue)");
        for s in q_j_strings.iter() {
            let p = st.positions(s);
            eprintln!("\tself spell:");
            eprintln!("\t\tstring:{s} positions {:?}", p);
        }

        for i in 0..n {
            // TODO: rename q_j_strings
            let w_i_strings: Vec<String> = utils::set_members(w, &properties_w, i);

            eprintln!("(i, j) ({i}, {j})");
            eprintln!("\tunderscores bit vector");
            eprintln!("\t\t{:?}", underscores_count_vec);

            eprintln!("\tq (red)");
            for s in w_i_strings.iter() {
                let positions: &[u32] = st.positions(s);
                eprintln!(
                    "\tquery: {} text: {:?} positions {:?}",
                    s, &underscore_separated_string, positions
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
                        && underscore_separated_string.as_bytes()[start_position - 1] as char != '_'
                    {
                        continue;
                    }

                    let j_letter_start = j_starts[i][0];

                    let actual_start = if start_position == 0 {
                        start_position + j_letter_start
                    } else {
                        start_position + j_letter_start - underscores_count_vec[start_position]
                    };

                    let matrix_col =
                        if j_letter_start == 0 && underscores_count_vec[start_position] == 0 {
                            actual_start + s.len()
                        } else {
                            actual_start + s.len() - 1
                        };

                    eprintln!(
                        "\t\tactual start {} end_position {}",
                        actual_start, matrix_col
                    );

                    // TODO: add condition, check prev col

                    // Checks previous row to see if any active prefix can be extended
                    let any_active_prefix = || -> bool {
                        if j == 0 {
                            return false;
                        }
                        j_ends[j - 1]
                            .iter()
                            .fold(false, |acc, idx| acc || q_matrix[(i - 1, *idx)] == 1)
                    };

                    if j == 0 && i == 0 {
                        eprintln!("\t\t({i}, {matrix_col}) <- 1 ");
                        q_matrix[(i, matrix_col)] = 1;
                    } else if i > 0 && any_active_prefix() {
                        eprintln!("\t\t({i}, {matrix_col}) <- 1 ");
                        q_matrix[(i, matrix_col)] = 1;
                    } else {
                        ()
                    };
                }
            }
        }
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
