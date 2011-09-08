#!/usr/bin/perl -w
## 
# From:
# http://stackoverflow.com/questions/137038/how-do-you-get-assembler-output-from-c-c-source-in-gcc
# Original by Loren Merritt.
##
$ptr='(BYTE|WORD|DWORD|QWORD|XMMWORD) PTR ';
$reg='(?:[er]?(?:[abcd]x|[sd]i|[sb]p)|[abcd][hl]|r1?[0-589][dwb]?|mm[0-7]|xmm1?[0-9])';
open FH, '-|', '/usr/bin/objdump', '-w', '-M', 'intel', @ARGV or die;
$prev = "";
while(<FH>){
    if(/$ptr/o) {
        s/$ptr(\[[^\[\]]+\],$reg)/$2/o or
        s/($reg,)$ptr(\[[^\[\]]+\])/$1$3/o or
        s/$ptr/lc $1/oe;
    }
    if($prev =~ /\t(repz )?ret / and
       $_ =~ /\tnop |\txchg *ax,ax$/) {
       # drop this line
    } else {
       print $prev;
       $prev = $_;
    }
}
print $prev;
close FH;
