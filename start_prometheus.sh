#docker run -d -p 9092:9090 -v `pwd`/prometheus.yml:/etc/prometheus/prometheus.yml prom/prometheus
#!/bin/bash

CONTAINER_NAME="prometheus"
IMAGE_NAME="prom/prometheus"
CONFIG_FILE_PATH="`pwd`/prometheus.yml"
PORT_MAPPING="9092:9090"

# Check if container exists
if [ "$(docker ps -q -f name=$CONTAINER_NAME)" ]; then
    # Container exists, restart it
    docker restart $CONTAINER_NAME
else
    # Container does not exist, create and run it
    docker run -d --name $CONTAINER_NAME -p $PORT_MAPPING -v $CONFIG_FILE_PATH:/etc/prometheus/prometheus.yml $IMAGE_NAME
fi
