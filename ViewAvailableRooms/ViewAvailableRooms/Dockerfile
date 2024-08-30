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
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Устанавливаем рабочую директорию
WORKDIR /app

# Клонируем библиотеку Crow
RUN git clone https://github.com/CrowCpp/Crow.git /app/Crow

# Клонируем и устанавливаем ASIO
RUN git clone https://github.com/chriskohlhoff/asio.git /app/asio

# Копируем исходный код и CMakeLists.txt в рабочую директорию
COPY . /app

# Сборка проекта
RUN mkdir -p /app/build && cd /app/build && \
    cmake .. && \
    make

#EXPOSE 5432
# Указываем команду для запуска приложения
CMD ["./build/CheckGym"]

#CMD ["./build/CheckGym"]