genrule(
    name = "solution",
    srcs = ["solution.rs"],
    outs = ["solution.rlib"],
    cmd = "MAIN=$(@D)/solution_main.rs; " +
          "cp $(location solution.rs) $(@D)/solution.rs; " +
          "echo 'mod solution;' > $$MAIN; " +
          "echo 'fn main() { let _ = solution::Solution::string_matching(vec![\"mass\".to_string(), \"as\".to_string(), \"hero\".to_string(), \"superhero\".to_string()]); }' >> $$MAIN; " +
          "rustc $$MAIN --edition=2021 -o $@",
)

