with import <nixpkgs> {};
mkShell.override { stdenv = llvmPackages_16.stdenv; } {
    buildInputs = [
        mold
        openssl
    ];
    shellHook = ''
        . .shellhook
    '';
    hardeningDisable = [ "all" ];
}
