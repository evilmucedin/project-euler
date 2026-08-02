# @param {String} text
# @return {String}
def reorder_spaces(text)
  total_spaces = text.count(' ')
    words = text.split
      
        if words.length == 1
                return words[0] + ' ' * total_spaces
                  end

                    gap_spaces = total_spaces / (words.length - 1)
                      rem_spaces = total_spaces % (words.length - 1)

                        words.join(' ' * gap_spaces) + ' ' * rem_spaces
                        end
