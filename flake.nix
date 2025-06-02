{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        bin = pkgs.stdenv.mkDerivation {
          name = "lifetime";
          src = ./.;
          nativeBuildInputs = with pkgs; [
            clang
            ncurses.dev
          ];
          buildPhase = ''
            mkdir -p $out/bin
            clang -o $out/bin/lifetime lifetime.c -lm -lncurses
          '';
        };
      in {
        packages.${system}.default = bin;
        apps.default = flake-utils.lib.mkApp {
          drv = bin;
        };
      }
    );
}
