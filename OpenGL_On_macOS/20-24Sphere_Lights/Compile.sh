mkdir -p MyWindow20.app/Contents/MacOS

clang++ -o MyWindow20.app/Contents/MacOS/MyWindow20 MyWindow20.mm -framework Cocoa -framework QuartzCore -framework OpenGL
