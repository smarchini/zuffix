{ stdenv }:

stdenv.mkDerivation rec {
  name = "pizzachili";

  src = builtins.fetchTarball "http://pizzachili.dcc.uchile.cl/utils/utils.tar.gz";

  installPhase = ''
    mkdir -p $out/bin
	  cp cut compare alph entrop entrop2 entrop3 gentext genpatterns genintervals $out/bin
  '';

  meta = {
    description = "Pizza&Chili utilities";
  };
}
