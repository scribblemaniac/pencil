#!/usr/bin/env bash

error() {
  if [ "${GITHUB_ACTIONS}" = "true" ]; then
    echo "::error::$1"
  else
    echo -e "\033[1m\033[31m==> ERROR:\033[0m \033[1m$1\033[0m" >&2
  fi
}

msg() {
  if [ "${GITHUB_ACTIONS}" = "true" ]; then
    echo "$1"
  else
    echo -e "\033[1m\033[32m==>\033[0m \033[1m$1\033[0m"
  fi
}

group() {
  if [ "${GITHUB_ACTIONS}" = "true" ]; then
    echo "::group::$1"
  else
    echo -e "\033[1m\033[32m==>\033[0m \033[1m$1\033[0m"
  fi
}

endgroup() {
  if [ "${GITHUB_ACTIONS}" = "true" ]; then
    echo "::endgroup::"
  fi
}

set_output() {
  if [ "${GITHUB_ACTIONS}" = "true" ]; then
    echo "::set-output name=$1::$2"
  fi
}

create_package_linux() {
  group "Set up AppImage contents"
  make install INSTALL_ROOT="${PWD}/Pencil2D"
  endgroup

  group "Create AppImage"
  # "Downgrade" the desktop entry to version 1.0
  sed -i "/^Keywords\(\[[a-zA-Z_.@]\+\]\)\?=/d;/^Version=/cVersion=1.0" \
    Pencil2D/usr/share/applications/org.pencil2d.Pencil2D.desktop
  # Copy FFmpeg executable
  install -Dm755 /usr/bin/ffmpeg Pencil2D/usr/plugins/ffmpeg
  # Copy GStreamer binaries
  install -Dm755 "/usr/lib/x86_64-linux-gnu/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner" \
    "Pencil2D/usr/lib/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner"
  local gst_executables="-executable=Pencil2D/usr/lib/gstreamer1.0/gstreamer-1.0/gst-plugin-scanner"
  for plugin in adpcmdec alsa app audioconvert audioparsers audioresample \
      autodetect coreelements gsm id3demux jack mpg123 mulaw playback \
      pulse typefindfunctions wavparse apetag; do
    install -Dm755 "/usr/lib/x86_64-linux-gnu/gstreamer-1.0/libgst${plugin}.so" \
      "Pencil2D/usr/lib/gstreamer-1.0/libgst${plugin}.so"
    gst_executables="${gst_executables} -executable=Pencil2D/usr/lib/gstreamer-1.0/libgst${plugin}.so"
  done

  curl -fsSLO https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
  chmod 755 linuxdeployqt-continuous-x86_64.AppImage
  LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/lib/x86_64-linux-gnu/pulseaudio" \
    ./linuxdeployqt-continuous-x86_64.AppImage \
    Pencil2D/usr/share/applications/org.pencil2d.Pencil2D.desktop \
    -executable=Pencil2D/usr/plugins/ffmpeg \
    ${gst_executables} \
    -appimage
  mv Pencil2D*.AppImage* "pencil2d-linux-$1.AppImage"
  set_output "package-name" "pencil2d-linux-$1.AppImage"
  endgroup
}

create_package_macos() {
  group "Clean"
  make clean
  mv bin Pencil2D
  endgroup

  pushd Pencil2D >/dev/null

  group "Copy FFmpeg plugin"
  mkdir Pencil2D.app/Contents/MacOS/plugins
  curl -fsSLo ffmpeg.7z https://evermeet.cx/ffmpeg/getrelease/7z
  curl -fsSLo ffmpeg.7z.sig https://evermeet.cx/ffmpeg/getrelease/7z/sig
  mkdir -m700 ~/.gnupg
  echo "trusted-key 0x476C4B611A660874" > ~/.gnupg/gpg.conf
  curl -fsSL https://evermeet.cx/ffmpeg/0x1A660874.asc | gpg --import
  gpg --verify ffmpeg.7z.sig ffmpeg.7z
  bsdtar xfC ffmpeg.7z Pencil2D.app/Contents/MacOS/plugins
  rm ffmpeg.7z ffmpeg.7z.sig
  endgroup

  msg "Deploy Qt libraries"
  macdeployqt Pencil2D.app

  group "Apply macdeployqt fix"
  curl -fsSLO https://github.com/aurelien-rainone/macdeployqtfix/archive/master.zip
  bsdtar xf master.zip
  python macdeployqtfix-master/macdeployqtfix.py \
    Pencil2D.app/Contents/MacOS/Pencil2D \
    /usr/local/Cellar/qt/5.9.1/
  endgroup

  msg "Remove files"
  rm -rf macdeployqtfix-master master.zip

  popd >/dev/null

  msg "Create ZIP"
  bsdtar caf "pencil2d-mac-$1.zip" Pencil2D
  set_output "package-name" "pencil2d-mac-$1.zip"
}

create_package_windows() {
  msg "Copy FFmpeg plugin"
  local platform="${2%%_*}"
  local ffmpeg="ffmpeg-${platform}.zip"
  curl -fsSLO "https://github.com/pencil2d/pencil2d-ffmpeg/releases/download/v4.1.1/$ffmpeg"
  "${WINDIR}\\System32\\tar" xf "${ffmpeg}"
  mkdir bin/plugins
  mv "ffmpeg.exe" bin/plugins/
  rm -rf "${ffmpeg}"

  mv bin Pencil2D
  msg "Remove files"
  find \( -name '*.pdb' -o -name '*.ilk' \) -delete

  group "Deploy Qt libraries"
  windeployqt Pencil2D/pencil2d.exe
  endgroup

  msg "Copy OpenSSL DLLs"
  local xbits="-x${platform#win}"
  cp "C:\\Program Files\\OpenSSL\\lib"{ssl,crypto}"-1_1${xbits/-x32/}.dll" Pencil2D/

  msg "Create ZIP"
  "${WINDIR}\\System32\\tar" caf "pencil2d-${platform}-$1.zip" Pencil2D
  set_output "package-name" "pencil2d-${platform}-$1.zip"
}

trap 'error "Command failed"' ERR
set -eE

if [ $# -lt 2 -o \( "$(echo $2 | tr '[A-Z]' '[a-z]')" = "windows" -a $# -lt 3 \) ]; then
  echo "Usage: $0 [version] (Linux|macOS|Windows [architecture])" >&2
  exit 1
fi

"create_package_$(echo $2 | tr '[A-Z]' '[a-z]')" "$1" "$3"
