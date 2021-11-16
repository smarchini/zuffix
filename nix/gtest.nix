{ stdenv, fetchFromGitHub, cmake }:

stdenv.mkDerivation rec {
  name = "gtest";
  version = "main";

  src = fetchFromGitHub {
      owner = "google";
      repo = "googletest";
      rev = "${version}";
      sha256="R7fnC6XXyTBmIh3dZGR9/fHIBcqns7rQWHJSz9kJx0c=";
  };

  nativeBuildInputs = [ cmake ];

  meta = {
    description = "google test";
  };
}
