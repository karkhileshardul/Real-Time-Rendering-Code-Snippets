mkdir -p MyWindow12.app/Contents/MacOS

clang++ -o MyWindow12.app/Contents/MacOS/MyWindow12 MyWindow12.mm -framework Cocoa -framework QuartzCore -framework OpenGL
