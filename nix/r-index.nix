{ pkgs ? import <nixpkgs> { }, stdenv, fetchFromGitHub, cmake }:

stdenv.mkDerivation rec {
  name = "r-index";
  version = "master";

  src = fetchFromGitHub {
      owner = "nicolaprezza";
      repo = "r-index";
      rev = "${version}";
      sha256 = "U5nEaxCXAqxYyiujRpLSsVUjIKUpxqY4agYLkgFcxKk=";
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

  meta = {
    description = "r-index: the run-length BWT index";
  };
}
