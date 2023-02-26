{ pkgs ? import <nixpkgs> {} }:

rec {
  lvgui = pkgs.callPackage ./support/lvgui.nix {};
  lvgui-simulator = lvgui.override { withSimulator = true; };
  hello = pkgs.callPackage ./support/hello {
    inherit lvgui;
  };
  hello-simulator = pkgs.callPackage ./support/hello {
    inherit lvgui;
    withSimulator = true;
  };
}
