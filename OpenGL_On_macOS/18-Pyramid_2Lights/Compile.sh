mkdir -p MyWindow18.app/Contents/MacOS

clang++ -o MyWindow18.app/Contents/MacOS/MyWindow18 MyWindow18.mm -framework Cocoa -framework QuartzCore -framework OpenGL
