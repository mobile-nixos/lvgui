{ stdenv
, lib
, lvgui
, nix-gitignore

, withSimulator ? false
}:

let lvgui' = lvgui; in
let
  inherit (lib)
    optional
    optionalString
  ;
  lvgui = lvgui'.override { inherit withSimulator; };
in
stdenv.mkDerivation {
  name = "lvgui${optionalString withSimulator "-simulator"}-hello";

  src = nix-gitignore.gitignoreSource [] ./.;

  nativeBuildInputs = lvgui.nativeBuildInputs;

  buildInputs = lvgui.buildInputs;

  postPatch = ''
    cp -r "${lvgui.src}" lvgui
    chmod -R +w lvgui
  '';

  makeFlags = []
    ++ optional withSimulator "LVGL_ENV_SIMULATOR=1"
    ++ optional (!withSimulator) "LVGL_ENV_SIMULATOR=0"
  ;

  CFLAGS = [
    ''-DHELLO_ASSETS_PATH='"${placeholder "out"}/share/lvgui-hello/"' ''
  ];

  installFlags = [
    "PREFIX=$(out)"
  ];

  postInstall = ''
    mkdir -vp $out/share/lvgui-hello
    cp -v -t $out/share/lvgui-hello hello.svg
  '';
}
