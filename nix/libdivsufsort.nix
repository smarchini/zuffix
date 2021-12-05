{ stdenv, fetchFromGitHub, cmake }:

stdenv.mkDerivation rec {
  name = "libdivsufsort";
  version = "master";

  src = fetchFromGitHub {
    owner = "y-256";
    repo = "libdivsufsort";
    rev = "${version}";
    sha256 = "J4l/GSa9fnvK7ytv0erO9C6QsSKjkIAiBeTNLZkJ4zU=";
  };

  cmakeFlags =
    [ "-DBUILD_DIVSUFSORT64=ON" "-DBUILD_EXAMPLES=OFF" "-DUSE_OPENMP=OFF" ];

  nativeBuildInputs = [ cmake ];

  meta = {
    description =
      "Library to construct the suffix array and the BW transformed string";
  };
}
