{ pkgs ? import <nixpkgs> { }, stdenv, fetchFromGitHub, cmake }:

stdenv.mkDerivation rec {
  name = "sdsl-lite";
  version = "v2.1.1";

  src = fetchFromGitHub {
    owner = "simongog";
    repo = "sdsl-lite";
    rev = "${version}";
    sha256 = "5xbRxXM9spGqtPYjroj7XnOBCNulIxCZiCJM2AmrRYo=";
  };

  patches = [ ./sdsl-lite.patch ];

  nativeBuildInputs = with pkgs; [ cmake ];

  buildInputs = with pkgs; [ gtest (callPackage ./libdivsufsort.nix { }) ];

  buildPhase = ''
    mkdir -p $out
    patchShebangs ../build/build.sh
    patchShebangs ../build/clean.sh
    patchShebangs ../extras/cheatsheet
    patchShebangs ../extras/cheatsheet/sdsl-cheatsheet.tex
    patchShebangs ../install.sh
    patchShebangs ../test/coverage/clean.sh
    patchShebangs ../test/coverage/run.sh
    patchShebangs ../uninstall.sh
    ../install.sh $out
  '';

  installPhase = ''
    echo Installed
  '';

  meta = { description = "SDSL - Succinct Data Structure Library"; };
}
