{ stdenv }:

stdenv.mkDerivation rec {
  name = "pizzachili";

  src = builtins.fetchTarball {
    url = "http://pizzachili.dcc.uchile.cl/utils/utils.tar.gz";
    sha256 = "12kvhsxhy04bv9kccqcq9gqv34hgfs8hp25bvwr7dkg1bq9zrw3p";
  };

  installPhase = ''
    mkdir -p $out/bin
    cp cut compare alph entrop entrop2 entrop3 gentext genpatterns genintervals $out/bin
  '';

  meta = { description = "Pizza&Chili utilities"; };
}
