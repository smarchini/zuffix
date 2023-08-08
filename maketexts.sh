#!/usr/bin/env bash
SCRIPT=$(realpath "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
DESTPATH="$SCRIPTPATH"/texts

download() {
    mkdir -p "$DESTPATH" && cd "$DESTPATH" || exit
    DOWNLOADED=$(echo "$1" | rev | cut -d '/' -f1 | rev)
    GUNZIPPED=$(echo "$DOWNLOADED" | rev | cut -d '.' -f1 --complement | rev)
    FINALTEXT=$(echo "$GUNZIPPED" | sed 's/\./-/g')".txt"
    wget -P "$DESTPATH" "$1" && gunzip "$DOWNLOADED" && mv "$GUNZIPPED" "$FINALTEXT"
}

# # pizza&chili natural language
download "http://pizzachili.dcc.uchile.cl/texts/nlang/english.gz"
download "http://pizzachili.dcc.uchile.cl/texts/nlang/english.50MB.gz"
download "http://pizzachili.dcc.uchile.cl/texts/nlang/english.100MB.gz"
download "http://pizzachili.dcc.uchile.cl/texts/nlang/english.200MB.gz"
download "http://pizzachili.dcc.uchile.cl/texts/nlang/english.1024MB.gz"

# pizza&chili dna
download "http://pizzachili.dcc.uchile.cl/texts/dna/dna.gz"
download "http://pizzachili.dcc.uchile.cl/texts/dna/dna.50MB.gz"
download "http://pizzachili.dcc.uchile.cl/texts/dna/dna.100MB.gz"
download "http://pizzachili.dcc.uchile.cl/texts/dna/dna.200MB.gz"

# pizza&chili source code
download "http://pizzachili.dcc.uchile.cl/texts/code/sources.gz"
download "http://pizzachili.dcc.uchile.cl/texts/code/sources.50MB.gz"
download "http://pizzachili.dcc.uchile.cl/texts/code/sources.100MB.gz"
download "http://pizzachili.dcc.uchile.cl/texts/code/sources.200MB.gz"

# pizza&chili music
wnload "http://pizzachili.dcc.uchile.cl/texts/music/pitches.gz"
download "http://pizzachili.dcc.uchile.cl/texts/music/pitches.50MB.gz"

# pizza&chili proteins
download "http://pizzachili.dcc.uchile.cl/texts/protein/proteins.gz"
download "http://pizzachili.dcc.uchile.cl/texts/protein/proteins.50MB.gz"
download "http://pizzachili.dcc.uchile.cl/texts/protein/proteins.100MB.gz"
download "http://pizzachili.dcc.uchile.cl/texts/protein/proteins.200MB.gz"

# pizza&chili xml
download "http://pizzachili.dcc.uchile.cl/texts/xml/dblp.xml.gz"
download "http://pizzachili.dcc.uchile.cl/texts/xml/dblp.xml.50MB.gz"
download "http://pizzachili.dcc.uchile.cl/texts/xml/dblp.xml.100MB.gz"
download "http://pizzachili.dcc.uchile.cl/texts/xml/dblp.xml.200MB.gz"

# fibonacci
mkdir -p "$DESTPATH"
"$SCRIPTPATH"/bin/util/generate_fibonacci_string 34 > "$DESTPATH/texts/fibonacci-34.txt"

# random
mkdir -p "$DESTPATH"
"$SCRIPTPATH"/bin/util/generate_random_string  $((200 << 20)) "ACGT" > "$DESTPATH/random-ACGT-200MB.txt"
"$SCRIPTPATH"/bin/util/generate_random_string  $((200 << 20)) "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" > "$DESTPATH/random-azAZ09-200MB.txt"
