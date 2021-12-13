{ stdenv, fetchFromGitHub }:

stdenv.mkDerivation rec {
  name = "libsais";
  version = "master";

  src = fetchFromGitHub {
    owner = "IlyaGrebnov";
    repo = "libsais";
    rev = "${version}";
    sha256 = "isXZzrvD1jpiH/7KRVhKC6BWWjQ5qZ9En8adPt7fHVA=";
  };

  patches = [ ./libsais.patch ];

  buildPhase = ''
    make CC=g++ PROJECT=sais all
    make CC=g++ PROJECT=sais64 all
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
