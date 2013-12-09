#!/bin/bash

### FSTs ###


# FST T
cat > T.txt << FST
0 1 a x 0.5
0 1 b y 1.5
1 2 c z 2.5
2 3.5
FST

cat > T.isyms << SYM
a 1
b 2
c 3
SYM

cat > T.osyms << SYM
x 1
y 2
z 3
SYM

# FST V
cat > V.txt << FST
0 1 a x 0.5
0 1 b y 1.5
1 2 c z 2.5
1 -0.5
2 3.5
FST

cp T.isyms V.isyms
cp T.osyms V.osyms

# FST S
cat > S.txt << FST
0 1 a x 1.5
0 1 b y 0.5
1 2 c z 0.5
2 3.5
FST

cat > S.isyms << SYM
a 1
b 2
c 3
SYM

cat > S.osyms << SYM
x 1
y 2
z 3
SYM


### script ###
fsts="T S V"

for name in $fsts; do
    fstcompile --arc_type=log -isymbols=${name}.isyms -osymbols=${name}.osyms \
        ${name}.txt ${name}.fst
    fstdraw --portrait=true -isymbols=${name}.isyms -osymbols=${name}.osyms \
        ${name}.fst | dot -Tsvg  > ${name}.svg
done

exit 0
