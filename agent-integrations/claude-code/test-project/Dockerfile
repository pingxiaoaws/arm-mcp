FROM amd64/ubuntu:22.04

RUN apt-get update && apt-get install -y \
    g++ \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY main.cpp .
RUN g++ -O2 -mavx2 -mfma -o app main.cpp

CMD ["./app"]
