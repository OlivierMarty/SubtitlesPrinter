# SubtitlesPrinter

Print subtitles above a X-screen, independently of the video player.

## Requirement

This program uses X11 to show subtitles. If you are using Windows you probably don't use X11, if you are using Linux you probably do.

Currently, this program support only .srt subtitles. Please check also there is no empty line at the beginning of the file, or some unprintable characters before the first "1".
You can see the specs of this format here : http://matroska.org/technical/specs/subtitles/srt.html

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

To quit, press CTRL+C in the terminal.
To pause and resume, press space anywhere (this does not work with all windows).

### Optional arguments

It is possible to skip x seconds, or to change the delay before starting. For other parameters and details see
```bash
subtitlesPrinter -h
```

## TODO

* Manage other evenements, like shift...
* Support more type of file (with an extern library ?)

Every improvement is welcome !

## LICENSE

This program is under GPLv2 licence, see LICENCE file for more details.
