kmeans-coreset
======================================================
[![Build Status](https://travis-ci.org/lpradel/kmeans-coreset.svg?branch=master)](https://travis-ci.org/lpradel/kmeans-coreset)

This is a heuristic C++ implementation of the constant-size coresets algorithm for *k*-means by Feldman, Schmidt and Sohler. The heuristic lies in the initial *k*-means++ sampling and the modified *k*-means++ sampling (*'D²-sampling'*) in the recursive component of the original algorithm. The implementation is based on the following paper of the authors:

**Dan Feldman, Melanie Schmidt, and Christian Sohler. 2013. Turning big data into tiny data: constant-size coresets for k-means, PCA and projective clustering. In Proceedings of the Twenty-Fourth Annual ACM-SIAM Symposium on Discrete Algorithms (SODA '13). Society for Industrial and Applied Mathematics, Philadelphia, PA, USA, 1434-1453.**

## Installation

The installation process of this algorithm relies on the CMake build tool. If you are unfamiliar with CMake, the necessary steps are listed below:

### CMake

- Change into the `build` directory of the project
- Run the command `cmake ..`
- You will find the compiled binaries for Debug and Release in the `build` directory depending on your specific CMake configuration

### Dependencies

The implementation itself has no library dependencies. It does however use the **C++11** standard.

You will have to ensure the presence of a suitable compiler for this (e.g. GCC or Clang).

## Usage

You can execute the algorithm over the command line interface:

```
./kmeans-coreset input k output
```

The arguments are as follows:
- `input`: The file containing your clustering data. Data points are separated by newlines and the dimensions of a data point are delimited by a comma (`,`). See `MATRIX_FILE_DELIMITER` in `Coreset.h`
- `k`: The number of centers and clusters
- `output`: The file to store the resulting coreset. The datapoints are formatted the same as in the `input` file

## Contributing

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request :)

## History

- Version 1.0: Initial release.

## Credits

- [Lukas Pradel](https://github.com/lpradel)

## License


    Copyright 2015 Lukas Pradel
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
      http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
