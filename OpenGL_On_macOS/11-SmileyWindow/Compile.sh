mkdir -p MyWindow11.app/Contents/MacOS

clang++ -o MyWindow11.app/Contents/MacOS/MyWindow11 MyWindow11.mm -framework Cocoa -framework QuartzCore -framework OpenGL
