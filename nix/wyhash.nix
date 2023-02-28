{ stdenv, fetchFromGitHub, gtest }:

stdenv.mkDerivation rec {
  name = "wyhash";
  version = "wyhash_final4";

  src = fetchFromGitHub {
    owner = "wangyi-fudan";
    repo = "wyhash";
    rev = "${version}";
    sha256 = "/FkVumXtf6fY+pnzyiqQ+JocR4IazZMyv7uLydyBXZ0=";
  };

  buildPhase = ''
    # do nothing
  '';

  installPhase = ''
    mkdir -p $out/include/
    cp ${src}/wyhash.h $out/include/
    cp ${src}/wyhash32.h $out/include/
  '';

  meta = { description = "WyHash"; };
}
