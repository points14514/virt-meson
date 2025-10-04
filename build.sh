# This is a complete compilation series, It may compile the file you want
# Package is required for installation
sudo apt-get install libgtk-3-dev
# Compile
gcc -o virt-meson virt-meson.c `pkg-config --cflags --libs gtk+-3.0`
# Mutation may fail, please be patient
echo "The mutation may have failed,Or it worked,But I count you as a success"
