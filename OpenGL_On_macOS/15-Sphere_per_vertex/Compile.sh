mkdir -p MyWindow15.app/Contents/MacOS

clang++ -o MyWindow15.app/Contents/MacOS/MyWindow15 MyWindow15.mm -framework Cocoa -framework QuartzCore -framework OpenGL
