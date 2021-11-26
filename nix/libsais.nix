{ stdenv, fetchFromGitHub }:

stdenv.mkDerivation rec {
  name = "libsais";
  version = "2.6.0";

  src = fetchFromGitHub {
    owner = "IlyaGrebnov";
    repo = "libsais";
    rev = "v${version}";
    sha256 = "NHvdLWM4+TYZoy0lpeUSinjDsgKTFleLyo8IKm3fm7k=";
  };

  #cmakeFlags = [ "-DBUILD_DIVSUFSORT64=ON" "-DBUILD_EXAMPLES=OFF" "-DUSE_OPENMP=OFF" ];

  #nativeBuildInputs = [ cmake ];

  patches = [ ./libsais.patch ];

  buildPhase = ''
     make PROJECT=sais all
     make PROJECT=sais64 all
  '';

  installPhase = ''
     make PROJECT=sais PREFIX=$out install
     make PROJECT=sais64 PREFIX=$out install
  '';

  meta = {
    description = "libsais: fast linear time suffix array and Burrows-Wheeler transform construction";
  };
}