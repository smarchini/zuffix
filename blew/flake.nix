{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/master";
  };

  outputs = { self, nixpkgs }:
    let
      targetSystems = [ "x86_64-linux" ];
      pkgsFor = system: import nixpkgs { 
        inherit system; 
        overlays = [(self: super: {
          python = super.python312;
        })];
      };
    in {
      devShells = nixpkgs.lib.genAttrs targetSystems (system:
        let
          pkgs = pkgsFor system;
          pythonEnv = pkgs.python3.withPackages(ps: with ps; [
            matplotlib
            networkx
            pandas
            scipy
            numpy
          ]);
        in {
          default = pkgs.mkShell {
            packages = [ pythonEnv ];
          };
        });
    };
}
