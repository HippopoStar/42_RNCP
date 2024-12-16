
[42 — ft\_ping — about 49 hours](https://cdn.intra.42.fr/pdf/pdf/124498/en.subject.pdf)  

[Internet Engineering Task Force — Request For Comments 792 — Internet Control Message Protocol](https://www.ietf.org/rfc/rfc792.html#page-14)  

https://manpages.debian.org/wheezy/inetutils-ping/ping.8.en.html  

https://directory.fsf.org/wiki/Inetutils  
https://www.gnu.org/software/inetutils/  
https://www.gnu.org/software/inetutils/manual/html_node/ping-invocation.html  
https://git.savannah.gnu.org/gitweb/?p=inetutils.git;a=tree;f=ping;hb=HEAD  

https://directory.fsf.org/wiki/Gnulib  
https://www.gnu.org/software/gnulib/  
https://www.gnu.org/software/gnulib/manual/html_node/program_005finvocation_005fname.html  
https://www.gnu.org/software/gnulib/manual/html_node/progname-and-getprogname.html  
https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/progname.h;hb=HEAD  
https://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob;f=lib/progname.c;hb=HEAD  

https://directory.fsf.org/wiki/Libc  
https://www.gnu.org/software/libc/  
https://sourceware.org/glibc/manual/latest/html_node/Argp.html  
https://sourceware.org/git/?p=glibc.git;a=blob;f=posix/bits/getopt_ext.h;hb=HEAD  
https://sourceware.org/git/?p=glibc.git;a=tree;f=argp;hb=HEAD  
https://sourceware.org/git/?p=glibc.git;a=blob;f=argp/argp-parse.c;hb=HEAD#l860  

https://www.debian.org/distrib/archive  
https://cdimage.debian.org/cdimage/unofficial/non-free/cd-including-firmware/archive/7.0.0-live+nonfree/amd64/iso-hybrid/  
```
curl -C - -L -O https://cdimage.debian.org/cdimage/unofficial/non-free/cd-including-firmware/archive/7.0.0-live+nonfree/amd64/iso-hybrid/debian-live-7.0.0-amd64-standard+nonfree.iso
```

```
# Test cases
./ft_ping -v -? 192.168.1.1
./ft_ping -v -s 0 192.168.1.1
./ft_ping -v -s 65400 192.168.1.1
./ft_ping -v -p '' 192.168.1.1
./ft_ping -v -p '2a2a2a2a2a2a2a2a2a2a2a2a2a2a2a2a' 192.168.1.1
./ft_ping -v -p '4142434445464748494a4b4c4d4e4f50' 192.168.1.1
./ft_ping -v -p '3031323334353637383941424344454621' 192.168.1.1

./ft_ping -v -s 0 -p '2a' 192.168.1.1
```
