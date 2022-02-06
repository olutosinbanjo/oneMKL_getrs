## Submit jobs to queue to obtain execution times

1. Clone this repository

```bash
git clone https://github.com/olutosinbanjo/oneMKL_getrs.git && cd job
```

2. Request a computational node

```bash
qsub -I -l nodes=1:gpu:ppn=2 
```

3. Submit job (for example)

```bash
qsub getrs_buf.sh
```
