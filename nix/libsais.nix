{ stdenv, fetchFromGitHub }:

stdenv.mkDerivation rec {
  name = "libsais";
  version = "master";

  src = fetchFromGitHub {
    owner = "IlyaGrebnov";
    repo = "libsais";
    rev = "${version}";
    sha256 = "Tu3uBvzOS29Nb4o9uV12dGvKq2eowYTEh31VEx8PYno=";
  };

  # patches = [ ./libsais.patch ];

  buildPhase = ''
    make CC=g++ CFLAGS+=-fopenmp PROJECT=sais all
    make CC=g++ CFLAGS+=-fopenmp PROJECT=sais64 all
  '';

  installPhase = ''
    make PROJECT=sais PREFIX=${placeholder "out"} install
    make PROJECT=sais64 PREFIX=${placeholder "out"} install
  '';

  meta = {
    description =
      "libsais: fast linear time suffix array and Burrows-Wheeler transform construction";
  };
}
