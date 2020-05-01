# Morrowind Addon Randomizer

This is a program to randomize content for the game The Elder Scrolls III: Morrowind.

This is done through reading in original game data and plugins and then generating a unique omwaddon file to use with [OpenMW](https://openmw.org/). Please note that you will need to purchase and install both the original game (the original game discs will work but also online distribution like [GOG](https://www.gog.com/game/the_elder_scrolls_iii_morrowind_goty_edition) or [Steam](https://store.steampowered.com/app/22320/)) and then install and configure OpenMW for it to work correctly. Only the english release has been tested but all other language versions should work as well. It should also be possible to use the generated omwaddone file with tes3mp, OpenMW's official multiplayer platform, but this is also untested.

This program comes *without* any data copyrighted by Bethesda Softworks LLC or ZeniMax Media Inc.

## Running the program

At the moment this program will only work on Linux (and maybe some other unix-like operating systems). However, generated omwaddon files are, to the best of my knowledge, platform independent, so if you're on a Windows machine you could use a Linux VM to generate plugin files and copy them over to your Windows install. Additionally, the source code will need to be edited to customize settings and generate the plugin file correctly. All necessary options can be found in the main.cpp file. It is of vital importance that `MasterDataFilesDir` and `PluginOutputDir` are set to existing directories and that the master files are actually present inside of `MasterDataFilesDir`. The documentation for the other options can be found in settings.hpp.

After executing the program successfully, the newly generated omwaddon file will be located in `PluginOutputDir`. Copy it into one of OpenMW's accepted plugin directories and enable it using openmw-launcher.

Creating a GUI and being able to compile and use the program platform-independently (at least Linux and Windows) is on the roadmap (see below).

## Compiling the program

### Required software

* A C++ compiler supporting C++17 (tested with GCC 9.3)
* CMake (at least version 3.16, lower version not tested, edit CMakeLists.txt to check)
* GNU Make (at least version 4.2.1, lower version not tested)

### Build steps

* `git clone https://github.com/tkoehlmann/MWAddonRandomizer.git`
* `mkdir -p MWAddonRandomizer/build`
* `cd MWAddonRandomizer/build`
* `cmake -DCMAKE_BUILD_TYPE=Release ..`
* `make morrowindaddonrandomizercpp`

### Installing

The program does not need to be installed, it can be run directly from its compiled location.

## Contributing

* Discuss your planned changes (by opening an issue) before implementing them and sending a pull request to make sure that only one person or small team is working on a specific feature.
* All your code must adhere to the license of this project (see below), please state that you accept this in your pull request.
* Use clang-format (or format any changed files using a text editor configured to use the .clang-format file) to format all code as per the coding standards defined therein before committing and sending a pull request.
* If you have many commits in your pull request then [*squash*](http://gitready.com/advanced/2009/02/10/squashing-commits-with-rebase.html) them into one or only a few meaningful ones.
* Don't send pull requests for more than one feature or modification at once.

## Versioning

I use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/tkoehlmann/MWAddonRandomizer/tags).

### Roadmap

* Version 0.1
  * [x] Randomize weapons

* Version 0.2
  * [x] Randomize alchemy effects
  * [x] Randomize artifacts and unique items

* Version 0.3
  * [ ] Change dialogue/journal for changed artifatcs/uniques accordingly

* Version 1.0
  * [ ] Be able to read settings from a file
  * [ ] Make a Qt/wxWidgets UI for easier use and more hints
  * [ ] Make CMake generate a library that can be used by the GUI program
  * [ ] Make the program also work on Windows

* post-1.0
  * Add other randomization options

## License

This project is licensed under the GNU Affero General Public License v3.0 (AGPL 3.0) - see the [LICENSE](https://github.com/tkoehlmann/MWAddonRandomizer/blob/master/LICENSE) file for details.
