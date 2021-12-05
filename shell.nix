{ pkgs ? import <nixpkgs> { }, }:
pkgs.mkShell {
  packages = with pkgs; [
    glibcLocales
    bear
    (callPackage ./nix/pizzachili.nix { })
    (callPackage ./nix/r-index.nix { })
  ];

  nativeBuildInputs = with pkgs; [
    pkg-config
    llvmPackages_latest.clang
    gcc_latest
  ];

  buildInputs = with pkgs; [
    gtest
    gbenchmark
    llvmPackages_13.openmp
    (callPackage ./nix/sux.nix { })
    (callPackage ./nix/xxhash.nix { })
    (callPackage ./nix/libsais.nix { })
    (callPackage ./nix/libdivsufsort.nix { })
  ];
}
