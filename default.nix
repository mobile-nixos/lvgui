{ pkgs ? import <nixpkgs> {} }:

rec {
  lvgui = pkgs.callPackage ./support/lvgui.nix {};
  lvgui-simulator = lvgui.override { withSimulator = true; };
}
