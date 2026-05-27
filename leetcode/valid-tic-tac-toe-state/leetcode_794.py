def validTicTacToe(board):
    def win(player):
        for i in range(3):
            if all(board[i][j] == player for j in range(3)) or \
               all(board[j][i] == player for j in range(3)):
                return True
        if board[0][0] == board[1][1] == board[2][2] == player or \
           board[0][2] == board[1][1] == board[2][0] == player:
            return True
        return False
    
    moves_x = sum(row.count('X') for row in board)
    moves_o = sum(row.count('O') for row in board)
    
    if not (moves_x == moves_o or moves_x == moves_o + 1):
        return False
    if win('X') and moves_x != moves_o + 1:
        return False
    if win('O') and moves_x != moves_o:
        return False
    
    return True

# Example usage:
print(validTicTacToe(["O  ", "   ", "   "])) # Output: false
print(validTicTacToe(["XOX", " X ", "   "])) # Output: false
print(validTicTacToe(["XOX", "O O", "XOX"])) # Output: true