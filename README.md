# ConcurrentPrintServiceAPI
This project is a simulation of a multithreaded printing service designed to demonstrate and explore key concepts in concurrent programming. It implements a multi-stage producer-consumer pattern involving multiple producers, multiple consumers, and the management of shared, finite resources.

- See React frontend [here](https://github.com/joshoti/ConcurrentPrintService) (Still private for now)

## Architecture
![Architecture diagram](docs/Architecture%20diagram.png)

## Local Environment Setup
To compile and run terminal version, execute the below command in the terminal window
```sh
make -s bin/cli && ./bin/cli
```

To see list of configurable options, execute the below command in the terminal window
```sh
make -s bin/cli && ./bin/cli -help
```