{ stdenv, fetchFromGitHub, gtest }:

stdenv.mkDerivation rec {
  name = "sux";
  version = "1.0.3";

  src = fetchFromGitHub {
      owner = "vigna";
      repo = "sux";
      rev = "${version}";
      sha256 = "NVb6uhFOngI1qbaHJvXXM7zRmgHKrm3D8EpqTQ5mVDA=";
  };

  nativeBuildInputs = [ gtest ];

  installPhase = ''
     mkdir -p $out/include/
     cp -r ${src}/sux $out/include/
  '';

  meta = {
    description = "Sux";
  };
}
