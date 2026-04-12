for n in 100; #size of the lattice
do
for w in 1.0; #wall fraction in bottom layer
do
for p in 0.0; #probability of crossing wall
do
for a in 0.1; #ant fraction
do
for k_d in 0.001; #number of MC steps in which a polymer row degrades
do
s="run_w_${w}_p_${p}_a_${a}_n_${n}" #name of the directory to store results
mkdir -p "$s" 
cd "$s"
sweeps=100000 #number of MC sweeps in simulation 
mcruns=100 #number of MC runs to average over
for ((r=1;r<=mcruns;r++)); 
do
    ../ANT --WALLF "$w" --PWALL "$p" -a "$a" -r "$r" -s "$sweeps" -S 10 -n "$n" -k_d "$k_d"
    mv "PolymerArea.txt" "PolymerArea_${r}.txt"
done
python3 ../Process.py "$mcruns" "$a" "$w" "$p" "$sweeps" "$n" #processing part of the code 
find . -maxdepth 1 -name "rand_*.dat" -print0 | xargs -0 -r rm #to remove per-run ant data files
find . -maxdepth 1 -name "PolymerArea_*.txt" -print0 | xargs -0 -r rm #to remove per-run area files
cd ..
done
done
done
done
done