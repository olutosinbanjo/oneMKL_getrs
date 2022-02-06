#!bin/bash -f

#PBS -l nodes=1:gpu:ppn=2,walltime=12:00:00
#PBS -N getrs_usm_het
cd ~/oneMKL_getrs/bin/

echo SIZE=500
./getrs_usm_het 500
./getrs_usm_het 500
./getrs_usm_het 500
./getrs_usm_het 500
./getrs_usm_het 500

echo SIZE=2500
./getrs_usm_het 2500
./getrs_usm_het 2500
./getrs_usm_het 2500
./getrs_usm_het 2500
./getrs_usm_het 2500

echo SIZE=5000
./getrs_usm_het 5000
./getrs_usm_het 5000
./getrs_usm_het 5000
./getrs_usm_het 5000
./getrs_usm_het 5000

echo SIZE=10000
./getrs_usm_het 10000
./getrs_usm_het 10000
./getrs_usm_het 10000
./getrs_usm_het 10000
./getrs_usm_het 10000

