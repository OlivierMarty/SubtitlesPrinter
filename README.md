# SubtitlesPrinter

Print subtitles above a X-screen, independently of the video player.

## Requirement

This program uses X11 to show subtitles.

This program support .srt, .vtt, and two .sub subtitles types. Please check also there is no empty line at the beginning of the file, or some unprintable characters before the first character.
You can see the specs of format .srt here : http://matroska.org/technical/specs/subtitles/srt.html

To have transparency between subtitles, try to launch xcompmgr before subtitlesPrinter.

## Installation

Run in a terminal
```bash
git clone https://github.com/OlivierMarty/SubtitlesPrinter.git
make
sudo make install
```

## Usage

To show file.srt, run in a terminal :
```bash
subtitlesPrinter file.srt
```

### Controls

To quit, press CTRL+C in the terminal.
To pause and resume, press space anywhere (badly this does not work with all windows).
To shift the clock, press left and right arrows.

### Optional arguments

It is possible to skip x seconds with option -s, and to scale time with option -t. For other parameters and details see
```bash
subtitlesPrinter -h
```

## TODO

* Support more type of file (with an extern library ?)

Every improvement is welcome !

## LICENSE

This program is under GPLv2 licence, see LICENCE file for more details.
