set term postscript enhanced color eps
set output "bands.eps"
set ylabel "Energy (eV)"

Elo = -4
Ehi = 4
Efermi = 6.1185
set xrange [0.0:1.18217]
set yrange [Elo:Ehi]

set xtics ("{/Symbol G}" 0.0,\
"X" 0.184162,\
"W" 0.276243,\
"K" 0.341354,\
"{/Symbol G}" 0.536688,\
"L" 0.696177,\
"U" 0.808952,\
"W" 0.874063,\
"L" 1.00429,\
"K|U" 1.11706,\
"X" 1.18217)

set arrow from      0.184162, Elo to      0.184162, Ehi nohead
set arrow from      0.276243, Elo to      0.276243, Ehi nohead
set arrow from      0.341354, Elo to      0.341354, Ehi nohead
set arrow from      0.536688, Elo to      0.536688, Ehi nohead
set arrow from      0.696177, Elo to      0.696177, Ehi nohead
set arrow from      0.808952, Elo to      0.808952, Ehi nohead
set arrow from      0.874063, Elo to      0.874063, Ehi nohead
set arrow from      1.004286, Elo to      1.004286, Ehi nohead
set arrow from      1.117061, Elo to      1.117061, Ehi nohead

set style line 1 lt 1 lw 1 pt 7 ps 0.3 lc rgb "black"
unset key

plot for[i=0:7] "segment1" index i u 1:($2-Efermi) w lines ls 1,\
for[i=0:7] "segment2" index i u 1:($2-Efermi) w lines ls 1,\
for[i=0:7] "segment3" index i u 1:($2-Efermi) w lines ls 1,\
for[i=0:7] "segment4" index i u 1:($2-Efermi) w lines ls 1,\
for[i=0:7] "segment5" index i u 1:($2-Efermi) w lines ls 1,\
for[i=0:7] "segment6" index i u 1:($2-Efermi) w lines ls 1,\
for[i=0:7] "segment7" index i u 1:($2-Efermi) w lines ls 1,\
for[i=0:7] "segment8" index i u 1:($2-Efermi) w lines ls 1,\
for[i=0:7] "segment9" index i u 1:($2-Efermi) w lines ls 1,\
for[i=0:7] "segment10" index i u 1:($2-Efermi) w lines ls 1
