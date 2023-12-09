# **FISSES NG**

## ***Runtime requirements***

- [Skyrim Script Extender (SKSE)](https://skse.silverlock.org/)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)

## ***Build requirements***

- [CMake](https://cmake.org/)
- [vcpkg](https://vcpkg.io/en/)
- [Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/community/)
- [CommonLibSSE-NG](https://github.com/CharmedBaryon/CommonLibSSE-NG)

## ***Building***

In `Developer Command Prompt for VS 2022` or `Developer PowerShell for VS 2022`, run:

~~~
.\cmake\build.ps1
~~~

or

~~~
.\cmake\build.ps1 -buildPreset ALL-relwithdebinfo
~~~

or

~~~
.\cmake\build.ps1 -buildPreset ALL-debug
~~~

or

~~~
cmake -B build -S . --preset ALL --fresh
cmake --build build --preset ALL-release
~~~

Then get the .dll in build/Release, or the .zip (ready to install using mod manager) in build.

## ***Credits***

TerenceYao at [Nexus Mods](https://www.nexusmods.com/skyrimspecialedition/mods/13956) for SE/AE version

Lord Conti at [Nexus Mods](https://www.nexusmods.com/skyrim/mods/48265/) for original LE version
