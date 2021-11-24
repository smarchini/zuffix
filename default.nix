{ pkgs ? import <nixpkgs> { }, }:
pkgs.mkShell {
    packages = with pkgs; [
      bear 
      (callPackage ./nix/pizzachili.nix { })
    ];

    nativeBuildInputs = with pkgs; [
      pkg-config
      llvmPackages_latest.clang
      gcc_latest
      #(callPackage ./nix/r-index.nix { })
      #(callPackage ./nix/csapp.nix { })
    ];

    buildInputs = with pkgs; [
      gtest
      gbenchmark
      xxHash
      llvmPackages_13.openmp
      (callPackage ./nix/sux.nix { })
      (callPackage ./nix/libsais.nix { })
      (callPackage ./nix/libdivsufsort.nix { })
      #(callPackage ./nix/spooky.nix { })
      #(callPackage ./nix/smasher.nix { })
    ];
}
