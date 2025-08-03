{ lib
, newScope
}:

lib.makeScope newScope (
  self:
  let
    inherit (self) callPackage;
  in
  {
    lvgui = callPackage ./lvgui.nix {};
    lvgui-simulator = self.lvgui.override { withSimulator = true; };
    hello = callPackage ../hello {};
    hello-simulator = self.hello.override { withSimulator = true; };
  }
)
