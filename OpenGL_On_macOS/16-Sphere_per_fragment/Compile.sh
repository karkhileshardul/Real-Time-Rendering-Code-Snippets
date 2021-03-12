mkdir -p MyWindow16.app/Contents/MacOS

clang++ -o MyWindow16.app/Contents/MacOS/MyWindow16 MyWindow16.mm -framework Cocoa -framework QuartzCore -framework OpenGL
