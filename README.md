# RISC-V-Board-Game
Project with Professor Christopher Nitta in class ECS050

Brief Description:
In the project, I make a game that allowed two players to play different color tokens to win the game if there tokens are having 4 in 
a line(Horizontal, vertical, diagonal). There should be text mode and graphic mode that can be changed depend on user's choice. But due
to the variable name are repeadly used, text mode may be not functioning. (Also, some of the old algorithm that is wrong haven't been fixed
for the text mode but are fixed in graph mode). Due to the time limitations, I am unable to implement animations with the help of time 
interrupt. I tried it several times but failed. But I still make the board to show in front of the tokens so that it work toward like an
animation. Other than that, all the functions are wokring. Cmd will clear out the board and begin a new game. A and D will control the 
token to move left or right. U will drop the token. At the end, the system may cuase blink when running the program on a new machine 
for the first few times. But the blink of tokens will go away after several runs.

Potential flaws: I may be optimize my winning detection codes by using iteration. I should use different variable names for text modes
and graph modes at the beginnig so that it wouldn't messed up. 

Thank you for the summer! Hope you have a great vocation after this. 
