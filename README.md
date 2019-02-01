# multiarch-azure-iot-c
A multiarch container with a sample that connects to Azure IoTHub in C

### Build

```bash
$ ./build.sh
```

### Run

```bash
$ docker run -it --rm multiarch-azure-iot-c:x86_64
```

### Debug

Launch the docker container in a terminal with the following command

```bash
$ docker run -it --rm -v /Users/seank/Development/multiarch-azure-iot-c/sample:/src/sample2 -p 9090:9090 --cap-add=SYS_PTRACE --security-opt seccomp=unconfined multiarch-azure-iot-c:dev /bin/bash
```

Edit `CMakeLists.txt` to add the `-ggdb` flag to the compiler

From the bash shell in the container build the project and launch the debug server

```bash
$ cd ../sample2/
$ mkdir bin
$ cd bin
$ cmake ../
$ make
$ gdbserver :9090 ./sample_app 
```

Goto Debug tab in VS Code and select C++ Launch from the drop down and press `Start Debugging`

### Setup host debugging for VS Code

1. Install gdb
    ```bash
    $ brew install gdb --with-all-targets
    ```
2. Open `Keychain Access`
3. In the menu, open **Keychain Access > Certificate Assistant > Create a certificate**
4. Give it the name `gdbc`
    + Identity type: Self Signed Root
    + Certificate type: Code Signing
    + Check: let me override defaults
5. Continue until it prompts you for: "Please specify a keychain where the certificate will be stored."
6. Set Keychain: to `System`
7. Create a certificate and close assistant.
8. Find the certificate in System keychains, right click it > **get info** (or just double click it)
9.  Expand **Trust**, set **Code signing** to `always trust`
10. Close and Update
11. Finally, restart taskgated and sign from terminal:
    ```bash
    $ sudo killall taskgated
    $ sudo codesign -fs gdbc /usr/local/bin/gdb
    ```
