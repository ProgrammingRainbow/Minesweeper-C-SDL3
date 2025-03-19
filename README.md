![Screenshot](screenshot.png)

# Minesweeper (C - SDL3)
Uncover all non mine/flower tiles to win. Left click on a square to uncover it. Right click on a tile to mark it as a mine or with a question mark. These marks are purely for the user keep track of mines or unknown squares. They prevent that tile being uncovered by a left click. The top left number is the number of mines on the board minus the number of flags placed. The top right number is the elapsed time. Any numbered square holds the number of how many mines are immediately touching that square. The first turn may not end the game in a win or lose situation.

# ArchLinux instructions.
You will need to make sure SDL3 and SDL3_image is installed.
```
sudo pacman -S --needed base-devel sdl3
```
```
cd
git clone https://aur.archlinux.org/sdl3_image-git.git
sdl3_image-git
makepkg -i
```
```
cd
git clone https://github.com/ProgrammingRainbow/Beginners-Guide-to-SDL3-in-C
cd Beginners-Guide-to-SDL3-in-C
make run
```
The Makefile also supports these commands:
```
make rebuild
make clean
make release
make debug
CC=clang SRC_DIR=Video8 make clean run
```
# Controls
1 through 8 - Change the theme of the game.\
Q, W, E, R, T - Change size from Tiny to Huge.\
A, S, D, F - Change difficulty from Easy to Very Hard\
Left Click on tile to uncover.\
Left Clock on Face to reset.\
Right Click on tile to mark.\
B - Changes size. \
N - Reset. \
Escape - Quits
