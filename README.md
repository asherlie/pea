# PEA

PEA is used to securely create and sign petitions

## host
a host daemon (source file: _host/pea.c_) is spawned
#### usage:
```sh
./pea_h [socket file] [petition output file]
```
socket file and petition output file must both be complete filepaths
both files should be in directories accessible by any users who might want or need to sign a petition

## client
pea is used to operate on petitions
petitions can be signed, created, removed (with the proper credentials), or written to the petition output file designated by `pea_h`

#### usage:
```sh
./pea socket_file list                 # list available petitions
./pea socket_file sign list_item       # sign petition
./pea socket_file create petition_name # create new petition
./pea socket_file remove list_item     # remove petition
./pea socket_file unsign list_item     # remove petition
```

## notes
* PEA will only compile on linux machines. PEA is able to record signatures securely by avoiding relying on signers to report their own credentials. this is done by having the kernel report credentials through [AF_UNIX](https://linux.die.net/man/7/unix) sockets using [SO_PEERCRED](https://linux.die.net/man/7/socket)
* when a petition is removed, petition indices are shifted
* PEA sends petition indices using 16 bits of precision. for this reason, PEA only supports indices up to 2^16 or 65,535

## TODO
* ~~add README~~
* ~~add unsign functionality~~
* rename list to printfile, add list command that prints petitions locally
* use less precision for pet_option in packed integer to make more room for petition indices
* fix undefined behavior in remove_p and remove_sig
