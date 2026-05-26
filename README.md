# BambooFilter

This project is a part of the [Bioinformatika 1](https://www.fer.unizg.hr/predmet/bio1) course on the Faculty of Electrical Engineering and Computing in Zagreb.

## Project Description

Implementation of the [Bamboo filter data structure proposed by Wang et al.](https://ieeexplore.ieee.org/document/10540052)

## Requirements
- C++20-compatible compiler
- C++20 standard
- CMake >= 3.10

## Running

### Genome from file
Put genome file in `tests/data`
``` sh
cd tests
./run.sh file <FILE> <K-MER> <COUNT>
```

### Random
``` sh
cd tests
./run.sh random <COUNT>
```

## Coding Style
This project follows the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

## Third-party
- [wyhash.h by Wang Yi](https://github.com/wangyi-fudan/wyhash)
- [timing.h by Wang Hancheng](https://github.com/wanghanchengchn/bamboofilters/tree/main)
- [random.h by Wang Hancheng](https://github.com/wanghanchengchn/bamboofilters/tree/main)
- [E. coli genome we tested on](https://www.ncbi.nlm.nih.gov/datasets/genome/GCF_030708175.1/)

## Team Members

- Ivan Dujmić
- Tia Rapo
