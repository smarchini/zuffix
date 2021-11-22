{ stdenv, fetchFromGitHub }:

stdenv.mkDerivation rec {
  name = "xxHash";
  version = "dev";

  src = fetchFromGitHub {
      owner = "Cyan4973";
      repo = "xxHash";
      rev = "${version}";
      sha256 = "h/OnVjToQQFS7iEwex4474D4cM+l6lRG8iOvcRoN4/A=";
  };

  meta = {
    description = "Extremely fast hash algorithm";
  };
}
