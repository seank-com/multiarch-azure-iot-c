#docker run --rm --privileged multiarch/qemu-user-static:register --reset
docker build -t multiarch-azure-iot-c:x86_64 -f Dockerfile.x86_64 .
docker build --target development -t multiarch-azure-iot-c:dev -f Dockerfile.x86_64 .

#docker build -t multiarch-azure-iot-c:arm64 -f Dockerfile.arm64 .
#docker build -t multiarch-azure-iot-c:armhf -f Dockerfile.armhf .
