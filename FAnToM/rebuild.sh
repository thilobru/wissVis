#sudo rm grundaufgaben_build -r
#mkdir grundaufgaben_build
cd grundaufgaben_build
cmake ../grundaufgaben_src 
ccmake . #-G %1 FANTOM_DIR=../prebuild
make
sudo make install