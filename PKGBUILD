# Maintainer: Simon Berger <simberger@gmail.com>
pkgname=visual_papara-git
pkgver=20120529
pkgrel=1
pkgdesc="GUI frontend for PaPaRa 2.0 sequence alignment program."
arch=('i686' 'x86_64')
url="http://exelixis-lab.org"
license=('GPL')
groups=()
depends=(qt boost)
makedepends=(git cmake)
provides=(visual_papara)
conflicts=(visual_papara)
replaces=()
backup=()
options=()
install=
source=()
noextract=()
md5sums=() #generate with 'makepkg -g'

_gitroot="git://github.com/sim82/contraption.git"
_gitname=contraption.git

build() {
  cd "$srcdir"
  msg "Connecting to GIT server...."

  if [[ -d "$_gitname" ]]; then
    cd "$_gitname" && git pull origin
    msg "The local files are updated."
  else
    git clone "$_gitroot" "$_gitname"
    cd $_gitname
    sh ro_submodules.sh

    git submodule init

    mkdir build
  fi

  git submodule update

  msg "GIT checkout done or server timeout"
  msg "Starting build..."


  cd build

  cmake -DCMAKE_BUILD_TYPE=Release ..
  
  make

}

package() {
  cd "$srcdir/$_gitname/build"

  install -D -m644 ../LICENSE "${pkgdir}/usr/share/licenses/${pkgname}/LICENSE"
  install -D -m644 ../ivy_mike/LICENSE "${pkgdir}/usr/share/licenses/${pkgname}/LICENSE.ivy_mike"
  install -D -m755 test $pkgdir/usr/bin/VisualPaPaRa
}

