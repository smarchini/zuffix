{ stdenv, fetchurl }:

stdenv.mkDerivation rec {
  name = "spooky";
  version = "2";

  src = fetchurl {
    url = "https://burtleburtle.net/bob/c/SpookyV2.h";
    sha256 = "2504yaf+rJflinqjoIEl0tdJxllrrIKoZ0QLcy23tg4=";
  };

  srcpp = fetchurl {
    url = "https://burtleburtle.net/bob/c/SpookyV2.cpp";
    sha256= "+scf1H04GuZ94Ongvr7ryGcSycrjE4I2YYQrPXJlv5I=";
  };

  # It is just the file. No unpacking needed. Seems like we need to create
  # _some_ folder, otherwise we get errors.
  unpackCmd = "mkdir dummy_dir";

  installPhase = ''
    mkdir -p $out/include
    mkdir -p $out/lib
    echo "#pragma once" >> $out/include/SpookyV2.h
    cat ${src} >> $out/include/SpookyV2.h
    g++ -fpic -shared ${srcpp} -I $out/include -o $out/lib/libspooky.so
  '';

  meta = {
    description = "Spooky hash";
    homepage = https://burtleburtle.net/bob/hash/spooky.html;
  };

}
