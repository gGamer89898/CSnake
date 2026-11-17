# CSnake
A really small terminal snake game I made with C++

# Compiling
To run this, you have to compile it yourself. Currently, it is confirmed to run on MacOS Arm x64, it’s likely to run on Linux and I’m making a windows version.
## Requirements:
* gcc installed
* C++ library ncurses
## Steps:
1. Open a terminal
2. Write `cd /path/to/CSnake/` and press enter
3. Write `gpp -g ./main.cpp -o ./CSnake -lncurses` and press enter
4. (optional) put the binary created on any directory in the `$PATH` variable or edit `~/.bashrc` (Linux) or `~.zsh` (MacOS) to add the line `alias csnake=“/path/to/CSnake/CSnake`
5. You can run it by writing the binary path on the terminal. If it doesn’t run, try `chmod +x ./CSnake`
