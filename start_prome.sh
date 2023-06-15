docker run -d -p 9092:9090 -v `pwd`/prometheus.yml:/etc/prometheus/prometheus.yml prom/prometheus
