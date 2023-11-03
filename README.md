# 15 PUZZLE
Its a simple puzzle where you have to organize the squares of a 4x4 grid, beeing only able to move
one of the 15 squares in the blank position.
Despite the semplicity of the game, finding an optimal solution (aka doing the fewest number of 
moves to reorganize the grid) is a NP-HARD: `O((N^2)!/2)`.  
- In order to minimize the time spent looking for the optimal solution I started implementing
`A*` algorithm using **Manhattan Distance** as the heuristic on NxM grids. The program ran good 
on some example of the 4x4 grid, but very poorly on harder ones.
- So I decided to narrow my focus only on 15 puzzle (after having calculated the extremly high 
complexity).
- I wrote `A* + Meet in the middle` algorithm, improving the speed of the program by some 
orders of magnitude.
- Beeing the algorithm still to slow, I tried using the **Walking Distance** as the heuristic for
the `A* algorithm`, finding this as a suggestion on a blog.
- Unfortunately this heuristic + `Meet in the middle` got the performance worst (and so you won't 
find this algorithm in the repo).
- Furthermore the memory used to run those programs on the hardest case was much more than the 16
GigaBytes RAM of my laptop. So I wrote `IDA*` algorithm with both heuristic. Despite the improvment
with memory usage, the program was noticeably slower even on simpler problems
- Finally, while I was not able to crack the 80-moves puzzle, I wrote a `binary search`, that looked
for the best solution possible, within a limited range of position processed per step. The results 
are not so bad: less than 15% away from the best solution with the worst case, and many 100% 
accuracy on simpler puzzles

P.S. I find it satisfying to stare at the puzzle while the solution is displayed 
