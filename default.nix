{ pkgs ? import <nixpkgs> { }, }:
pkgs.mkShell {
    packages = with pkgs; [
      bear 
      (callPackage ./nix/pizzachili.nix { })
    ];

    nativeBuildInputs = with pkgs; [
      pkg-config
      clang_13
    ];

    buildInputs = with pkgs; [
      gtest
      gbenchmark
      xxHash
      (callPackage ./nix/sux.nix { })
      (callPackage ./nix/sais.nix { })
      (callPackage ./nix/libdivsufsort.nix { })
      #(callPackage ./nix/spooky.nix { })
      #(callPackage ./nix/smasher.nix { })
    ];
}
