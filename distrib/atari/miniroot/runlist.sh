#	$NetBSD: runlist.sh,v 1.1.1.1 1996/05/19 19:43:36 leo Exp $

if [ "X$1" = "X-d" ]; then
	SHELLCMD=cat
	shift
else
	SHELLCMD="sh -e"
fi

( while [ "X$1" != "X" ]; do
	cat $1
	shift
done ) | awk -f ${CURDIR}/list2sh.awk | ${SHELLCMD}
