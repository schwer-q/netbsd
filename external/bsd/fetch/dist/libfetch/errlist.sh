#!/bin/sh
# $NetBSD: errlist.sh,v 1.1.1.1 2008/09/30 18:46:07 joerg Exp $

printf "static struct fetcherr $1[] = {\n"
while read code type msg; do
	[ "${code}" = "#" ] && continue
	printf "\t{ ${code}, FETCH_${type}, \"${msg}\" },\n"
done < $3

printf "\t{ -1, FETCH_UNKNOWN, \"Unknown $2 error\" }\n"
printf "};\n"
