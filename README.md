# txtrtool
Decodes and encodes TXTR files from Metroid Prime 1, 2: Echoes, and 3: Corruption.

## Table of Contents
- [Info](#info)
- [Building](#building)
- [Credits](#credits)

## Info
TODO: Info

## Building

### Linux
1. Install dependencies: `cmake ninja clang` via your package manager.
2. Clone this repo: `git clone --recursive https://github.com/xchellx/txtrtool.git && cd txtrtool`.
3. Build the project: `cmake -S . -B build -DCMAKE_BUILD_TYPE=<RELEASE_TYPE> -G Ninja -DSTBIR_SSE2:BOOL=ON -DSTBIR_AVX:BOOL=ON -DSTBIR_AVX2:BOOL=ON -DBUILD_SQUISH_WITH_SSE2:BOOL=ON` where `<RELEASE_TYPE>` would be either `Release` or `Debug`. Specify `-DTXTRTOOL_NOASAN:BOOL=ON` to not include ASAN (address sanitizer) on a `Debug` build.
4. Compile the project: `cmake --build build`.
5. The resulting binary should be in `build` directory, named `txtrtool`.

### Windows
1. Go to [MSYS2's website](https://www.msys2.org) and follow directions `1-4` on the website and uncheck `Run MSYS2 now` at step 4 on the website.
2. Press the `Windows Key` (or click the Windows icon) and search for `msys2 clang64`, click the result titled `MSYS2 CLANG64`.
3. Update the package database: `pacman -Syu`. Be patient. If prompted, respond with `Y`. If the prompt is `:: To complete this update all MSYS2 processes including this terminal will be closed. Confirm to proceed [Y/n]`, then respond with `Y` which will close the window then repeat steps `2` and `3`.
4. Install dependencies: `pacman -S mingw-w64-clang-x86_64-cmake mingw-w64-clang-x86_64-ninja mingw-w64-clang-x86_64-clang`.
5. Clone this repo: `git clone --recursive https://github.com/xchellx/txtrtool.git && cd txtrtool`.
6. Build the project: `cmake -S . -B build -DCMAKE_BUILD_TYPE=<RELEASE_TYPE> -G Ninja -DSTBIR_SSE2:BOOL=ON -DSTBIR_AVX:BOOL=ON -DSTBIR_AVX2:BOOL=ON -DBUILD_SQUISH_WITH_SSE2:BOOL=ON` where `<RELEASE_TYPE>` would be either `Release` or `Debug`.
7. Compile the project: `cmake --build build`.
8. The resulting binary should be in `build` directory, named `txtrtool.exe`.
9. To run `txtrtool.exe`, do either `A`, `B`, or `C` where `<INSTALL_PATH_OF_MSYS2>` is where you installed MSYS2 to:
    - A. If testing for development inside the MSYS2 CLANG64 terminal: Simply run `txtrtool.exe` with a full or relative path prefix.
    - B. If testing for development outside the MSYS2 CLANG64 terminal: add `<INSTALL_PATH_OF_MSYS2>\clang64\bin` to your `PATH` environment variable.
    - C. For redistribution: copy `libc++.dll` from `<INSTALL_PATH_OF_MSYS2>\clang64\bin` to the same location as `txtrtool.exe`.
        - **NOTE:** Distributing libstdc++-\*.dll violates the GPL license! **Only distribute libc++.dll** (when compiling with MSYS2 CLANG64)!

### Additional
If cmake files are changed in a way that makes the cache out of date, `[ -d build ] && rm -r build` will refresh things (this will require another build). Additionally, I personally wrote and use these for ease of compiling in my `.<shell>rc` file:
```sh
function cmake-clean { [ -d build ] && rm -r build ${@: 1}; }
function cmake-build { [ -n "$1" ] && cmake -S . -B build -DCMAKE_BUILD_TYPE=$1 ${@: 2}; }
function cmake-compile { [ -d build ] && cmake --build build ${@: 1}; }
```
This makes it simple to build with cmake from the terminal. For example, if I update cmake related files which would make the cache out of date, I'd just do `cmake-clean` then `cmake-build Debug` then `cmake-compile` for example. Easier and quicker to type.

Also, there is a `delcfg.sh` script to delete all `configure_file` configured files for this and subprojects (that use the similar pattern). This specifically will only work with this repo (and subrepos that use the same file pattern such as ones I author).

For copying dependencies in [Windows](#windows) step 9.C, there is a script `copylibs.sh` to help with that.

## Credits
- [unknown](https://github.com/hackyourlife) for the lots of help that they've given me developing this (and getting comfortable with C from the start).
- Retro Modding Wiki's documenation on the TXTR format [here](https://wiki.axiodl.com/w/index.php?title=TXTR_(Metroid_Prime)).
- Custom Mario Kart Wiiki for it's documentation of the image formats of the GX graphics system [here](https://wiki.tockdom.com/wiki/Image_Formats).
- Metaforce for its template code of expanding and compacting to and from 8 bits and it's documentation of the TXTR format [here](https://github.com/AxioDL/metaforce/blob/a413a010b50a53ebc6b0c726203181fc179d3370/DataSpec/DNACommon/TXTR.cpp).
- noclip.website for examples of how GX texture data is decoded [here](https://github.com/magcius/noclip.website/blob/6c47bb40e2c00ab22612f7f488eda0090c7a3be9/rust/src/gx_texture.rs).
- Luigis-Mansion-Blender-Toolkit for examples of how GX texture data is decoded [here](https://github.com/Astral-C/Luigis-Mansion-Blender-Toolkit/blob/3c596b1f64d7d506ed37e131cec6f95ec8df3766/gx_texture.py)
- PrimeWorldEditor for examples of how GX texture data is decoded [here](https://github.com/AxioDL/PrimeWorldEditor/blob/e5d1678ff626fd16c7160332e1e59003804f970a/src/Core/Resource/Factory/CTextureDecoder.cpp).
- Truevision Inc.'s documentation on the Targa (TGA) image format [here](https://www.dca.fee.unicamp.br/~martino/disciplinas/ea978/tgaffs.pdf) or [here](http://tfc.duke.free.fr/coding/tga_specs.pdf).
- FileFormat's documentation on the Targa (TGA) image format [here](https://docs.fileformat.com/image/tga/).
- Paul Bourke's documentation on the Targa (TGA) image format [here](http://www.paulbourke.net/dataformats/tga/).
- Wikipedia's documentation on the Targe (TGA) image format [here](https://wikipedia.org/wiki/Truevision_TGA).
- Just Solve The File Format Problem's documentation on the Targa (TGA) image format [here](http://fileformats.archiveteam.org/wiki/TGA)
- SweetScape's 010 Editor's Binary Template functionality for helping with TXTR and TGA format research [here](https://www.sweetscape.com/010editor/).
- Surma, for his article of various dithering algorithms [here](https://surma.dev/things/ditherpunk/).
- Tanner Helland, for his article of variois dithering algorithms [here](https://tannerhelland.com/2012/12/28/dithering-eleven-algorithms-source-code.html).
- Wikipedia for it's various pages on dithering algorithms such as [here](https://en.wikipedia.org/wiki/Error_diffusion), [here](https://en.wikipedia.org/wiki/Floyd%E2%80%93Steinberg_dithering), and [here](https://en.wikipedia.org/wiki/Atkinson_dithering).
