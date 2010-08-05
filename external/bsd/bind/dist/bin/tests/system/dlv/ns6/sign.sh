#!/bin/sh
#
# Copyright (C) 2010  Internet Systems Consortium, Inc. ("ISC")
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
# OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

# Id: sign.sh,v 1.2.2.2 2010/05/27 23:49:55 tbox Exp

SYSTEMTESTTOP=../..
. $SYSTEMTESTTOP/conf.sh

RANDFILE=../random.data

zone=grand.child1.utld.
infile=child.db.in
zonefile=grand.child1.utld.db
outfile=grand.child1.signed

keyname1=`$KEYGEN -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`
keyname2=`$KEYGEN -f KSK -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`

cat $infile $keyname1.key $keyname2.key >$zonefile

$SIGNER -g -r $RANDFILE -o $zone -f $outfile $zonefile > /dev/null 2> signer.err || cat signer.err
echo "I: signed $zone"


zone=grand.child3.utld.
infile=child.db.in
zonefile=grand.child3.utld.db
outfile=grand.child3.signed
dlvzone=dlv.utld.

keyname1=`$KEYGEN -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`
keyname2=`$KEYGEN -f KSK -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`

cat $infile $keyname1.key $keyname2.key >$zonefile

$SIGNER -g -r $RANDFILE -o $zone -f $outfile $zonefile > /dev/null 2> signer.err || cat signer.err
echo "I: signed $zone"


zone=grand.child4.utld.
infile=child.db.in
zonefile=grand.child4.utld.db
outfile=grand.child4.signed
dlvzone=dlv.utld.

keyname1=`$KEYGEN -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`
keyname2=`$KEYGEN -f KSK -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`

cat $infile $keyname1.key $keyname2.key >$zonefile

$SIGNER -g -r $RANDFILE -o $zone -f $outfile $zonefile > /dev/null 2> signer.err || cat signer.err
echo "I: signed $zone"


zone=grand.child5.utld.
infile=child.db.in
zonefile=grand.child5.utld.db
outfile=grand.child5.signed
dlvzone=dlv.utld.

keyname1=`$KEYGEN -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`
keyname2=`$KEYGEN -f KSK -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`

cat $infile $keyname1.key $keyname2.key >$zonefile

$SIGNER -g -r $RANDFILE -o $zone -f $outfile $zonefile > /dev/null 2> signer.err || cat signer.err
echo "I: signed $zone"


zone=grand.child7.utld.
infile=child.db.in
zonefile=grand.child7.utld.db
outfile=grand.child7.signed
dlvzone=dlv.utld.

keyname1=`$KEYGEN -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`
keyname2=`$KEYGEN -f KSK -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`

cat $infile $keyname1.key $keyname2.key >$zonefile

$SIGNER -g -r $RANDFILE -o $zone -f $outfile $zonefile > /dev/null 2> signer.err || cat signer.err
echo "I: signed $zone"


zone=grand.child8.utld.
infile=child.db.in
zonefile=grand.child8.utld.db
outfile=grand.child8.signed
dlvzone=dlv.utld.

keyname1=`$KEYGEN -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`
keyname2=`$KEYGEN -f KSK -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`

cat $infile $keyname1.key $keyname2.key >$zonefile

$SIGNER -g -r $RANDFILE -o $zone -f $outfile $zonefile > /dev/null 2> signer.err || cat signer.err
echo "I: signed $zone"


zone=grand.child9.utld.
infile=child.db.in
zonefile=grand.child9.utld.db
outfile=grand.child9.signed
dlvzone=dlv.utld.

keyname1=`$KEYGEN -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`
keyname2=`$KEYGEN -f KSK -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`

cat $infile $keyname1.key $keyname2.key >$zonefile

$SIGNER -g -r $RANDFILE -o $zone -f $outfile $zonefile > /dev/null 2> signer.err || cat signer.err
echo "I: signed $zone"

zone=grand.child10.utld.
infile=child.db.in
zonefile=grand.child10.utld.db
outfile=grand.child10.signed
dlvzone=dlv.utld.

keyname1=`$KEYGEN -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`
keyname2=`$KEYGEN -f KSK -r $RANDFILE -a DSA -b 768 -n zone $zone 2> /dev/null`

cat $infile $keyname1.key $keyname2.key >$zonefile

$SIGNER -g -r $RANDFILE -o $zone -f $outfile $zonefile > /dev/null 2> signer.err || cat signer.err
echo "I: signed $zone"
