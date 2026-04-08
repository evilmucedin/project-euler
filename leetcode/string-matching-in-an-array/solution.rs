impl Solution {
    pub fn string_matching(words: Vec<String>) -> Vec<String> {
        let mut result = Vec::new();
        let n = words.len();

        for i in 0..n {
            for j in 0..n {
                // Check if it's a different word
                if i != j {
                    // Check if words[i] is a substring of words[j]
                    if words[j].contains(&words[i]) {
                        result.push(words[i].clone());
                        break; // Found a match, no need to check other words
                    }
                }
            }
        }
        
        result
    }
}
