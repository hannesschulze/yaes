{
  description = "NES Emulator";

  inputs = {
    nixpkgs = {
      url = "github:nixos/nixpkgs/nixpkgs-unstable";
    };

    flake-utils = {
      url = "github:numtide/flake-utils";
    };
  };

  outputs = { self, nixpkgs, flake-utils, ... }@inputs:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        shell = pkgs.mkShell {
          packages = with pkgs; [ cmake sdl3 gcc binutils gdb gnumake valgrind clang-tools ];
		  hardeningDisable = [ "fortify" ];
        };
      in
      {
        devShells = {
          default = shell;
        };
      });
}

