#!bin/bash -f

#PBS -l nodes=1:gpu:ppn=2,walltime=12:00:00
#PBS -N getrs_buf
cd ~/GITHUB/GETRS/bin/

echo SIZE=500
./getrs_buffer 500
./getrs_buffer 500
./getrs_buffer 500
./getrs_buffer 500
./getrs_buffer 500

echo SIZE=2500
./getrs_buffer 2500
./getrs_buffer 2500
./getrs_buffer 2500
./getrs_buffer 2500
./getrs_buffer 2500

echo SIZE=5000
./getrs_buffer 5000
./getrs_buffer 5000
./getrs_buffer 5000
./getrs_buffer 5000
./getrs_buffer 5000

echo SIZE=10000
./getrs_buffer 10000
./getrs_buffer 10000
./getrs_buffer 10000
./getrs_buffer 10000
./getrs_buffer 10000

