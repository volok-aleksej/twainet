call npm install https://github.com/dcodeIO/ProtoBuf.js/tarball/master
node install.js
rmdir /s /q node_modules\udt
"%PROGRAMFILES%\7-zip\7z.exe" x node_modules\udt.zip -onode_modules
cd node_modules
rename udt-master udt
cd udt
call npm install https://github.com/bigeasy/packet/tarball/master