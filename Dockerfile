FROM debian

COPY ssl-server /ssl-server
COPY key.pem /key.pem
COPY cert.pem /cert.pem

RUN apt-get update && apt-get install -y openssl vim

CMD ["/ssl-server"]
