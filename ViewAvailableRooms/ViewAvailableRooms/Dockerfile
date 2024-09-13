# Используем образ на базе Ubuntu
FROM ubuntu:latest

# Устанавливаем необходимые пакеты
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    git \
    libpq-dev \
    libasio-dev \
    libssl-dev \
    libhiredis-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Устанавливаем рабочую директорию
WORKDIR /app

# Копируем исходный код и CMakeLists.txt в рабочую директорию
COPY . .

# Клонируем библиотеку Crow и redis-plus-plus
RUN git clone https://github.com/CrowCpp/Crow.git /app/Crow
RUN git clone https://github.com/sewenew/redis-plus-plus.git /app/redis++ && \
    mkdir -p /app/redis++/build && cd /app/redis++/build && cmake .. && make && make install

# Сборка проекта
RUN mkdir -p /app/build && cd /app/build && \
    cmake .. && \
    make

# Указываем порт, который будет использоваться приложением
EXPOSE 80

# Указываем команду для запуска приложения
CMD ["./build/CheckGym"]
