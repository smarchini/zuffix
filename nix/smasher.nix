{ stdenv, fetchFromGitHub, cmake }:

stdenv.mkDerivation rec {
  name = "smasher";
  version = "master";

  src = fetchFromGitHub {
      owner = "aappleby";
      repo = "smhasher";
      rev = "${version}";
      sha256 = "UaMGyRfq2ZSgp1wMrO0mV8ALoeI/q+B+Q9238v+LWsE=";
  } + "/src";

  nativeBuildInputs = [ cmake ];

  installPhase = ''
    mkdir -p $out/include
    mkdir -p $out/lib
    cp
  '';

  meta = {
    description = "SMHasher: a test suite of non-cryptographic hash functions.";
  };
}
