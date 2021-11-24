{ pkgs ? import <nixpkgs> { }, stdenv, fetchFromGitHub, cmake }:

stdenv.mkDerivation rec {
  name = "CSA++";
  version = "master";

  src = fetchFromGitHub {
      owner = "mpetri";
      repo = "benchmark-suffix-array";
      rev = "${version}";
      sha256 = "6YiZdTNkGCicTyLXiNbihlNjQAaxXL2aEVUjNIOWM9k=";
  };

  patches = [ ./csapp.patch ];

  nativeBuildInputs = [ cmake ];

  # buildInputs = with pkgs; [
  #   (callPackage ./sdsl-lite.nix { })
  #   (callPackage ./libdivsufsort.nix { })
  # ];

  meta = {
    description = "CSA++: Fast Pattern Search for Large Alphabets";
  };
}
