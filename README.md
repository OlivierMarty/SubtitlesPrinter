# SubtitlesPrinter

Print subtitles above a X-screen, independently of the video player.

## Requirement

This program uses X11 to show subtitles. If you are using Windows you probably don't use X11, if you are using Linux you probably do.

Currently, this program support only .srt subtitles. Please check also there is no empty line at the beginning of the file, or some unprintable characters before the first "1".
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

It is possible to skip x seconds, or to change the delay before starting. For details see
```bash
./subtitlesPrinter -h
```

## TODO

* Manage some evenements, like pause, shift...
* Support more type of file (with an extern library ?)
* Use a larger, customizable font

Every improvement is welcome !

## LICENSE

This program is under GPLv2 licence, see LICENCE file for more details.
