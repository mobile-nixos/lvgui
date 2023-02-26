{ stdenv
, pkgs
, lib
, pkg-config
, freetype
, libdrm
, libinput
, libevdev
, libxkbcommon
, nix-gitignore
, SDL2
, withSimulator ? false
}:

let stdenv = pkgs.stdenvAdapters.keepDebugInfo pkgs.stdenv; in

let
  inherit (lib) optional optionals optionalString;
  simulatorDeps = [
    SDL2
  ];
in
  stdenv.mkDerivation {
    pname = "lvgui${optionalString withSimulator "-simulator"}";
    version = "2023-02-25";

    src = nix-gitignore.gitignoreSource [] ../.;

    # Document `LVGL_ENV_SIMULATOR` in the built headers.
    # This allows the mrbgem to know about it.
    # (In reality this should be part of a ./configure step or something similar.)
    postPatch = ''
      sed -i"" '/^#define LV_CONF_H/a #define LVGL_ENV_SIMULATOR ${if withSimulator then "1" else "0"}' lv_conf.h
    '';

    nativeBuildInputs = [
      pkg-config
    ];

    buildInputs = [
      freetype
      libevdev
      libdrm
      libinput
      libxkbcommon
    ]
    ++ optionals withSimulator simulatorDeps
    ;

    NIX_CFLAGS_COMPILE = [
      "-DX_DISPLAY_MISSING"
    ];

    makeFlags = [
      "PREFIX=${placeholder "out"}"
    ]
    ++ optional withSimulator "LVGL_ENV_SIMULATOR=1"
    ++ optional (!withSimulator) "LVGL_ENV_SIMULATOR=0"
    ;

    enableParallelBuilding = true;
  }
