#!/bin/bash

set -ex

if [ "$1" == "addons" ]; then
  # Note: this section does not need to be called by travis, use the addons
  # section instead
  sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
  sudo add-apt-repository ppa:ubuntu-sdk-team/ppa -y
  sudo add-apt-repository ppa:mc3man/trusty-media -y
  sudo add-apt-repository ppa:beineri/opt-qt591-trusty -y
  sudo apt-get update -q
  sudo apt-get install build-essential qt59tools qt59base qt59multimedia \
    qt59svg qt59xmlpatterns bsdtar ffmpeg doxygen doxygen-doc doxygen-latex \
    doxygen-gui graphviz python3 python3-pip
elif [ "$1" == "before_install" ]; then
  # Nothing to do here at the moment
  :
elif [ "$1" == "install" ]; then
  pip3 freeze > requirements.txt
  pip3 install -r requirements.txt

  # Install Google API library for Python3
  sudo pip3 install google-api-python-client
  python3 -V
  pip3 -V
elif [ "$1" == "before_script" ]; then
  if [ "$TRAVIS_BRANCH" = "release" ]; then
    # If we are building a release, then always assume there are code changes
    CODE_CHANGES=0
  else
    # If not release and there are changes, then set CODE_CHANGES to true
    $TRAVIS_BUILD_DIR/util/checkchanges.sh && CODE_CHANGES=0
  fi
  if [ $CODE_CHANGES ]; then
    echo "Some code appears to have changed"
    . /opt/qt59/bin/qt59-env.sh
    export DISPLAY=:99.0
    sh -e /etc/init.d/xvfb start
    echo "Waiting for xvfbto start"
    sleep 3
    qmake --version
  else
    echo "None of the code appears to have changed, skipping rebuild"
  fi
elif [ "$1" == "script" ]; then
  if [ $CODE_CHANGES ]; then
    mkdir "build" && cd "build"
    if [ "$TRAVIS_BRANCH" == "release" ]; then
      qmake ../ PREFIX=/usr CONFIG+=release DEFINES+=QT_NO_DEBUG_OUTPUT DEFINES+=PENCIL2D_RELEASE
    else
      qmake ../ PREFIX=/usr CONFIG+=GIT CONFIG+=NIGHTLY
    fi
    make
    "$TRAVIS_BUILD_DIR/build/tests/tests"
  fi
elif [ "$1" == "after_success" ]; then
  if [ $CODE_CHANGES ]; then
    echo "look for build folder and zip file"
    ls
    make INSTALL_ROOT="${PWD}/Pencil2D" install;
    rm -rf Pencil2D/usr/lib;
    echo "Creating AppImage...";
    sed -i "/^Keywords\(\[[a-zA-Z_.@]\+\]\)\?=/d;/^Version=/cVersion=1.0" Pencil2D/usr/share/applications/pencil2d.desktop;
    install -Dm755 /usr/bin/ffmpeg Pencil2D/usr/plugins/ffmpeg;
    curl -fsSLO https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage;
    chmod 755 linuxdeployqt-continuous-x86_64.AppImage;
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/lib/x86_64-linux-gnu/pulseaudio" ./linuxdeployqt-continuous-x86_64.AppImage Pencil2D/usr/share/applications/pencil2d.desktop -executable=Pencil2D/usr/plugins/ffmpeg -extra-plugins=iconengines/libqsvgicon.so -appimage;
    mv "Pencil2D-x86_64.AppImage" "pencil2d-linux-$(date +"%Y-%m-%d").AppImage";
    echo "zipping done"
    ls

    # Upload to google drive
    if [ \( "$TRAVIS_BRANCH" == "master" -o "$TRAVIS_BRANCH" == "release" \) -o "$FORCE_NIGHTLY_UPLOAD" == "yes" ]; then
      echo "Initiate deployment on Google Drive"
      cd "$TRAVIS_BUILD_DIR/util"
      python3 nightly-build-upload.py "$LINUX_NIGHTLY_PARENT" "$TRAVIS_BUILD_DIR/build/pencil2d-linux-$(date +"%Y-%m-%d").AppImage"
      echo "Operation done"
    fi
  fi

  # Deploy doxygen documentation to github pages
  if [ "$TRAVIS_BRANCH" == "master" ]; then
    cd "$TRAVIS_BUILD_DIR/util/docs"
    for i in core svg xmlpatterns; do
      curl -fsSLO "https://doc.qt.io/qt-5/qt$i.tags"
    done
    ./documentation-deploy.sh
  fi
else
  echo "Unknown operating mode exiting" >&2
  exit 1
fi
