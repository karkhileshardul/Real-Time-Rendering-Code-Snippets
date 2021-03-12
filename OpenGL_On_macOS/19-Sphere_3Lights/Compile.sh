mkdir -p MyWindow19.app/Contents/MacOS

clang++ -o MyWindow19.app/Contents/MacOS/MyWindow19 MyWindow19.mm -framework Cocoa -framework QuartzCore -framework OpenGL
