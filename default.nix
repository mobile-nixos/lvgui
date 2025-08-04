{ pkgs ? null
, nixpkgs_path ? null
}@args:

let
  pkgs =
  if args ? pkgs && args ? nixpkgs_path then
    builtins.abort "Either pkgs or nixpkgs_path can be given, not both at once."
  else if args ? pkgs then
    args.pkgs
  else if args ? nixpkgs_path then
    import nixpkgs_path {}
  else
    import (import ./npins).nixpkgs {}
  ;
in

(pkgs.callPackage ./support/nix {}) // {
  inherit pkgs;
}
