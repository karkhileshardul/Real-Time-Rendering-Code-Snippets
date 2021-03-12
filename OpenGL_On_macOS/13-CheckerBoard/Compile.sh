mkdir -p MyWindow13.app/Contents/MacOS

clang++ -o MyWindow13.app/Contents/MacOS/MyWindow13 MyWindow13.mm -framework Cocoa -framework QuartzCore -framework OpenGL
