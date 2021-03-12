mkdir -p MyWindow14.app/Contents/MacOS

clang++ -o MyWindow14.app/Contents/MacOS/MyWindow14 MyWindow14.mm -framework Cocoa -framework QuartzCore -framework OpenGL
