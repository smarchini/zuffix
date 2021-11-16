{ pkgs ? import <nixpkgs> { }, }:
rec {
  myProject = pkgs.stdenv.mkDerivation {
    name = "zuffix";
    version = "master";
    buildInputs = with pkgs; [
      gtest
      gbenchmark
      libdivsufsort
      (callPackage ./nix/sux.nix { })
      (callPackage ./nix/sais.nix { })
      (callPackage ./nix/spooky.nix { })
    ];
  };
}
