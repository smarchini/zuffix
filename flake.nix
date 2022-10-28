{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/master";
  };

  outputs = { self, nixpkgs }:
    let
      pkgsFor = system: import nixpkgs { inherit system; };
      targetSystems = [ "x86_64-linux" ];
    in
    {
      devShells = nixpkgs.lib.genAttrs targetSystems (system:
        let
          pkgs = pkgsFor system;
          mkShellNative = pkgs.mkShell.override { stdenv = pkgs.stdenvAdapters.impureUseNativeOptimizations pkgs.gcc12Stdenv; };
        in
        {
          default = mkShellNative {
            packages = with pkgs; [
              bear
              pkg-config
              gcc_latest
              llvmPackages_latest.clang

              (callPackage ./nix/pizzachili.nix { })
              (callPackage ./nix/r-index.nix { })

              gtest
              gbenchmark
              llvmPackages_13.openmp
              zlib
              (callPackage ./nix/sux.nix { })
              (callPackage ./nix/xxhash.nix { })
              (callPackage ./nix/libsais.nix { })
              (callPackage ./nix/libdivsufsort.nix { })
            ];
          };
        });
    };
}
