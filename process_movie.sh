#!/bin/bash

# Movie generation for the current single-layer model.
# Uses .gnu files written by System::writeGNU() in system.cpp.

set -euo pipefail

# Simulation parameters (aligned with process.sh)
for n in 10; do            # lattice size
for w in 1.0; do            # wall fraction
for p in 0.0; do            # wall crossing probability
for a in 0.0; do            # ant fraction
for k_d in 0.001 0.01 0.1; do  # degradation probabilities

	sweeps=1000           # total MC sweeps
	frame_sample=1       # write a frame every frame_sample sweeps (-S)
	fps=10                  # output movie frame rate

	s="movie_w_${w}_p_${p}_a_${a}_n_${n}_k_d_${k_d}"

	mkdir -p "movie/$s"
	cd "movie/$s"

	echo "Running simulation for $s"
	../../ANT --WALLF "$w" --PWALL "$p" -a "$a" -r 1 -s "$sweeps" -S "$frame_sample" -n "$n" -k_d "$k_d"

	echo "Rendering JPEG frames from GNU scripts..."
	shopt -s nullglob
	gnu_files=( *.gnu )
	if [ ${#gnu_files[@]} -eq 0 ]; then
		echo "No .gnu files found. Check that -S is set and writeGNU is being called."
		cd ../..
		continue
	fi

	# Render each generated gnuplot script (0.gnu, 1000.gnu, ...)
	for g in "${gnu_files[@]}"; do
		gnuplot "$g"
	done

	echo "Building movie..."
	ffmpeg -y -framerate "$fps" -start_number 0 -i %d.jpeg -c:v libx264 -pix_fmt yuv420p "${s}.mp4"

	echo "Cleaning intermediate frame files..."
	rm -f *.jpeg *.gnu *.dat

	cd ../..
	echo "Finished processing for $s"

done
done
done
done
done

echo "All movies created."