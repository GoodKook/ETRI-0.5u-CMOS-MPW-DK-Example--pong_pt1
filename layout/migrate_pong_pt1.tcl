box 0 0 0 0
drc off
snap int
lef read /usr/local/share/qflow/tech/etri050/etri050_stdcells.lef
def read pong_pt1
load pong_pt1
select top cell
select area labels
setlabel font FreeSans
setlabel size 0.3um
box grow s -[box height]
box grow s 100
select area labels
setlabel rotate 90
setlabel just e
select top cell
box height 100
select area labels
setlabel rotate 270
setlabel just w
select top cell
box width 100
select area labels
setlabel just w
select top cell
box grow w -[box width]
box grow w 100
select area labels
setlabel just e
writeall force pong_pt1
lef write pong_pt1 
expand
extract do local
extract all
ext2spice hierarchy on
ext2spice format ngspice
ext2spice scale off
ext2spice renumber off
ext2spice cthresh infinite
ext2spice rthresh infinite
ext2spice blackbox on
ext2spice subcircuit top auto
ext2spice global off
ext2spice
quit
