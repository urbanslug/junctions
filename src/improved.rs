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
    |          W_matrix           |
    |                             |
    |-----------------------------|
    n
     */
    let mut q_matrix: Array2<i32> = Array::zeros((n, size_q)); // rows, cols
    let mut w_matrix: Array2<i32> = Array::zeros((m, size_w)); // rows, cols

    for i in 0..n {
        for j in 0..m {
            let w_i_strings: Vec<String> = utils::set_members(w, &properties_w, i);
            let w_j_strings: Vec<String> = utils::set_members(q, &properties_q, j);

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

            eprintln!("bv {:?}", underscores_count_vec);

            let st = SuffixTable::new(w_i_gen_string);

            eprintln!("{i} {j}");
            eprintln!("blue");
            for s in w_i_strings.iter() {
                let p = st.positions(s);
                eprintln!("{s} {:?}", p);
            }

            eprintln!("red");
            for s in w_j_strings.iter() {
                let p: &[u32] = st.positions(s);
                let positions: &[u32] = st.positions(s);

                // all occurences of s
                for position in positions.iter() {
                    let start_position = *position as usize;
                    let end_position = start_position + s.len();

                    let actual_start = if start_position == 0 {
                        start_position
                    } else {
                        start_position - underscores_count_vec[start_position] - 1
                    };

                    let matrix_col = actual_start + end_position;

                    // TODO: add condition, check prev col

                    if j == 0 || w_matrix[(j - 1, actual_start)] == 1 {
                        eprintln!("({j}, {matrix_col}) ");
                        w_matrix[(j, end_position)] = 1;
                    }
                }

                eprintln!("{s} {:?}", p);
            }

            // let st = build_generalized_suffix_tree(&w_i_strings);
        }
    }

    if true {
        utils::visualize_matrix(&w_matrix, "imrpoved_w_matrix.png");
        utils::visualize_matrix(&q_matrix, "imrpoved_q_matrix.png");
    }

    q_matrix[(n - 1, size_q - 1)] == 1 && w_matrix[(m - 1, size_w - 1)] == 1
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::utils::print_dot;

    // TODO: maybe merge the naive and improved tests?
    #[test]
    fn test_has_intersect() {
        let ed_string_w = "{AT,TC}{ATC,T}";
        let ed_string_q = "{TT,T,}{CT,T}";

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

    #[test]
    #[ignore]
    fn test_search_using_st() {
        // let text = Vec::from(["ATCAT", "ATCAG"]);
        // build_generalized_suffix_tree(text);
    }
}
