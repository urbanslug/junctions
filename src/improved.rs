use eds::EDT;
use generalized_suffix_tree::GeneralizedSuffixTree;

fn build_generalized_suffix_tree(text: Vec<&str>) {
    let mut tree = GeneralizedSuffixTree::new();

    text.into_iter().enumerate().for_each(|(idx, s)| {
        tree.add_string(String::from(s), (idx as u8) as char);
    });
    tree.pretty_print();
}

pub fn intersect(w: &EDT, q: &EDT) -> bool {
    false
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::utils::print_dot;

    // TODO: maybe merge the naive and improved tests?
    #[test]
    fn test_contains_intersect() {
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
    fn test_search_using_st() {
        let text = Vec::from(["ATCAT", "ATCAG"]);
        build_generalized_suffix_tree(text);
    }
}
