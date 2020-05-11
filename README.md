# Halma AI Game Playing Agent

A **C++11 programmed** AI will play the game of Halma, an adversarial game with some similarities to checkers. The game uses a 16x16 checkered gameboard. Each player starts with 19 game pieces clustered in diagonally opposite corners of the board. To win the game, a player needs to transfer all of their pieces from their starting corner to the opposite corner, into the positions that were initially occupied by the opponent. Read [this](https://en.wikipedia.org/wiki/Halma) article for further details on Halma and its rules.

Essentially, our agent will evaluate the gameboard and output the **best / optimal** move by using [Alpha Beta Pruning](https://en.wikipedia.org/wiki/Alpha_beta_pruning) Minimax algorithm.

PS. This was homework #2 of CSCI 561 - Fall 2019 - Foundations of Artificial Intelligence - under [Professor Laurent Itti](http://ilab.usc.edu/itti/).

## Setup

- [MinGW](https://sourceforge.net/projects/mingw/) is required in order to run the executable file.
- [Download](https://github.com/Matrix7689/Halma-AI/releases) this executable file (Windows Only).
- Run "halma_player.exe".
- The program will auto-generate an "output.txt" file with the results inside the text file.