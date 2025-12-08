### Build for Linux
---
```Shell
g++ main.cpp glad/glad.c stb_image.cpp -o myogl \
-std=c++17 \
-I./ \
-I./glad \
-I./glm \
-I./assimp/include \
-L./assimp/lib \
-lassimp -lminizip -lz \
-lglfw \
-lX11 -lXrandr -lXi -lXcursor -lXxf86vm \
-lGL -ldl -lpthread
```

### Build for Windows (MinGW)
---
```Shell
g++ main.cpp glad\glad.c stb_image.cpp -o myogl.exe ^
-std=c++17 ^
-I. ^
-Iglad ^
-Iglm ^
-Iassimp\include ^
-Lassimp\lib ^
-lassimp ^
-lminizip ^
-lz ^
-lglfw3 ^
-lopengl32 ^
-lgdi32 ^
-luser32 ^
-lkernel32
```

### Run
---
```Shell
./myogl
```