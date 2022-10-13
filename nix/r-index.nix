{ pkgs ? import <nixpkgs> { }, stdenv, fetchFromGitHub, cmake }:

stdenv.mkDerivation rec {
  name = "r-index";
  version = "master";

  src = fetchFromGitHub {
    owner = "nicolaprezza";
    repo = "r-index";
    rev = "${version}";
    sha256 = "SF2kRiLg5QfPeOvaG37U/OF9oZ5yq/8QolshHggX1hQ=";
  };

  nativeBuildInputs = [ cmake ];

  buildInputs = with pkgs; [
    (callPackage ./sdsl-lite.nix { })
    (callPackage ./libdivsufsort.nix { })
  ];

  buildPhase = ''
    cmake ..
    make
  '';

  installPhase = ''
    mkdir -p $out/bin/
    cp ri-build ri-locate ri-count $out/bin/
  '';

  meta = { description = "r-index: the run-length BWT index"; };
}
