{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/master";
    mini-compile-commands = { url = github:danielbarter/mini_compile_commands; flake = false; };
  };

  outputs = { self, nixpkgs, mini-compile-commands }:
    let
      targetSystems = [ "x86_64-linux" ];
      pkgsFor = system: import nixpkgs { 
        inherit system; 
        overlays = [ (self: super: { 
          libdivsufsort64 = super.libdivsufsort.overrideDerivation (old: { configureFlags = old.configureFlags ++ ["--enable-divsufsort64"]; });
        })];
      };
      nativePkgsFor = system: import nixpkgs {
        inherit system;
        overlays = [ (self: super: { 
          stdenv = super.impureUseNativeOptimizations super.stdenv;
          libdivsufsort64 = super.libdivsufsort.overrideDerivation (old: { configureFlags = old.configureFlags ++ ["--enable-divsufsort64"]; });
        })];
      };
    in
    {
      devShells = nixpkgs.lib.genAttrs targetSystems (system:
        let
          pkgs = pkgsFor system;
          nativePkgs = nativePkgsFor system;
          mkShellNative = with pkgs; mkShell.override {
            stdenv = stdenvAdapters.impureUseNativeOptimizations fastStdenv;
            #stdenv = stdenvAdapters.impureUseNativeOptimizations llvmPackages_latest.stdenv;
          };
          mkShellMcc = with pkgs; mkShell.override {
            # Usage: mini_compile_commands_server.py compile_commands.json
            stdenv = (callPackage mini-compile-commands {}).wrap (
              #stdenvAdapters.impureUseNativeOptimizations fastStdenv
              stdenvAdapters.impureUseNativeOptimizations llvmPackages_latest.stdenv
            );
          };
        in
        {
          default = mkShellNative {
            packages = [
              pkgs.pkg-config
              pkgs.clang-tools

              pkgs.gtest
              pkgs.gbenchmark
              pkgs.llvmPackages_latest.openmp

              nativePkgs.folly
              nativePkgs.zlib
              nativePkgs.xxHash
              nativePkgs.libdivsufsort64
              (nativePkgs.callPackage ./nix/sux.nix { })
              (nativePkgs.callPackage ./nix/libsais.nix { })
              (nativePkgs.callPackage ./nix/wyhash.nix { })

              (pkgs.callPackage ./nix/pizzachili.nix { })
              (pkgs.callPackage ./nix/r-index.nix { })
            ];
          };
        });
    };
}
