# SubtitlesPrinter


Print subtitles above a X-screen

## Requirement

This program uses X11 to print subtitles. If you are using Windows you probably don't use X11, if you are using Linux you probably do.

Currently, this program support only .srt subtitles. Please check also there is no empty line at the beginning of the file, or some weird characters before the first "1".
You can see the specs of this format here : http://matroska.org/technical/specs/subtitles/srt.html

## Installation

Run in a terminal
```bash
    git clone https://github.com/OlivierMarty/SubtitlesPrinter.git
    make
```

## Usage

To show file.srt, run in the installation directory :
```bash
    ./subtitlesPrinter file.srt
```

To quit, press CTRL+C in the terminal.

### Optional arguments

If you have already seen X seconds of your video, you can use the following to skip the beginning :
```bash
    ./subtitlesPrinter file.srt X
```

By default, the program leave you 5 seconds to start your video, you can change that with the last optional argument, for Y seconds :
```bash
    ./subtitlesPrinter file.srt X Y
```

## TODO

* Manage some evenement, like pauses.
* Support more type of file (with an extern library ?)

Every improvement is welcome !

## LICENSE

This program is under GPLv2 licence, see LICENCE file for more details.
