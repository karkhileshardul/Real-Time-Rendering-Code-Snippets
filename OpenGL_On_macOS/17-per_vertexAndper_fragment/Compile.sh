mkdir -p MyWindow17.app/Contents/MacOS

clang++ -o MyWindow17.app/Contents/MacOS/MyWindow17 MyWindow17.mm -framework Cocoa -framework QuartzCore -framework OpenGL
