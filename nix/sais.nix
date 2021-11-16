{ stdenv, fetchzip, cmake }:

stdenv.mkDerivation rec {
  name = "sais";
  version = "2.4.1";

  src = fetchzip {
      url = "https://sites.google.com/site/yuta256/sais-2.4.1.zip";
      sha256 = "HffrHZSihHsKrd5NpjP+7PEODo66neMi7JXrsu6P7KQ=";
  };

  patches = [ ./sais.patch ];

  nativeBuildInputs = [ cmake ];

  meta = {
    description = "SA-IS algorithm";
  };
}
