genrule(
    name = "solution",
    srcs = ["solution.rs"],
    outs = ["solution.rlib"],
    cmd = "rustc $(location solution.rs) --crate-name solution --crate-type lib -o $@",
)

