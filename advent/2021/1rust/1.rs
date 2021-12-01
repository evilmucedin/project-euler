fn get_input() -> String {
    let mut buffer = String::new();
    std::io::stdin().read_line(&mut buffer).expect("Failed");
    buffer
}

fn main() {
    let mut count = 0;
    let mut n = get_input().trim().parse::<i32>().unwrap();
    loop {
        match get_input().trim().parse::<i32>() {
            Ok(nn) => {
                if nn > n {
                    count += 1;
                }
                n = nn;
            },
            Err(_) => {
                break;
            }
        }
    }
    println!("{}", count);
}
